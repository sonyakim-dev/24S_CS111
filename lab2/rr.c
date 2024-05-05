#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  // linked list pointers - prev and next
  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 waiting_time;
  u32 response_time;
  u32 remain_time;
  bool responded;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}

int compare_arrival_time(const void *a, const void *b) {
    struct process *processA = (struct process *)a;
    struct process *processB = (struct process *)b;
    return (processA->arrival_time - processB->arrival_time);
}


int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data; // array of processes
  u32 size; // number of processes
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list; // linked list of processes
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */
  if (quantum_length <= 0) return EINVAL;

  // sort the processes by arrival time
  qsort(data, size, sizeof(struct process), compare_arrival_time);

  u32 curr_time = 0;
  u32 completed = 0; // the number of completed processes
  u32 i = 0; // the index of the next process to arrive
  struct process *prev_process = NULL;

  while (completed < size)
  {
    // check if there is a process that has arrived
    while (i < size && data[i].arrival_time <= curr_time)
    {
      struct process *new_process = &data[i];
      if (new_process->burst_time > 0) {
        new_process->remain_time = new_process->burst_time;
        new_process->responded = false;
        TAILQ_INSERT_TAIL(&list, new_process, pointers);
      }
      i++;
    }

    // push back the previous process
    if (prev_process != NULL) {
      TAILQ_INSERT_TAIL(&list, prev_process, pointers);
    }

    // printf("Time: %d\n", curr_time);
    // for (struct process *p = TAILQ_FIRST(&list); p != NULL; p = TAILQ_NEXT(p, pointers)) {
    //   printf("PID: %d, Arrival Time: %d, Burst Time: %d, Remaining Time: %d\n", p->pid, p->arrival_time, p->burst_time, p->remain_time);
    // }

    if (!TAILQ_EMPTY(&list)) {
      // pop front
      struct process *curr_process = TAILQ_FIRST(&list);
      TAILQ_REMOVE(&list, curr_process, pointers);

      // check if the process has responded
      if (!curr_process->responded) {
        curr_process->response_time = curr_time - curr_process->arrival_time;
        total_response_time += curr_process->response_time;
        curr_process->responded = true;
        // printf("PID: %d, Response Time: %d\n", curr_process->pid, curr_process->response_time);
      }

      // if curr_process can be completed within the quantum
      if (curr_process->remain_time <= quantum_length) {
        curr_time += curr_process->remain_time;
        curr_process->remain_time = 0;
        curr_process->waiting_time = curr_time - curr_process->arrival_time - curr_process->burst_time;
        total_waiting_time += curr_process->waiting_time;
        prev_process = NULL;
        completed++;
        // printf("PID: %d, Waiting Time: %d\n", curr_process->pid, curr_process->waiting_time);
      }
      // if curr_process cannot be completed within the quantum
      else {
        curr_time += quantum_length;
        curr_process->remain_time -= quantum_length;
        prev_process = curr_process;
      }
    }
    else {
      curr_time++;
    }
  }
  /* End of "Your code here" */
  
  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
