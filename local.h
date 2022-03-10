#ifndef __LOCAL_H__
#define __LOCAL_H__


/*
 * Common header file for Message Queue Example
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <wait.h>

#define ORIGINAL "\x1B[0m"
#define RED_COLOR "\x1B[31m"
#define GREEN_COLOR "\x1B[32m"
#define YELLOW_COLOR "\x1B[33m"
#define BLUE_COLOR "\x1B[34m"
#define PURPLE_COLOR "\x1B[35m"
#define CYAN_COLOR "\x1B[36m"
#define WHITE_COLOR "\x1B[37m"

#define INPUT_FILE "./input.txt"
#define QUEUE_IDs_FILE "./queue_IDs.txt"
#define pass_struct_slots 900


int bus_size_counter = 0;
int pg = 0;
int pd = 0;
int pu = 0;
int flagOff = 0;

union semun
{
  int val;
  struct semid_ds *buf;
  ushort *array;
};

struct sembuf acquire = {0, -1, SEM_UNDO},
              release = {0, 1, SEM_UNDO};


enum
{
  AVAIL_SLOTS,
  TO_CONSUME
};

enum
{
  AVAIL_SLOTS1,
  TO_CONSUME1
};

struct passanger
{
  int pid;
  char nationality[2];
  int havepassport;
  int P_lvl;
  int index;
};

struct MEMORY
{
  int num_of_pass_in_hall;
  int stop;
  int passengers_IDs[pass_struct_slots]; //= {[0 ... (pass_struct_slots-1)] = 0};
  int counter;
};

typedef struct
{

  char buffer[BUFSIZ];
} MESSAGE;

#endif
