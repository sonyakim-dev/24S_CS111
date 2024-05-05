# You Spin Me Round Robin

- Demonstrate Round Robin scheduling algorithm using queue(linked list)

## Building

```shell
make
```

## Running

```shell
./rr [INPUT_FILE] [QUANTUM_LEN]
```

#### Example 1

Processes.txt

```
4
1, 0, 7
2, 2, 4
3, 4, 1
4, 5, 4
```

Command: `./rr processes.txt 1` <br/>
Output:

```
  Average waiting time: 5.50
  Average response time: 0.75
```

Command: `./rr processes.txt 3` <br/>
Output:

```
  Average waiting time: 7.00
  Average response time: 2.75
```

#### Example 2

Processes.txt

```
4
1, 1, 3
2, 4, 4
3, 4, 1
4, 5, 4
```

Command: `./rr processes.txt 2` <br/>
Output:

```
  Average waiting time: 2.25
  Average response time: 1.00
```

## Cleaning up

```shell
make clean
```
