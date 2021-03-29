#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>
#include<cstring>
#include "util.h"

using namespace std;

int main()
{
    printf("\n--- --- --- [[ NEW STATE ]] -- --- ---\n\n");



    //---------------- [[ FILE READING ]] ------------------

    int algo = ERR, quanta = INT_MAX;

    int fd = open("processes.txt", O_RDONLY);


    //Backing up standard input file descriptor

    int stdin = dup(0);


    //Copying process.txt file descriptor to standard in

    dup2(fd,0);

    char s_algo[5];
    cin >> s_algo;

    if (strcmp(s_algo, "FCFS") == 0)
        algo = FCFS;
    else if (strcmp(s_algo, "RR") == 0)
    {
        algo = RR;
        cin >> quanta;
    }

    else if (strcmp(s_algo, "SJF") == 0)
        algo = SJF;
    else if (strcmp(s_algo, "SRTF") == 0)
        algo = SRTF;

    printf("\nAlgo is: %d %s\n", algo, s_algo);

    char temp_p_name[8];
    PriorityQueue<Proc> proc_l;
    Proc temp_p;

    while (cin >> temp_p_name)
    {
        strcpy(temp_p.name,temp_p_name);
        cin >> temp_p.arrival_t;
        cin >> temp_p.burst_t;
        //cin >> temp_p.priority;
        temp_p.priority = 0;
        temp_p.job_t = temp_p.burst_t;
        temp_p.latest_arrival_t = temp_p.arrival_t;
        temp_p.order_by = ARRIVAL;
        proc_l.enqueue(temp_p);
    }

    //----------- [[ END OF FILE READING ]] ----------




    
    // ------------ CREATING PIPES
    
    int new_ready = 1, num_proc = proc_l.getLength() + 1;

    printf("\n\nTHE NUMBER OF PROCS IS: %d\n\n", num_proc);

    if (access("NEW_READY", F_OK) == -1)
        new_ready = mkfifo("NEW_READY", 0666);

    if (new_ready < 0)
    {
        printf("--- [[ Pipe failed in new_state.cpp ]] ---\n");
        exit(1);
    }
    
    //---------- [[ END OF PIPE CREATION                        //printf("\nQueue is empty.. %i\n", run_stat); ]] ----------




    time_t start = time(NULL);                              // Storing starting time - used to judge arrival time


    pid_t ready_pid = fork();

    //Parent process - NEW STATE
    if (ready_pid > 0)
    {
        new_ready = open("NEW_READY", O_WRONLY | O_TRUNC);


        //Send choice of scheduling algorithm to ready_state, quanta for Round Robin

        write(new_ready, &algo, sizeof(int));

        if (algo == RR)
        {
            write(new_ready, &quanta, sizeof(int));
        }

        write(new_ready, &num_proc, sizeof(int));



        //Send processes if arrival_t == current time, else sleep
        //Priority queue so we only need to check the first element

        while (!proc_l.isEmpty())
        {
            if (time(NULL) - start == proc_l.getRear().arrival_t)
            {
                write(new_ready, &proc_l.getRear(), sizeof(Proc));
                proc_l.dequeue();
            }
            else
                sleep(1);
        }
        close(new_ready);


        //Wait for ready_state status...

        wait(NULL);
    }

    //Child process - EXEC TO READY STATE
    else if (ready_pid == 0)
    {
        execl("./ready_state", "./ready_state", NULL);
    }
    else
        printf("--- [[ Fork error in new_state.cpp ]] ---\n");


    printf("Closing new_state\n");

    return 0;
}