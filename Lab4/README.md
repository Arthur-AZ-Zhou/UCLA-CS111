# Hey! I'm Filing Here

In this lab, I successfully implemented the following 1 MiB ext2 file system with 2 directories (the mount and root), 1 regular file (the hello-world file), and 1 symbolic link (hello). I did this via creating a block group, beginning with the superblock in the write_superblock() function. The first function was quite simple as it was plugging in the correct definitions into the struct unsigned 32-bit integer values. Similar could be said about the write_block_group_descriptor_table() function as it was simply plugging in the right definitions into block_group_descriptor structure values. The writing into bitmaps was a bit tougher as it required writing into the map values, populating the entire map_value[BLOCK_SIZE] array with hex values. As for writing to the inode table, I simply wrote the permissions and the link counts to match the total result shown in the end and in the screenshots included in the slideshow from dicsussion, such as matching the file permissions. Finally, writing the write_root_dir_block() and the write_hello_world_file_block() functions was quite simple to match the names of directory and the hello world text respectively. However the "hello world" had to be written in hex in big endian. 

## Building

Run "make" to compile the executable, creating executable file "ext2-create". We then run "ext2-create" via "./ext2-create" to create base image "cs111-base.img". 

## Running

We can use "dumpe2fs cs111-base.img" to debug possible errors in the base image data, such as the bitmaps and other superblock data. If everything is according to plan, we run "fsck.ext2 cs111-base.img" to check if the filesystem is correct. We should see something like this with all 5 tests passed: 

e2fsck 1.46.4 (18-Aug-2021)
cs111-base has gone 0 days without being checked, check forced.
Pass 1: Checking inodes, blocks, and sizes
Pass 2: Checking directory structure
Pass 3: Checking directory connectivity
Pass 4: Checking reference counts
Pass 5: Checking group summary information
cs111-base: 13/128 files (0.0% non-contiguous), 24/1024 blocks

We can then create the mount using "mkdir mnt" and mounting filesystem via "sudo mount -o loop cs111-base.img mnt". We can then cd into the mount and run "ls -ain" to see:

total 7
    2 drwxr -xr-x 3 0 0 1024 .
                             ..
    13 lrw-r--r-- 1 1000 1000 11 hello -> hello -world
    12 -rw-r--r-- 1 1000 1000 12 hello -world
    11 drwxr -xr-x 2 0 0 1024 lost+found

"cat hello-world" and "cat hello" should both print out "hello world" to the terminal.

## Cleaning up

We can unmount the filesystem via "sudo umount mnt" and then remove the mount to delete the directory. Then we do "make clean" to clean up the rest of the executable files.
