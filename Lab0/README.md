# A Kernel Seedling
TODO: I began by installing the correct version of VirtualBox for Windows 11 and the OS image. It took a while since the image was around 6 GB but afterwards I
was able to get the CS111 environment set up. The sizing of the screen was a bit strange on my computer but I made a few edits and everything worked out in the
end.

## Building
```shell
TODO: make makes the correct files and processes to run while sudo insmod proc_count.ko inserts proc_count.ko module.
```

## Running
```shell
TODO: cat /proc/count runs the proc_count() function and prints out the supposed number of processes
```
TODO: results?

## Cleaning Up
```shell
TODO: sudo rmmod proc_count to remove the module, then use make clean to remove everything else made by the initial make function.
```

## Testing
```python
python -m unittest
```
TODO: At first I was getting all 3 tests as fails, then turns out my formatting was wrong and I made a few changes and got all 3 correct.

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```
TODO: 5.14.8-arch1-1