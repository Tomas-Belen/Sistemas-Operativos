#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main (int argc, char**argv){
    
    //Arguments invalids
    if (argc != 2){
        fprintf(2, "ERROR: the pingpong accept a single argument\n");
        exit(1);
    }

    int round = atoi(argv[1]);

    //Number of rounds invalids
    if (round < 1){
        fprintf(2, "ERROR: number of rounds must be greater/equal to 1.\n");
        exit(1);
    }

    int get_child = get_sem(0);
    int get_parent = get_sem(1);

    if (get_child == -1 || get_parent == -1){
        fprintf(2, "ERROR: No semaphores avaible\n");
        return 1;
    }

    //Management of process
    int rc = fork();

    if (rc < 0){
        fprintf(2, "ERROR: fork failed\n");
        return 1;
    }else if(rc == 0){
        for (unsigned int i = 0; i < round; ++i){
            sem_down(get_child);
            printf("\tpong\n");
            sem_up(get_parent);
        }

        return 0;
    }else{
        for (unsigned int i = 0; i < round; ++i){
            sem_down(get_parent);
            printf("ping\n");
            sem_up(get_child);
        }
        
    }

    wait(0);
    sem_close(get_child);
    sem_close(get_parent);

    return 0;    
}