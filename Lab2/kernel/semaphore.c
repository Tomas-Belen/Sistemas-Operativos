#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"


#define MAX_SEM 512

//Define the struct for the semaphore
struct semaphore{
  unsigned int value;
  struct spinlock lock;
};

struct semaphore sem_array[MAX_SEM];


//Implementation of the function sem_array_init()
void sem_array_init(void){
    for (unsigned int i = 0; i < MAX_SEM; ++i)
    {
        sem_array[i].value = -1;
        initlock(&(sem_array[i].lock), "semaphore");
    }
}

//This is auxiliar function to get an available semaphore
int get_sem(int value){
    int sem = 0;
    while (sem < MAX_SEM && !sem_open(sem, value))
    {
        ++sem;
    }

    if (sem == MAX_SEM)
    {
        sem = -1;
    }
  
    return sem;
}



//Implementation of the function sem_open()
int sem_open (int sem, int value){

    //management of error
    if (sem < 0 || sem >= MAX_SEM){
        printf ("ERROR: Invalid semaphore. /n");
        return 0;
    }
    
    int ret_sem = 0;

    //getting the semaphore's lock and some busy waiting
    acquire(&(sem_array[sem].lock));
    
    
    //Changes te semaphore value an returns 1 
    if (sem_array[sem].value == -1){
        sem_array[sem].value = value;
        ret_sem = 1;
    }

    //Release the lock of the struct
    release (&(sem_array[sem].lock));

    return ret_sem;
}

//Implementation of the function sem_close()
int sem_close(int sem){

    //management of error
    if(sem < 0 || sem >= MAX_SEM){
        printf("ERROR: invalid semaphore\n");
        return 0;
    }

    if(sem_array[sem].value < 0){
        printf("ERROR: Semaphore already closed\n");
        return 0;
    }

    //getting the semaphore's lock and some busy waiting
    acquire(&(sem_array[sem].lock));

    //closing the semaphore
    sem_array[sem].value = -1;

    //Release the lock of the struct
    release(&(sem_array[sem].lock));

    return 1;
}


//Implementation of the function sem_up()
int sem_up (int sem){
  
    //Try to access of semaphorus before a critic situation
    acquire(&(sem_array[sem].lock));
  
    //If a negative value, release the semaphorus
    if(sem_array[sem].value < 0){
        printf("ERROR: tried to increase closed semaphorus\n");
        release(&(sem_array[sem].lock));
        return 0;
    }
  
    //The blocked process are been awakened!!!!
    if(sem_array[sem].value == 0){
        wakeup(&(sem_array[sem]));  
    }
  
    //Increment the count in semaphorus
    ++(sem_array[sem].value);
  
    //Release the lock of the struct 
    release(&(sem_array[sem].lock));
  
    return 1;
}

//Implementation of the function sem_down()
int sem_down(int sem)
{
    //getting the semaphore's lock and some busy waiting
    acquire(&(sem_array[sem].lock));

    //management of error
    if(sem_array[sem].value < 0){
        printf("Tried to decrease a closed semaphore\n");
        release(&(sem_array[sem].lock));
        return 0;
    }

    //while the semaphore's value is on zero, the process is put to sleep
    while(sem_array[sem].value == 0){
        sleep(&(sem_array[sem]), &(sem_array[sem].lock));
    }

    //decrease semaphore value
    --sem_array[sem].value;

    //"breaking the locks"
    release(&(sem_array[sem].lock));

    return 1;
}


