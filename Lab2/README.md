# You Spin Me Round Robin

The task is to schedule processes sharing one CPU and one core in the round-robin style, letting each task run for a certain amount of time (the quantum) and then
letting another process take a turn to establish fairness that all processes get to run.

## Building

```shell
The process was complicated. I began by adding a wait, response, remaining, and starting time for each process as other variables in the process struct. To establish the main routine, I began by initializing the variables and first processes and had a while loop so that while there were still processes in the queue the code will see if it's the first time the process has ran. If it is we set its response time and remaining time accordingly. Afterwards we add new processes to the end of the list and see if there's time remaining for the task at hand and if there is we put it to the end of the queue. If there isn't we add the process' waiting time to the total and move onto the next task.
```

## Running

cmd for running TODO
```shell
./rr processes.txt quantum_time
where ./rr is the calling of the round robin executable file and processes.txt is the file to run. You can choose to put a quantum_time or you can type median to let the code calculate a dynamic quantum time based on the median of all CPU times consumed so far 
```

results TODO
```shell
The original processes.txt nets an Average Wait Time of 75.00 and an Average Response Time of 29.25 including the context switch times
The median has an Average Wait Time of 88.25 and Average Response Time of 4.50

```

## Cleaning up

```shell
The command "make clean" should be enough to get rid of the executable files, allowing you a chance to remake and compile the proper files again.
```
