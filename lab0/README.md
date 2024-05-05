# A Kernel Seedling

- create kernel module
- create _/proc/count_ file that shows the current number of running process

## Building

```shell
make
sudo insmod proc_count.ko
```

## Running

```shell
cat /proc/count
```

Results: `80`

## Cleaning Up

```shell
sudo rmmod proc_count.ko
```

## Testing

```shell
python -m unittest
```

Results:

```
...
-------------------------------------------
Ran 3 tests in 6.133s

OK
```

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -rsv
```

Kernel Version: `Linux 5.14.8-arch1-1 #1 SMP PREEMPT Sun, 26 Sep 2021 19:36:15 +0000`
