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
  struct job *next;
};

/*** Globals ***/
int seed = 100;

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;
static int count = 0;
/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length)
{
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job *)malloc(sizeof(struct job));

  // tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->isChecked = 0;

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

  if ((fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1)
  {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");

    // Make sure neither arrival nor length are null.
    assert(arrival != NULL && length != NULL);

    append(id++, atoi(arrival), atoi(length));
    count++;
  }

  fclose(fp);

  // Make sure we read in at least one job
  assert(id > 0);

  return;
}

void policy_FIFO(struct job *head)
{
  // iterate through linked list and add to data array
  int time = 0;
  printf("Begin FIFO:\n");
  for (struct job *curr = head; curr != NULL; curr = curr->next)
  {
    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, curr->id, curr->arrival, curr->length);
    time += curr->length;
  }
  printf("End FIFO.\n");
  return;
}

void analyze_FIFO(struct job *head)
{
  printf("Begin analyzing FIFO:\n ");
  int startTime = 0;
  int arrivalTime = 0;
  int completionTime = 0;
  int responseTime = 0;
  int turnaroundTime = 0;
  int waitTime = 0;

  int responseSum = 0;
  int turnaroundSum = 0;
  int waitSum = 0;

  int time = 0;
  for (struct job *curr = head; curr != NULL; curr = curr->next)
  {
    // calculate reponseTime
    arrivalTime = curr->arrival;
    startTime = time;
    responseTime = startTime - arrivalTime;

    // calculate turnaroundTime
    completionTime = time + curr->length;
    turnaroundTime = completionTime - arrivalTime;

    // calculate waitTime
    waitTime = turnaroundTime - curr->length;

    printf("Job %d -- Response time: %d Turnaround: %d Wait: %d\n", curr->id, responseTime, turnaroundTime, waitTime);
    time += curr->length;
    responseSum += responseTime;
    turnaroundSum += turnaroundTime;
    waitSum += waitTime;
  }
  printf("Average -- Response time: %.2f Turnaround: %.2f Wait: %.2f\n", (double)responseSum / count, (double)turnaroundSum / count, (double)waitSum / count);
  printf("End analyzing FIFO.\n ");
}

void policy_SJF()
{
  printf("Execution trace with SJF:\n");
  int time = 0;
  struct job *curr;
  struct job *shortestJob = head;
  int nextArrivalTime = 0;
  for (int i = 0; i < count; i++)
  {
    curr = head->next;
    shortestJob = head;
    while (curr)
    {
      if ((curr->arrival <= time) && (curr->length < shortestJob->length) && (curr->isChecked == 0))
      {
        shortestJob = curr;
        if (curr->next != NULL)
        {
          nextArrivalTime = curr->next->arrival;
        }
      }
      curr = curr->next;
    }

    printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, shortestJob->id, shortestJob->arrival, shortestJob->length);
    time += shortestJob->length;
    if (time < nextArrivalTime)
    {
      time = nextArrivalTime;
    }
    shortestJob->isChecked = 1;
  }
  printf("End of execution with SJF.\n ");
}

void analyze_SJF(struct job *head)
{
  char *data[count];
  printf("Begin analyzing SJF:\n ");
  int startTime = 0;
  int arrivalTime = 0;
  int completionTime = 0;
  int responseTime = 0;
  int turnaroundTime = 0;
  int waitTime = 0;

  int responseSum = 0;
  int turnaroundSum = 0;
  int waitSum = 0;

  int time = 0;

  // add to data array
  int i = 0;
  for (struct job *curr = head; curr != NULL; curr = curr->next)
  {
    // calculate reponseTime
    arrivalTime = curr->arrival;
    startTime = time;
    responseTime = startTime - arrivalTime;

    // calculate turnaroundTime
    completionTime = time + curr->length;
    turnaroundTime = completionTime - arrivalTime;

    // calculate waitTime
    waitTime = turnaroundTime - curr->length;
    char temp[100];

    snprintf(temp, sizeof(temp), "Job %d -- Response time: %d Turnaround: %d Wait: %d\n", curr->id, responseTime, turnaroundTime, waitTime);
    data[i] = strdup(temp);
    time += curr->length;
    responseSum += responseTime;
    turnaroundSum += turnaroundTime;
    waitSum += waitTime;
    i++;
  }
  // for loop to print array in reverse
  for (int j = count - 1; j >= 0; j--)
  {
    printf("%s", data[j]);
    free(data[j]);
  }
  printf("Average -- Response time: %.2f Turnaround: %.2f Wait: %.2f\n", (double)responseSum / count, (double)turnaroundSum / count, (double)waitSum / count);
  printf("End analyzing SJF.\n ");
}

int main(int argc, char **argv)
{

  if (argc < 4)
  {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];

  // Note: we use a global variable to point to
  // the start of a linked-list of jobs, i.e., the job list
  read_workload_file(workload);
  if (strcmp(policy, "FIFO") == 0)
  {
    policy_FIFO(head);
    if (analysis)
    {
      analyze_FIFO(head);
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
  if (strcmp(policy, "SJF") == 0)
  {
    // for (struct job *curr = head; curr != NULL; curr = curr->next)
    // {
    //   printf("Job %d -- arrival: %d length: %d\n", curr->id, curr->arrival, curr->length);
    // }
    policy_SJF();
    if (analysis)
    {
      analyze_SJF(head);
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
