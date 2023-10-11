#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

// TODO: Add more fields to this struct
struct job
{
  int id;
  int arrival;
  int length;
  int isChecked;
  int tickets;
  int startTime;
  int startFlag;
  int remainingTime;

  int completionTime;
  int responseTime;
  int turnaroundTime;
  int waitTime;

  struct job *next;
};

/*** Globals ***/
int seed = 100;

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;
struct job *head2 = NULL;
static int count = 0;

/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length, int tickets)
{
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job *)malloc(sizeof(struct job));

  // tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->tickets = tickets;
  tmp->isChecked = 0;
  tmp->remainingTime = length;
  tmp->startFlag = 0;

  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty
  if (head == NULL)
  {
    head = tmp;
    return;
  }

  struct job *prev = head;

  // Find end of list
  while (prev->next != NULL)
  {
    prev = prev->next;
  }

  // Add job to end of list
  prev->next = tmp;
  return;
}

/*Function to read in the workload file and create job list*/
void read_workload_file(char *filename)
{
  int id = 0;
  FILE *fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL,
       *arrival = NULL,
       *length = NULL;
  int tickets = 0;

  struct job **head_ptr = malloc(sizeof(struct job *));

  if ((fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1)
  {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");
    tickets += 100;

    // Make sure neither arrival nor length are null.
    assert(arrival != NULL && length != NULL);

    append(id++, atoi(arrival), atoi(length), tickets);
    count++;
  }

  fclose(fp);

  // Make sure we read in at least one job
  assert(id > 0);

  return;
}

int get_total_runtime(struct job *head)
{
  int total = 0;
  struct job *curr = head;
  while (curr != NULL)
  {
    total += curr->length;
    curr = curr->next;
  }
  return total;
}

void policy_STCF(struct job *head, int slice)
{
  printf("Execution trace with STCF:\n");
  struct job *shortestJob = head;
  int completeJobs = 0, minM = INT_MAX, time = 0;
  // iterate through linked list and add to data array
  while (completeJobs != count)
  {
    for (struct job *curr = head; curr != NULL; curr = curr->next)
    {
      // find the shortest job that has arrived
      if (curr->arrival <= time && curr->remainingTime < minM && curr->isChecked == 0)
      {
        minM = curr->remainingTime;
        shortestJob = curr;
      }
    }

    // update shortestJob startTime
    if (shortestJob->startFlag == 0)
    {
      shortestJob->startTime = time;
      shortestJob->startFlag = 1;
    }

    shortestJob->remainingTime -= slice;
    minM = shortestJob->remainingTime;
    if (minM <= 0)
    {
      minM = INT_MAX;
    }
    int runTime = slice;
    // might not be necessary... Since time is in slices, we can just add the remaining time
    if (shortestJob->remainingTime < slice)
    {
      runTime = slice - shortestJob->remainingTime;
    }
    // if it is necessary, then completionTime = time + runTime
    if (shortestJob->remainingTime <= 0)
    {
      completeJobs++;
      shortestJob->isChecked = 1;
      shortestJob->completionTime = time + slice;
      // printf("[Job %d] shortestJob->completionTime: %d\n", shortestJob->id, shortestJob->completionTime);
    }
    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, shortestJob->id, shortestJob->arrival, runTime);
    time += slice;
  }

  printf("End of execution with STCF.\n");
  return;
}

void analyze_STCF(struct job *head)
{
  // TODO: Fill this in
  char *data[count];

  // int startTime = 0;
  int arrivalTime = 0;
  // int completionTime = 0;
  int responseTime = 0;
  int turnaroundTime = 0;
  int waitTime = 0;

  int responseSum = 0;
  int turnaroundSum = 0;
  int waitSum = 0;

  // add to data array
  for (struct job *curr = head; curr != NULL; curr = curr->next)
  {
    // calculate reponseTime
    arrivalTime = curr->arrival;
    // startTime = curr->startTime;
    responseTime = curr->startTime - arrivalTime;

    // calculate turnaroundTime
    // completionTime = startTime + curr->length;
    turnaroundTime = curr->completionTime - arrivalTime;

    // calculate waitTime
    waitTime = turnaroundTime - curr->length;
    char temp[100];

    snprintf(temp, sizeof(temp), "Job %d -- Response time: %d Turnaround: %d Wait: %d\n", curr->id, responseTime, turnaroundTime, waitTime);
    data[curr->id] = strdup(temp);
    responseSum += responseTime;
    turnaroundSum += turnaroundTime;
    waitSum += waitTime;
  }
  // print array
  for (int j = 0; j < count; j++)
  {
    printf("%s", data[j]);
    free(data[j]);
  }
  printf("Average -- Response time: %.2f Turnaround: %.2f Wait: %.2f\n", (double)responseSum / count, (double)turnaroundSum / count, (double)waitSum / count);

  return;
}

void policy_RR(struct job *head, int slice)
{
  printf("Execution trace with RR:\n");
  int total_runtime = get_total_runtime(head);
  struct job *curr = head;
  int time = 0;

  while (total_runtime > 0)
  {
    if (curr->arrival > time)
    {
      time = curr->arrival;
    }
    int temp = curr->length - slice;
    if (temp > 0)
    {
      printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, curr->id, curr->arrival, curr->length);
      curr->length = temp;
      total_runtime -= slice;
      time += slice;
    }
    else
    {
      printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, curr->id, curr->arrival, curr->length);
      total_runtime -= curr->length;
      time += curr->length;
      if (curr->next != NULL && curr->next->arrival <= time)
      {
        curr = curr->next;
      }
      else
      {
        curr = head;
      }
    }
  }

  printf("End of execution with RR.\n");
}

void analyze_RR(struct job *head)
{
  // TODO: Fill this in
}

void policy_LT(struct job *head, int slice)
{
  printf("Execution trace with LT:\n");

  int time = 0;

  while (head != NULL)
  {
    // calculate the total number of tickets
    int totalTickets = 0;
    struct job *curr = head;
    while (curr != NULL)
    {
      totalTickets += curr->tickets;
      curr = curr->next;
    }

    // generate a random number between 0 and totalTickets
    int winningTicket = rand() % totalTickets + 1;

    // find the job with the winning ticket by traversing the list until the winning ticket is found
    struct job *curr2 = head;
    while (curr2 != NULL)
    {
      winningTicket -= curr2->tickets;
      if (curr2 <= 0)
      {
        if (curr2->length > slice)
        {
          printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, curr->id, curr->arrival, curr->length);
          curr2->length -= slice;
          time += slice;
        }
        else
        {
          printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, curr->id, curr->arrival, curr->length);
          time += curr->length;
          curr2->length = 0;
          curr2->isChecked = 1;
        }
      }

      // remove the job from the list if it has finished executing
      if (curr2->length == 0)
      {
      }
    }
  }

  printf("End of execution with LT.\n");
}

void analyze_LT(struct job *head)
{
}

int main(int argc, char **argv)
{

  if (argc < 5)
  {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file slice-length\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];
  int slice = atoi(argv[4]);

  // Note: we use a global variable to point to
  // the start of a linked-list of jobs, i.e., the job list
  read_workload_file(workload);

  if (strcmp(policy, "STCF") == 0)
  {
    policy_STCF(head, slice);
    if (analysis)
    {
      printf("Begin analyzing STCF:\n");
      analyze_STCF(head);
      printf("End analyzing STCF.\n");
    }
    struct job *curr = head;
    // free memory
    while (curr != NULL)
    {
      struct job *temp = curr->next; // Store the next node before freeing the current one
      free(curr);
      curr = temp; // Move to the next node
    }

    exit(EXIT_SUCCESS);
  }

  // TODO: Add other policies
  if (strcmp(policy, "RR") == 0)
  {
    policy_RR(head, slice);
    if (analysis)
    {
      printf("Begin analyzing RR:\n");
      analyze_RR(head);
      printf("End analyzing RR.\n");
    }
    // free memory
    struct job *curr = head;
    while (curr != NULL)
    {
      struct job *temp = curr->next; // Store the next node before freeing the current one
      free(curr);
      curr = temp; // Move to the next node
    }

    exit(EXIT_SUCCESS);
  }
  if (strcmp(policy, "LT") == 0)
  {
    policy_LT(head, slice);
    if (analysis)
    {
      printf("Begin analyzing LT:\n");
      analyze_LT(head);
      printf("End analyzing LT.\n");
    }
    // free memory
    struct job *curr = head;
    while (curr != NULL)
    {
      struct job *temp = curr->next; // Store the next node before freeing the current one
      free(curr);
      curr = temp; // Move to the next node
    }

    exit(EXIT_SUCCESS);
  }
  exit(EXIT_SUCCESS);
}
