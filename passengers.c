
// 1180414 Yazan Daibes
// 1181557 Kareen Karam 
// 1180424 Omar Qattosh

#include "local.h"

void main(int argc, char *argv[])
{

    int mid = atoi(argv[2]);
    char passengersAttributes[100];
    MESSAGE message;
    struct passanger psngr_Struct;

    srand(time(0));
    usleep(1000);

    int passChance = (int)(1 + rand() % 99);
    int havePassportF, n;

    if (passChance > 10)
        havePassportF = 1; // have the passport
    else
        havePassportF = 0;

    srand(time(0));
    int p_level = (int)(1 + rand() % 4);

    psngr_Struct.pid = getpid();
    psngr_Struct.havepassport = havePassportF;
    strcpy(psngr_Struct.nationality, argv[1]);
    psngr_Struct.P_lvl = p_level;
    psngr_Struct.index = atoi(argv[3]);

    printf("%s", GREEN_COLOR);
    printf("\u263A :");
    printf("Passenger: id = %d, patience level = %d, nationality = %s , have passport = %d is at the crossing point\n", psngr_Struct.pid, psngr_Struct.P_lvl, psngr_Struct.nationality, psngr_Struct.havepassport);

    if (msgsnd(mid, (const char *)&psngr_Struct, sizeof(psngr_Struct), 0) == -1)
    {
        perror("Error: msgsnd Palestinian");
        exit(-1);
    }

    while (1)
        ;
}