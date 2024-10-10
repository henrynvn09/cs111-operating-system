# A Kernel Seedling
Setting up a virtual machine. Then creating a command that count the number of running processes that is exposed as /proc/count

## Building
```shell
make
sudo insmod proc_count.ko
```

## Running
```shell
cat /proc/count
```
88

## Cleaning Up
```shell
sudo rmmod proc_count
make clean
```

## Testing
```python
python -m unittest
```
TODO: results?

```
...
----------------------------------------------------------------------
Ran 3 tests in 11.049s

OK
```

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```

Linux 5.14.8-arch1-1 #1 SMF PREEMPT Sun, 26 Sep 2021 19:36:15 +0000