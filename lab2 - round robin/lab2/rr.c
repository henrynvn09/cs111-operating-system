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

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 time_left;
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

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */
  // for (int i = 0; i < size; i++)
  // {
  //   struct process* new_process = (struct process*)malloc(sizeof(struct process));
  //   new_process->pid = data[i].pid;
  //   new_process->arrival_time = data[i].arrival_time;
  //   new_process->burst_time = data[i].burst_time;
  //   new_process->time_left = data[i].burst_time;

  //   TAILQ_INSERT_TAIL(&list, new_process, pointers);
  // }

  if (size == 0) {
    perror("No process to run");
    return EINVAL;
  }
  if (quantum_length == 0) {
    perror("Quantum length must be greater than 0");
    return EINVAL;
  }

  struct process* current_process = TAILQ_FIRST(&list); // should be the NULL


  // sort the process by arrival time
  for (u32 i = 0; i < size; i++)
  {
    for (u32 j = i+1; j < size; j++)
    {
      if (data[i].arrival_time > data[j].arrival_time) {
        struct process temp = data[i];
        data[i] = data[j];
        data[j] = temp;
      }
    }
  }
  
  

  u32 clock_time = 0;
  u32 loaded_processes = 0;


  while (true)
  {
    // printf("========== separator ==========\n");
    // if the current clock time is >= the arrival time of all queued processes
    while (loaded_processes < size && clock_time >= data[loaded_processes].arrival_time) {
      struct process* new_process = (struct process*)malloc(sizeof(struct process));
      new_process->pid = data[loaded_processes].pid;
      new_process->arrival_time = data[loaded_processes].arrival_time;
      new_process->burst_time = data[loaded_processes].burst_time;
      new_process->time_left = data[loaded_processes].burst_time;

      TAILQ_INSERT_TAIL(&list, new_process, pointers);
      loaded_processes++;
      // printf("Process %d is loaded\n", new_process->pid);
    }

    // if it did work last clock
    if (current_process != NULL) {
      struct process* last_process = current_process;
      current_process = TAILQ_NEXT(current_process, pointers);
      
      // add the last process back to the tail of the queue if it is not done
      if (last_process->time_left > 0) {
        TAILQ_REMOVE(&list, last_process, pointers);
        TAILQ_INSERT_TAIL(&list, last_process, pointers);
      }
      else {
        // printf("remove process from the list %d\n", last_process->pid);
        TAILQ_REMOVE(&list, last_process, pointers);
        free(last_process);
      }
    }

    TAILQ_FOREACH(current_process, &list, pointers) {
      // printf("QUEUE: Process %d, time left: %d\n", current_process->pid, current_process->time_left);
    }

    // ========== execute the process ==========
    
    // if the current process is null
    if (current_process == NULL) {
      // printf("point current_process back to head \n");
      current_process = TAILQ_FIRST(&list);

    }
    // if the list is empty or all work is done
    if (current_process == NULL) {
      // printf("no more process in queue\n");
      // in case there is time that no process is running
      if (loaded_processes < size) {
        clock_time += quantum_length;
        continue;
      }
      
      break;
    }

    // printf("Clock time: %d = Process %d, arrival time: %d, burst time: %d, time left: %d\n", clock_time, current_process->pid, current_process->arrival_time, current_process->burst_time, current_process->time_left);

    // if it is the first time process run, calculate the response time
    if (current_process->time_left == current_process->burst_time) {
      total_response_time += clock_time - current_process->arrival_time;
    }
    
    // do work for the current process
    if (current_process->time_left <= quantum_length) {
      // printf("Process %d is done\n", current_process->pid);
      clock_time += current_process->time_left;
      current_process->time_left = 0;

      total_waiting_time += clock_time - current_process->arrival_time - current_process->burst_time;
    }
    else {
      // printf("Process %d is not done\n", current_process->pid);
      clock_time += quantum_length;
      current_process->time_left -= quantum_length;
    }

    // printf("Clock time: %d = Process %d, arrival time: %d, burst time: %d, time left: %d\n", clock_time, current_process->pid, current_process->arrival_time, current_process->burst_time, current_process->time_left);
  }
  
  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
