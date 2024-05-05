## UID: 106292803

## Pipe Up

- A program that accepts multiple commands as arguments and executes them in a pipeline using the pipe, fork, and execlp system calls.

## Building

```shell
make
```

## Running

#### Example 1

Command: `./pipe ls` <br/>
Output:

```shell
Makefile  README.md  __pycache__  pipe  pipe.c  pipe.o  test_lab1.py
```

#### Example 2

Command: `./pipe ls head wc` <br/>
Output:

```shell
      7       7      63
```

#### Example 3

Command: `./pipe cat < input.txt uniq sort tail wc` <br/>
Output:

```shell
     10      10      28
```

## Cleaning up

```shell
make clean
```
