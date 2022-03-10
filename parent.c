// 1180414 Yazan Daibes
// 1181557 Kareen Karam
// 1180424 Omar Qattosh

#include "local.h"

void main(int argc, char *argv[])
{
    int semid, shmid, croaker;
    char *shmptr;
    pid_t p_id, c_id, pid = getpid();
    FILE *fptr_input;
    FILE *fptr_queue_IDs;
    MESSAGE message;
    pid_t pid_passengers, pid_officers, pid_busses, pid_arr[3];
    static struct MEMORY memory;
    union semun arg, arg1;

    printf("Welcome to the borders between Palestine and Jordan\n");

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<read the input file>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    char inp_file_values[31][40];

    if ((fptr_input = fopen(INPUT_FILE, "r")) == NULL)
    {
        perror("INPUT_FILE");
        exit(-1);
    }

    // read the file
    for (int i = 0; i < 31; i++)
    {
        fscanf(fptr_input, "%s\n", inp_file_values[i]);
    }

    int numCrossPointsP = atoi(inp_file_values[7]);
    int numCrossPointsJ = atoi(inp_file_values[9]);
    int numCrossPointsF = atoi(inp_file_values[11]);
    int numOfOfficers = atoi(inp_file_values[13]);
    char pidOfficerssAsStr[numOfOfficers];

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< create message queue >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>...
    //  message queue is created
    int mid[numOfOfficers]; // queue for each officer
    char u_char = 'A';
    int key;
    for (int l = 0; l < numOfOfficers; l++, u_char++)
    {
        key = ftok(".", u_char);
        // conect to message queue if exists
        if ((mid[l] = msgget(key, 0)) == -1)
        {

            // create message queue
            if ((mid[l] = msgget(key, IPC_CREAT | 0666)) == -1)
            {
                perror("Queue create");
                exit(-5);
            }
        }
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< write message queue IDs on a file >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    if ((fptr_queue_IDs = fopen(QUEUE_IDs_FILE, "w")) == NULL)
    {
        perror("QUEUE_IDs_FILE");
        exit(-3);
    }
    for (int i = 0; i < numOfOfficers; i++)
    {
        fprintf(fptr_queue_IDs, "%d\n", mid[i]);
    }
    fclose(fptr_queue_IDs); //close the file

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< create shared memory & semaphores >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    if ((shmid = shmget((int)pid, sizeof(memory),
                        IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr = (char *)shmat(shmid, 0, 0)) == (char *)-1) // 0: means start from 0
        {
            perror("shmptr -- parent -- attach");
            exit(-3);
        }
        memcpy(shmptr, (char *)&memory, sizeof(memory));
    }
    else
    {
        perror("shmid -- parent -- creation");
        exit(-22);
    }

    /*
   * Create and initialize the semaphores for writing on memory
   */
    static ushort start_val[2] = {1, 0};
    if ((semid = semget((int)pid, 2,
                        IPC_CREAT | 0666)) != -1)
    {
        arg.array = start_val;

        if (semctl(semid, 0, SETALL, arg) == -1)
        {
            perror("semctl -- parent -- initialization");
            exit(-10);
        }
    }
    else
    {
        perror("semget -- parent -- creation");
        exit(-44);
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<forking the officers>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int officers_id[numOfOfficers];
    char numOfficers_str[10];
    int crossP = atoi(inp_file_values[7]);
    int crossJ = atoi(inp_file_values[9]);
    int crossF = atoi(inp_file_values[11]);

    int numPalCross = crossP;
    int numJorCross = crossJ;
    int numForCross = numOfOfficers - numPalCross - numJorCross;
    char index[10];
    char HMAX[10];
    strcpy(HMAX, inp_file_values[24]);
    char HMIN[10];
    strcpy(HMIN, inp_file_values[22]);
    int officersIDs[numOfOfficers];

    for (int k = 0; k < numOfOfficers; k++)
    {
        pid_officers = fork();
        if (pid_officers == 0 && k < numPalCross)
        {
            sprintf(numOfficers_str, "%d", numOfOfficers);
            sprintf(index, "%d", k);
            if (execl("./officers", "officers", numOfficers_str, "P", index, HMAX, HMIN, NULL) == -1)
            {
                perror("EXECL: officers");
                exit(-2);
            }
        }
        else if (pid_officers == 0 && (k >= numPalCross && k < (numOfOfficers - numForCross)))
        {
            sprintf(numOfficers_str, "%d", numOfOfficers);
            sprintf(index, "%d", k);
            if (execl("./officers", "officers", numOfficers_str, "J", index, HMAX, HMIN, NULL) == -1)
            {
                perror("EXECL: officers");
                exit(-3);
            }
        }
        else if (pid_officers == 0)
        {
            sprintf(numOfficers_str, "%d", numOfOfficers);
            sprintf(index, "%d", k);
            if (execl("./officers", "officers", numOfficers_str, "F", index, HMAX, HMIN, NULL) == -1)
            {
                perror("EXECL: officers");
                exit(-4);
            }
        }
        else
        {
            officersIDs[k] = pid_officers;
        }
    }
    sleep(3);

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< fork the busses >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    char BusSleepPeriodMax[10];
    strcpy(BusSleepPeriodMax, inp_file_values[20]);
    char BusSleepPeriodMin[10];
    strcpy(BusSleepPeriodMin, inp_file_values[19]);
    char BusSize[10];
    strcpy(BusSize, inp_file_values[17]);
    char indexB[10];
    int numOfBusses = atoi(inp_file_values[15]);
    char numBussesStr[20];
    strcpy(numBussesStr, inp_file_values[15]);
    int bussesIDs[numOfBusses];

    for (int b = 0; b < numOfBusses; b++)
    {
        pid_busses = fork();
        if (pid_busses == 0)
        {

            if (execl("./busses", "busses", BusSleepPeriodMax, BusSleepPeriodMin, BusSize, numBussesStr, HMIN, NULL) == -1)
            {
                perror("EXECL: bus");
                exit(-2);
            }
        }
        else
        {
            bussesIDs[b] = pid_busses;
        }
    }
    sleep(3);

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<fork the passengers based on their percentages>>>>>>>>>>>>>>>>>>>>>>
    // int maxNumberOfPassengers = atoi(inp_file_values[33]);
    while (1)
    {
        srand(time(0));
        int num_of_pass = (int)(20 + rand() % 100);
        int numPal = num_of_pass * atof(inp_file_values[1]);
        int numJor = num_of_pass * atof(inp_file_values[3]);
        int numFor = num_of_pass - numPal - numJor;
        char midAsStr[10];
        char index[10];
        int passengers_IDs[num_of_pass];
        printf("%s", RED_COLOR);
        printf("Number of passengers arived at the border = %d\n", num_of_pass);
        printf("            The passengers Queued at the crossing points:          \n");

        for (int i = 0; i < num_of_pass; i++)
        {
            // fork the passengers
            pid_passengers = fork();

            // Palestinian passenger
            if (pid_passengers == 0 && i < numPal)
            {
                sprintf(midAsStr, "%d", mid[i % numCrossPointsP]);
                sprintf(index, "%d", i);
                if (execl("./passengers", "passengers", "P", midAsStr, index, NULL) == -1)
                {
                    perror("EXECL: Passengers");
                    exit(-2);
                }
            }
            //Jordanians
            else if (pid_passengers == 0 && (i >= numPal && i < (num_of_pass - numFor)))
            {
                sprintf(midAsStr, "%d", mid[(i % numCrossPointsJ) + numCrossPointsP]);
                sprintf(index, "%d", i);
                if (execl("./passengers", "passengers", "J", midAsStr, index, NULL) == -1)
                {
                    perror("EXECL: Passengers");
                    exit(-3);
                }
            }
            // Foriegners
            else if (pid_passengers == 0)
            {
                sprintf(midAsStr, "%d", mid[(i % numCrossPointsF) + numCrossPointsP + numCrossPointsJ]);
                sprintf(index, "%d", i);
                if (execl("./passengers", "passengers", "F", midAsStr, index, NULL) == -1)
                {
                    perror("EXECL: Passengers");
                    exit(-4);
                }
            }
            // parent
            else
            {
                passengers_IDs[i] = pid_passengers;
            }
        }

        sleep(90);
        int pgFile = atoi(inp_file_values[26]);
        int pdFile = atoi(inp_file_values[28]);
        int puFile = atoi(inp_file_values[30]);

        // the program terminate when:
        // if (pg >= pgFile || pd >= pdFile || pu >= puFile)
        {
            // terminate the passengers
            for (int p = 0; p < num_of_pass; p++)
            {
                kill(passengers_IDs[p], SIGTERM);
            }
            // terminate the officers
            for (int f = 0; f < numOfOfficers; f++)
            {
                kill(officersIDs[f], SIGTERM);
            }
            // terminate the busses
            for (int b = 0; b < numOfBusses; b++)
            {
                kill(bussesIDs[b], SIGTERM);
            }

            // delete the shared memory
            if (shmdt(shmptr) == -1)
            {
                perror("shmdt:"); // remove
                exit(-5);
            }

            // remove shared memory
            if (shmctl(shmid, IPC_RMID, (struct shmid_ds *)0))
            {
                perror("shmctl: IPC_RMID"); /* remove shared memory */
                exit(-5);
            }

            // remove sempahore
            if (semctl(semid, 0, IPC_RMID, 0) == -1)
            {
                perror("semctl: IPC_RMID"); /* remove semaphore */
                exit(-6);
            }
            // remove message queues
            for (int m = 0; m < numOfOfficers; m++)
            {
                if (msgctl(mid[m], IPC_RMID, (struct msqid_ds *)0) == -1)
                {
                    perror("msgctl: IPC_RMID"); // remove message queue
                    exit(-6);
                }
            }
            printf("The program has been terminated\n");
            
            printf("\033[0;36m"); // cyan color
            printf("***********************************************************************************\n");
            printf("***********************************************************************************\n");
            printf("Done By: 1180414 Yazan Daibes \n         1180424 Omar Qattosh\n         1181557 Kareen Karam\n");
            printf("***********************************************************************************\n");
            printf("***********************************************************************************\n");
            break;
        }
    }
}
