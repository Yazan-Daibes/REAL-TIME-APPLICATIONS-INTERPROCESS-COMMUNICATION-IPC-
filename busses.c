
#include "local.h"

void main(int argc, char *argv[])
{
    int semid, semid1, shmid;
    pid_t ppid = getppid();
    char *shmptr;
    struct MEMORY *memptr;

    int busSleepPeriodMax = atoi(argv[1]);
    int busSleepPeriodMin = atoi(argv[2]);
    int busSize = atoi(argv[3]);
    int HMIN = atoi(argv[5]);
    int full = 0;

    int passINBuss[busSize];
    int indexInbuss = 0;

    if ((shmid = shmget((int)ppid, 0, 0)) != -1)
    {
        if ((shmptr = (char *)shmat(shmid, (char *)0, 0)) == (char *)-1)
        {
            perror("shmat -- busses -- attach");
            exit(1);
        }
        memptr = (struct MEMORY *)shmptr;
    }
    else
    {
        perror("shmget -- busses-- access");
        exit(2);
    }
    /*
   * Access the semaphore set of shared memory
   */
    //
    if ((semid = semget((int)ppid, 2, 0)) == -1)
    {
        perror("semget -- busses -- access");
        exit(3);
    }

    while (1)
    {

        acquire.sem_num = TO_CONSUME;
        if (semop(semid, &acquire, 1) == -1)
        {
            perror("semop -- bus -- acquire");
            exit(4);
        }

        for (int i = 0; i < pass_struct_slots; i++)
        {
            if (memptr->passengers_IDs[i] == 0)
            {
                continue;
            }
            else
            {
                if (bus_size_counter < busSize)
                {
                    bus_size_counter++;
                    passINBuss[indexInbuss] = memptr->passengers_IDs[i];
                    indexInbuss++;
                    memptr->passengers_IDs[i] = 0; // mark them as left
                    memptr->counter--;
                    if (memptr->counter < HMIN)
                    {
                        flagOff = 0;
                    }
                }
                else
                {

                    full = 1;
                    bus_size_counter = 0;
                }
            }
        }

        release.sem_num = AVAIL_SLOTS;
        if (semop(semid, &release, 1) == -1)
        {
            perror("semop -- bus-- release");
            exit(-5);
        }

        if (full)
        {
            printf("               <<     BUS ID: %d     >>\n",getpid());
            for (int j = 0; j < busSize; j++)
            {
                printf("%s", CYAN_COLOR);
                printf("\u263A :");
                printf("Passenger %d is in the buss ID %d going to Jordan\n", passINBuss[j], getpid());
                kill(passINBuss[j], SIGTERM);
            }
            printf("%s", YELLOW_COLOR);
            printf("        Bus %d has left\n", getpid());
            sleep((int)(busSleepPeriodMin + rand() % busSleepPeriodMax)); // buss will be unavailable for TB second
            printf("        Bus %d has returned\n", getpid());
            printf("        All passengers in bus %d hvae arrived peacefully | Welcome To Jordan |    \n",getpid());
            printf("%s",WHITE_COLOR);
        }
    }
    while (1)
        ;
}
