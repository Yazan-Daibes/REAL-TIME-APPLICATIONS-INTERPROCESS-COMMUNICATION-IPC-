
#include "local.h"

void main(int argc, char *argv[])
{
    FILE *fptr_queue_IDs;
    int num_officers = atoi(argv[1]);
    int HMAX = atoi(argv[4]);
    int HMIN = atoi(argv[5]);
    int queue_IDs[num_officers];
    MESSAGE message;
    struct passanger passngr_struct;
    int n;
    char *shmptr;
    static struct MEMORY *memptr;
    int semid, shmid;
    pid_t ppid = getppid();

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<read the queue IDs>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if ((fptr_queue_IDs = fopen(QUEUE_IDs_FILE, "r")) == NULL)
    {
        perror("QUEUE_IDs_FILE");
        exit(-7);
    }

    for (int f = 0; f < num_officers; f++)
    {
        fscanf(fptr_queue_IDs, "%d\n", &queue_IDs[f]);
    }

    fclose(fptr_queue_IDs);
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid = shmget((int)ppid, 0, 0)) != -1)
    {
        if ((shmptr = (char *)shmat(shmid, (char *)0, 0)) == (char *)-1)
        {
            perror("shmat -- passenger -- attach");
            exit(1);
        }
        memptr = (struct MEMORY *)shmptr;
    }
    else
    {
        perror("shmget -- passenger-- access");
        exit(2);
    }
    /*
   * Access the semaphore set
   */
    //
    if ((semid = semget((int)ppid, 2, 0)) == -1)
    {
        perror("semget -- producer -- access");
        exit(3);
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< officers are at the crossing points doing their job>>>>>>>>>>>>>>>>>>>>>>>
    while (1)
    {
        if (flagOff == 0)
        {
            if (strcmp(argv[2], "P") == 0)
            {

                if ((n = msgrcv(queue_IDs[atoi(argv[3])], (char *)&passngr_struct, sizeof(passngr_struct), 0, 0)) != -1)
                {
                    if (passngr_struct.havepassport == 1)
                    {
                        // passengers are not proccessed at the same rate
                        srand(time(0));
                        sleep((int)(1 + rand() % 4));
                        acquire.sem_num = AVAIL_SLOTS;
                        if (semop(semid, &acquire, 1) == -1)
                        {
                            perror("semop -- officers -- acquire");
                            exit(-3);
                        }
                        if (memptr->counter < HMAX)
                        {
                            memptr->passengers_IDs[passngr_struct.index] = passngr_struct.pid;
                            memptr->counter++;
                            printf("%s", BLUE_COLOR);
                            printf("\u263A :");
                            printf("Passenger %d has crossed the border and went to the hall\n", memptr->passengers_IDs[passngr_struct.index]);
                        }
                        else
                        {
                            flagOff = 1;
                        }

                        ++pg; // granted

                        release.sem_num = TO_CONSUME;
                        if (semop(semid, &release, 1) == -1)
                        {
                            perror("semop -- consumer -- indicating number has been read");
                            exit(-5);
                        }
                    }
                    else
                    {
                        ++pd; // denied
                        printf("%s", RED_COLOR);
                        printf("\u263A :");
                        printf("The Palestinian passenger with ID = %d is denied crossing\n", passngr_struct.pid);
                        kill(passngr_struct.pid, SIGTERM);
                    }
                }
            }
            else if (strcmp(argv[2], "J") == 0)
            {

                if ((n = msgrcv(queue_IDs[atoi(argv[3])], (char *)&passngr_struct, sizeof(passngr_struct), 0, 0)) != -1)
                {
                    if (passngr_struct.havepassport == 1)
                    {
                        srand(time(0));
                        sleep((int)(1 + rand() % 4));
                        acquire.sem_num = AVAIL_SLOTS;
                        if (semop(semid, &acquire, 1) == -1)
                        {
                            perror("semop -- officers -- acquire");
                            exit(-3);
                        }
                        if (memptr->counter < HMAX)
                        {

                            memptr->passengers_IDs[passngr_struct.index] = passngr_struct.pid;
                            printf("%s", BLUE_COLOR);
                            printf("\u263A :");
                            printf("Passenger %d has crossed the border and went to the hall\n", memptr->passengers_IDs[passngr_struct.index]);
                            memptr->counter++;
                        }
                        else
                        {

                            flagOff = 1;
                        }

                        ++pg; // granted
                        release.sem_num = TO_CONSUME;
                        if (semop(semid, &release, 1) == -1)
                        {
                            perror("semop -- officers -- indicating number has been read");
                            exit(-6);
                        }
                    }
                    else
                    {
                        ++pd; // denied
                        printf("%s", BLUE_COLOR);
                        printf("\u263A :");
                        printf("The Jordanian passenger with ID = %d is denied crossing\n", passngr_struct.pid);
                        kill(passngr_struct.pid, SIGTERM);
                    }
                }
            }
            else if (strcmp(argv[2], "F") == 0)
            {

                if ((n = msgrcv(queue_IDs[atoi(argv[3])], (char *)&passngr_struct, sizeof(passngr_struct), 0, 0)) != -1)
                {
                    if (passngr_struct.havepassport == 1)
                    {
                        srand(time(0));
                        sleep((int)(1 + rand() % 4));
                        acquire.sem_num = AVAIL_SLOTS;
                        if (semop(semid, &acquire, 1) == -1)
                        {
                            perror("semop -- officers -- acquire");
                            exit(-3);
                        }

                        if (memptr->counter < HMAX)
                        {
                            memptr->passengers_IDs[passngr_struct.index] = passngr_struct.pid;
                            printf("%s", BLUE_COLOR);
                            printf("\u263A :");
                            printf("Passenger %d has crossed the border and went to the hall\n", memptr->passengers_IDs[passngr_struct.index]);
                            memptr->counter++;
                        }
                        else
                        {
                            flagOff = 1;
                        }

                        ++pg; // granted
                        release.sem_num = TO_CONSUME;
                        if (semop(semid, &release, 1) == -1)
                        {
                            perror("semop -- officers -- indicating number has been read");
                            exit(9);
                        }
                    }
                    else
                    {
                        ++pd; // denied
                        printf("%s", BLUE_COLOR);
                        printf("\u263A :");
                        printf("The Foriegner passenger with ID = %d is denied crossing\n", passngr_struct.pid);
                        kill(passngr_struct.pid, SIGTERM);
                    }
                }
            }
        }
        else
        {
            printf("\u263A :");
            printf("%s", RED_COLOR);
            printf("Hall is full\n");
        }
    }
    while (1)
        ;
}
