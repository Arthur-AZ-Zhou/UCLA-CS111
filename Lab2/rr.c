#include <fcntl.h>
#include <stdbool.h>
#include <stdckdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <unistd.h>

/* A process table entry.  */
struct process {
    long pid;
    long arrival_time;
    long burst_time;

    TAILQ_ENTRY (process) pointers;

    /* Additional fields here */
    int ID; //no guarantee PID is in chronological order or won't overflow array
    bool started; //false by default
    long wait_time;
    long response_time;
    long remaining_time;
    /* End of "Additional fields here" */
};

TAILQ_HEAD (process_list, process);

/* Skip past initial nondigits in *DATA, then scan an unsigned decimal
   integer and return its value.  Do not scan past DATA_END.  Return
   the integer’s value.  Report an error and exit if no integer is
   found, or if the integer overflows.  */
static long next_int (char const **data, char const *data_end) {
    long current = 0;
    bool int_start = false;
    char const *d;

    for (d = *data; d < data_end; d++) {
        char c = *d;
        if ('0' <= c && c <= '9') {
            int_start = true;
            if (ckd_mul (&current, current, 10) || ckd_add (&current, current, c - '0')) {
                fprintf (stderr, "integer overflow\n");
                exit (1);
            }
        } else if (int_start)
            break;
    }

    if (!int_start) {
        fprintf (stderr, "missing integer\n");
        exit (1);
    }

    *data = d;
    return current;
}

/* Return the first unsigned decimal integer scanned from DATA.
   Report an error and exit if no integer is found, or if it overflows.  */
static long next_int_from_c_str (char const *data) {
    return next_int (&data, strchr (data, 0));
}

/* A vector of processes of length NPROCESSES; the vector consists of
   PROCESS[0], ..., PROCESS[NPROCESSES - 1].  */
struct process_set {
    long nprocesses;
    struct process *process;
};

/* Return a vector of processes scanned from the file named FILENAME.
   Report an error and exit on failure.  */
static struct process_set init_processes (char const *filename) {
    int fd = open (filename, O_RDONLY);
    if (fd < 0) {
        perror ("open");
        exit (1);
    }

    struct stat st;
    if (fstat (fd, &st) < 0) {
        perror ("stat");
        exit (1);
    }

    size_t size;
    if (ckd_add (&size, st.st_size, 0)) {
        fprintf (stderr, "%s: file size out of range\n", filename);
        exit (1);
    }

    char *data_start = mmap (NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data_start == MAP_FAILED) {
        perror ("mmap");
        exit (1);
    }

    char const *data_end = data_start + size;
    char const *data = data_start;

    long nprocesses = next_int (&data, data_end);
    if (nprocesses <= 0) {
        fprintf (stderr, "no processes\n");
        exit (1);
    }

    struct process *process = calloc (sizeof *process, nprocesses);
    if (!process) {
        perror ("calloc");
        exit (1);
    }

    for (long i = 0; i < nprocesses; i++) {
        process[i].pid = next_int (&data, data_end);
        process[i].arrival_time = next_int (&data, data_end);
        process[i].burst_time = next_int (&data, data_end);
        if (process[i].burst_time == 0) {
            fprintf (stderr, "process %ld has zero burst time\n",
            process[i].pid);
            exit (1);
        }
    }

    if (munmap (data_start, size) < 0) {
        perror ("munmap");
        exit (1);
    }

    if (close (fd) < 0) {
        perror ("close");
        exit (1);
    }

    return (struct process_set) {nprocesses, process};
}

int compare(const void * e1, const void * e2) { //sort array compare function
    long f = *((long*) e1);
    long s = *((long*) e2);

    if (f > s) 
        return 1;
    if (f < s) 
        return -1;

    return 0;
}

int compare_arrival_time(const void *e1, const void *e2) { //compare function for stable sort list array
    struct process f = *((struct process*) e1);
    struct process s = *((struct process*) e2);

    if (f.arrival_time > s.arrival_time)
        return 1;

    if (f.arrival_time < s.arrival_time)
        return -1;

    return 0;
}

int main (int argc, char *argv[]) {
    if (argc != 3) {
        fprintf (stderr, "%s: usage: %s file quantum\n", argv[0], argv[0]);
        return 1;
    }

    struct process_set ps = init_processes (argv[1]);
    long quantum_length = (strcmp (argv[2], "median") == 0 ? -1 : next_int_from_c_str (argv[2]));
    if (quantum_length == 0) {
        fprintf (stderr, "%s: zero quantum length\n", argv[0]);
        return 1;
    }

    struct process_list list;
    TAILQ_INIT (&list);

    long total_wait_time = 0;
    long total_response_time = 0;

    /* Your code here */
    int num_processes_active = 0;
    int num_processes_ran = 0;
    int current_index = 1;
    bool quantum_dynamic = ((quantum_length == -1)? true : false);
    qsort(ps.process, ps.nprocesses, sizeof(struct process), compare_arrival_time); 

    struct process *first = &ps.process[0];
    int current_time = first->arrival_time;
    long* active_processes_runtime = (long*) malloc(ps.nprocesses * sizeof(long)); //array of active processes to sort

    TAILQ_INSERT_TAIL(&list, first, pointers); //insert first process into end of linked list

    while (num_processes_ran < ps.nprocesses) {
        /*if (quantum_dynamic == true) { //HAVEN'T TESTED
            if (quantum_length <= 0) {
                quantum_length = 1;
            } else { //sort array and find middle
                long* temp_array = (long*) malloc(ps.nprocesses * sizeof(long)); //save original array
                for (int i = 0; i < num_processes_active; i++) //copy original array
                    temp_array[i] = active_processes_runtime[i];
                
                qsort(active_processes_runtime, num_processes_active, sizeof(long), compare); 
                int median = 0; //placeholder
                
                if (num_processes_active % 2 == 0) { //cancer scenario
                    if (num_processes_active != 0) {
                        int second_half_index = num_processes_active / 2;
                        int first_half_index = second_half_index - 1;
                        double decimal_median = ((double) (active_processes_runtime[first_half_index] + active_processes_runtime[second_half_index])) / 2;

                        if (decimal_median % 1.0 != 0.0) {
                            int lower = decimal_median;
                            int higher = decimal_median + 1;

                            if (lower % 2 == 0) { //floor is even
                                median = lower;
                            } else {
                                median = higher;
                            }
                        } else {
                            median = decimal_median;
                        }
                    }
                } else { //odd # of processes
                    median = active_processes_runtime[num_processes_active / 2];
                }

                if (median == 0)
                    median = 1;

                quantum_length = median;
                active_processes_runtime = temp_array;
                // printf("quantum_length/median: %ld\n", quantum_length);
            }
        } */

        struct process *current_process = TAILQ_FIRST(&list);
        printf("Process %ld executes at: %d \n", current_process->pid, current_time);
        for (int i = 0; i < num_processes_active; i++) {
            printf("%ld, ", active_processes_runtime[i]);
        }
        printf("quantum_length: %ld \n", quantum_length);

        if (current_process->started == false) { //first time process
            current_process->started = true;
            current_process->ID = num_processes_ran;
            num_processes_active++;
            num_processes_ran++;
            current_process->remaining_time = current_process->burst_time;
            current_process->response_time = current_time - current_process->arrival_time;
            total_response_time += current_process->response_time;
            printf("Process %ld response_time: %ld | Current total_response_time: %ld \n", current_process->pid, current_process->response_time, total_response_time);
        }

        int runtime = (current_process->remaining_time > quantum_length)? quantum_length : current_process->remaining_time; //min of quantum and remaining time
        int after_runtime = current_time + runtime;

        while (current_index < ps.nprocesses && ps.process[current_index].arrival_time < after_runtime) { //while current task is running put other tasks to queue
            struct process *next_process = &ps.process[current_index];
            TAILQ_INSERT_TAIL(&list, next_process, pointers);
            printf("Process %ld arrived at: %ld \n", (&ps.process[current_index])->pid, (&ps.process[current_index])->arrival_time);
            current_index++;
            current_time = next_process->arrival_time;
        }

        printf("remaining_time: %ld \n", current_process->remaining_time);
        current_process->remaining_time -= runtime;
        printf("remaining_time after runtime: %ld \n", current_process->remaining_time);
        //NEED TO UPDATE ACTIVE PROCESS LIST FOR THE DYNAMIC VERSION
        current_time = after_runtime;

        TAILQ_REMOVE(&list, current_process, pointers);

        if (current_process->remaining_time > 0) { //add process to end if they have time left
            TAILQ_INSERT_TAIL(&list, current_process, pointers);
        } else { //otherwise get total wait time - context switch
            printf("Process %ld DONE, wait_time: %ld \n", current_process->pid, current_time - current_process->arrival_time - current_process->burst_time);
            total_wait_time += (current_time - current_process->burst_time - current_process->arrival_time);
        }

        if (TAILQ_EMPTY(&list) && num_processes_ran < ps.nprocesses) { //process that arrive after
            struct process *next_process = &ps.process[current_index];
            TAILQ_INSERT_TAIL(&list, next_process, pointers);
            printf("Process %ld arrived at: %ld \n", (&ps.process[current_index])->pid, (&ps.process[current_index])->arrival_time);
            current_index++;
            current_time = next_process->arrival_time;
        }

        if (TAILQ_FIRST(&list) != current_process) 
            current_time++;
        
        printf(">>>current_time<<< %d\n", current_time);
    }

    // for (long i = 0; i < ps.nprocesses; i++) {
    //     printf("PID: %ld ", ps.process[i].pid);
    //     printf("Arrival Time: %ld ", ps.process[i].arrival_time);
    //     printf("Burst Time: %ld ", ps.process[i].burst_time);
    //     printf("\n");
    // }

    /* End of "Your code here" */

    printf ("Average wait time: %.2f\n", total_wait_time / (double) ps.nprocesses);
    printf ("Average response time: %.2f\n", total_response_time / (double) ps.nprocesses);

    if (fflush (stdout) < 0 || ferror (stdout)) {
        perror ("stdout");
        return 1;
    }

    free (ps.process);
    return 0;
}
