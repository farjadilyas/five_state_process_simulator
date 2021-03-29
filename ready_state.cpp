#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/types.h>
#include "util.h"

using namespace std;

pthread_t b_tid, r_tid, n_tid;
pthread_mutex_t lock;


// Package struct for Inter-Thread COmmunication
// stores file descriptors for 3 sources, algorithm, and both data structures to be used when appropriate

struct Package
{
    int fd[3];
    int algo;
    bool proc_received;
    Queue<Proc> qu;
    Heap<Proc> hp;
};

void * read_new(void * arg)
{
    int stat = -1;
    bool lp_cont = true, in_cont = true;
    Package * pck = (Package *)arg;

    Proc temp;


    while (lp_cont)
    {
        // READING FROM NEW STATE

        stat = read((*pck).fd[0], &temp, sizeof(Proc));

        switch (stat) { 
        case -1: 
            break;

        case 0: 
            pthread_exit(NULL);
            break;
        default: 
            printf("READY STATE - Received process from new, enqueueing...\n");

            //MUTEX LOCK --- data insertion

            pthread_mutex_lock(&lock);
            switch((*pck).algo)
            {
                case(FCFS):
                case(RR):
                    temp.order_by = ARRIVAL;
                    (*pck).qu.enqueue(temp);
                    break;
                case(SJF):
                    temp.order_by = BURST;
                    (*pck).hp.insert_data(temp);
                    break;
                case(SRTF):
                    temp.order_by = REM_JOB;
                    (*pck).hp.insert_data(temp);
                    break;
                default:
                    break;
            }
            pthread_mutex_unlock(&lock);

            //MUTEX UNLOCK
        }
    }
}

void * read_running(void * arg)
{
    int stat = -1, r_stat = -1;
    bool lp_cont = true, in_cont = true;
    Package * pck = (Package *)arg;

    Proc temp;

    while (lp_cont)
    {
        // READING FROM RUN STATE

        r_stat = read((*pck).fd[1], &temp, sizeof(Proc));

        switch (r_stat) { 
        case -1: 
            break;

        case 0: 
            pthread_exit(NULL);
            break;
        default: 
            printf("READY STATE - Received process from running, enqueueing...\n");

            //MUTEX LOCK --- data insertion

            pthread_mutex_lock(&lock);
            switch((*pck).algo)
            {
                case(FCFS):
                case(RR):
                    temp.order_by = ARRIVAL;
                    (*pck).qu.enqueue(temp);
                    break;
                case(SJF):
                    temp.order_by = BURST;
                    (*pck).hp.insert_data(temp);
                    break;
                case(SRTF):
                    temp.order_by = REM_JOB;
                    (*pck).hp.insert_data(temp);
                    break;
                default:
                    break;
            }
            pthread_mutex_unlock(&lock);

            //MUTEX UNLOCK
        }
    }
}

void * read_blocked(void * arg)
{
    int stat = -1, b_stat = -1;
    bool lp_cont = true, in_cont = true;
    Package * pck = (Package *)arg;

    Proc temp;

    while (lp_cont)
    {

        //READING FROM BLOCKED STATE

        b_stat = read((*pck).fd[2], &temp, sizeof(Proc));

        switch (b_stat) { 
        case -1: 
            break;

        case 0: 
            pthread_exit(NULL);
            break;
        default: 
            printf("READY STATE - Received process from blocked, enqueueing...\n");

            //MUTEX LOCK --- data insertion

            pthread_mutex_lock(&lock);
            switch((*pck).algo)
            {
                case(FCFS):
                case(RR):
                    temp.order_by = ARRIVAL;
                    (*pck).qu.enqueue(temp);
                    break;
                case(SJF):
                    temp.order_by = BURST;
                    (*pck).hp.insert_data(temp);
                    break;
                case(SRTF):
                    temp.order_by = REM_JOB;
                    (*pck).hp.insert_data(temp);
                    break;
                default:
                    break;
            }
            pthread_mutex_unlock(&lock);

            //MUTEX UNLOCK
        }
    }
}

int main()
{
    printf("You have entered the ready state!\n");


    
    pid_t run_pid = fork();


    //Parent process - READY_STATE

    if (run_pid > 0)
    {
        struct Package pck;         // Package object which stores ready queue/heap -- available to all threads of this process


        //----------------------------- [[ CREATE / OPEN FIFO PIPES ]] ---------------------------------

        int ready_running = 1, running_ready = 1, running_ready_p = 1, blocked_ready = 1;

        if (access("READY_RUNNING", F_OK) == -1)
            ready_running = mkfifo("READY_RUNNING", 0666);

        if (access("RUNNING_READY", F_OK) == -1)
            running_ready = mkfifo("RUNNING_READY", 0666);

        if (access("BLOCKED_READY", F_OK) == -1)
            blocked_ready = mkfifo("BLOCKED_READY", 0666);
        
        if (access("RUNNING_READY_P", F_OK) == -1)
            running_ready_p = mkfifo("RUNNING_READY_P", 0666);

        if (ready_running < 0 || blocked_ready < 0)
        {
            printf("--- [[ Pipe error in ready_state ]] ---\n");
            exit(1);
        }
        
        running_ready = open("RUNNING_READY", O_RDONLY);
        running_ready_p = open("RUNNING_READY_P", O_RDONLY | O_NONBLOCK);
        ready_running = open("READY_RUNNING", O_WRONLY | O_TRUNC); // ?
        blocked_ready = open("BLOCKED_READY", O_RDONLY | O_NONBLOCK);
        int new_ready = open("NEW_READY", O_RDONLY | O_NONBLOCK);

        //------------------------------- [[ END OF PIPE CREATION / OPENING ]] ------------------------





        //-------------------------------- [[ READ ALGORITHM / QUANTA / NUM PROCESSES AND PASS ONTO RUN STATE ]] ------------------


        int algo_stat = -1, algo = ERR, quanta = INT_MAX;

        while (algo_stat != 4)
        {
            algo_stat = read(new_ready, &algo, sizeof(int));
        }

        printf("\nThe aglo received is: %d\n", algo);

        int one = 1, zero = 0;

        if (algo == ERR)
        {
            printf("\nRUN STATE ERROR - algo is invalid\n");
            exit(0);
        }
        else if (algo == RR)
        {
            int quanta_stat = -1;

            while (quanta_stat != 4)
            {
                quanta_stat = read(new_ready, &quanta, sizeof(int));
            }

            printf("\n\n\nHello there! The quanta is: %d\n", quanta);

            write(ready_running, &algo, sizeof(int));

            write(ready_running, &quanta, sizeof(int));
        }
        else
        {
            write(ready_running, &algo, sizeof(int));
        }



        int num_proc_stat = -1, num_proc = 0;

        while (num_proc_stat != 4)
            num_proc_stat = read(new_ready, &num_proc, sizeof(int));

        write(ready_running, &num_proc, sizeof(int));

        //----------------------------------------------- [[ END OF ALGORITHM & NUM PROC I/O ]] -----------------------------------



        
        
        //-------------------[[ INITIALIZING MUTEX, PACKAGE OBJECT ]]------------

        Proc temp;
        int stat, b_stat, r_stat, timeout = 0, run_stat = 0, run_sign;
        bool in_cont = true, lp_cont = true;

        if (pthread_mutex_init(&lock, NULL) != 0) 
        { 
            printf("\n mutex init has failed\n"); 
            return 1; 
        } 

        pck.algo = algo;
        pck.proc_received = false;
        pck.fd[0] = new_ready;
        pck.fd[1] = running_ready_p;
        pck.fd[2] = blocked_ready;

        //-------------------------[[ END OF SECTION ]] ------------------------





        //------------------ [[ THREAD CREATION ]] -------------------------

        int n_in = -1, r_in = -1, b_in = -1;

        n_in = pthread_create(&n_tid, NULL, read_new, &pck);

        if (n_in == 0)
            r_in = pthread_create(&r_tid, NULL, read_running, &pck);
        if (r_in == 0)
            b_in = pthread_create(&b_tid, NULL, read_blocked, &pck);
        if (b_in != 0)
        {
            printf("\nREADY STATE - THREAD CREATION FAILED\n");
            exit(1);
        }

        // -------------------- [[ END OF SECTION ]] -----------------------





        //--------------------------------------------- [[ READY STATE I/O LOOP ]] ------------------------------------

        while(lp_cont)
        {

            if (pck.proc_received == true)
                continue;

            

            // READ SIGNAL FROM RUN_STATE

            if (run_stat == 0)
                run_stat = read(running_ready, &run_sign,sizeof(int));

            switch(run_stat)
            {
                case(-1):
                    break;

                

                //PIPE CLOSED - END OF COMMUNICATION

                case(0):
                    printf("End of comm from run_state.. exiting\n");
                    lp_cont = false;                                        // CHECK THIS IF YOU HAVE ANY ERRORS..  MAYBE IT SHOULDNT BE HERE
                    break;



                // READ SUCCESSFUL - IF PROC AVAILABLE - SEND, ELSE REMEMBER SIGNAL RECEIVED
                
                default:
                    if ( ((algo == FCFS || algo == RR) &&(!pck.qu.isEmpty())) || ((algo == SJF || algo == SRTF) && !pck.hp.isEmpty()))
                    {
                        pthread_mutex_lock(&lock);
                        switch(algo)
                        {
                            case(FCFS):
                            case(RR):
                                temp = pck.qu.dequeue().data;
                                break;
                            case(SJF):
                            case(SRTF):
                                temp = pck.hp.delete_min();
                                break;
                        }
                        pthread_mutex_unlock(&lock);

                        write(ready_running, &temp, sizeof(Proc));
                        run_stat = 0;
                    }
                    else
                    {
                        run_stat = 1;
                    }
                    break;

            }

            pck.proc_received = false;
        }


        //WAITS FOR THREADS TO EXIT (WHEN ALL CONCERNED PIPES ARE CLOSED FROM THE OTHER END)

        pthread_join(b_tid, NULL);
        pthread_join(r_tid, NULL);
        pthread_join(n_tid, NULL);


        //WAIT FOR RUN_STATE's RETURN STATUS

        int status = 0;
        waitpid(run_pid, &status, 0);
        printf("\nREADY STATE - STAUS IS: %d",int(WEXITSTATUS(status)));


        // CLOSE ALL RELEVANT FILE DESCRIPTORS
        
        close(blocked_ready);
        close(new_ready);
        close(ready_running);
        close(running_ready_p);
        close(running_ready);
    }

    //Child process - EXEC INTO RUN_STATE
    else if (run_pid == 0)
    {
        execl("./run_state", "./run_state", NULL);
    }
    else
        printf("--- [[ Fork error in ready_state.cpp ]] ---\n");

    printf("Closing ready_state\n");

    return 0;
}