#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<time.h>
#include "util.h"
//#include<pthread>

using namespace std;

int main()
{

    printf("Welcome to the run_state\n");

    int running_blocked = 1, running_exit = 1;

    pid_t blocked_pid, exit_pid;

    int timer = 0;
    time_t start_t = time(NULL), end_t;

    stringstream out, out2;

    blocked_pid = fork();

    //Parent
    if (blocked_pid > 0)
    {
        exit_pid = fork();

        //Parent
        if (exit_pid > 0)
        {

            if (access("RUNNING_BLOCKED", F_OK) == -1)
            running_blocked = mkfifo("RUNNING_BLOCKED", 0666);
            if (access("RUNNING_EXIT", F_OK) == -1)
                running_exit = mkfifo("RUNNING_EXIT", 0666);

            running_blocked = open("RUNNING_BLOCKED", O_WRONLY | O_TRUNC);
            running_exit = open("RUNNING_EXIT", O_WRONLY | O_TRUNC);
            int ready_running = open("READY_RUNNING",  O_RDONLY | O_NONBLOCK); // ?
            int running_ready = open("RUNNING_READY", O_WRONLY | O_TRUNC);
            int running_ready_p = open("RUNNING_READY_P", O_WRONLY | O_TRUNC);

            if (running_blocked < 0 || running_exit < 0)
            {
                printf("--- [[ Pipe error in run_state ]] ---\n");
                exit(1);
            }
            else
                printf("\nRUN - pipes created successfully\n");
            


            int ready_stat = -1, algo = 0, quanta = INT_MAX;

            while(ready_stat != 4)
            {
                ready_stat = read(ready_running, &algo, sizeof(int));
            }

            if (algo == RR)
            {
                ready_stat = -1;
                while (ready_stat != 4)
                {
                    ready_stat = read(ready_running, &quanta, sizeof(int));
                }
            }

            printf("\nRUN STATE - Signal: %d, Quanta: %d\n", algo, quanta);


            // HANDLING NUMBER OF PROCESSES---------------------------------

            int num_proc_stat = -1, num_proc = 0, num_complete = 0;

            while (num_proc_stat != 4)
                num_proc_stat = read(ready_running, &num_proc, sizeof(int));

            write(running_exit, &num_proc, sizeof(int));

            write(running_exit, &algo, sizeof(int));
            //---------------------------------------------------------


            int fd = open("gantt_chart.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            char buffer[4096];

            out<<setfill('=')<<setw(69)<<"=\n"<<"||\t\t\tGANTT CHART\t\t\t\t  ||\n"<<setw(70)<<"=\n\n"<<setfill(' ');
            out<<"Log Start Time: "<<ctime(&start_t)<<"\nScheduling Algorithm: ";
            switch(algo)
            {
                case(FCFS): out<<"First Come First Serve (FCFS)\n";         break;
                case(RR):   out<<"Round Robin (RR)\n";                      break;
                case(SJF):  out<<"Shortest Job First (SJF)\n";              break;
                case(SRTF): out<<"Shortest Remaining Time First (SRTF)\n";  break;
                default:    printf("ERROR IN EXIT'S ALGO\nExiting...\n");   exit(0); 
            }

            out<<setfill(' ')<<left<<setw(18)<<"\n\nPROCESS NAME"<<setw(11)<<"START_T"<<setw(10)<<"END_T"<<"\n";



            Proc temp;
            int s_stat = 1, rd_in = -1, i, j;
            bool block_proc = false, timeout = false, lp_cont = true;

            while(num_complete < num_proc)
            {
                printf("RUN STATE - sending time out signal\n");
                write(running_ready, &s_stat, sizeof(int));
                printf("\nSignal sent!!\n");

                while (rd_in != sizeof(Proc))
                {
                    rd_in = read(ready_running, &temp, sizeof(Proc));
                }

                printf("RD_IN IS: %i\n",rd_in);
                rd_in = -1;

                printf("RUN STATE - received proc, executing... %d\n\n", int(time(NULL) - start_t));
                cout<<temp<<"\n\n";

                out<<left<<setw(16)<<temp.name<<setw(11)<<int(time(NULL)-start_t);

                while (!temp.complete() &&!timeout && !block_proc)
                {
                    for (j = 0 ; j < temp.burst_t  && !temp.complete(j) &&!timeout && !block_proc; ++j)
                    {
                        sleep(1);
                        timer = int(time(NULL) - start_t);
                        printf("\033[A\33[2KT\rExecuting... %d/%d %d  %d\n", j + 1, temp.job_t, timer, int(time(NULL) - start_t));

                        if (j+1 == quanta)
                        {
                            timeout = true;
                        }
                        
                        else if (timer%5 == 0 && rand()%2 == 0)
                        {
                            block_proc = true;
                        }
                    }
                    temp.addBurst(timer - j, timer);
                }

                out<<setw(11)<<left<<int(time(NULL) - start_t)<<"\n";

                temp.setLatestArrivalTime(timer);

                printf("\nRemaining job: %d \n", int(temp.job_t));

                if (temp.complete())
                {
                    printf("\nRUN STATE - Sending proc to exit state...\n");
                    ++num_complete;
                    temp.completion_t = int(time(NULL) - start_t);
                    write(running_exit, &temp, sizeof(Proc));
                }
                else if (timeout)
                {
                    printf("\nRUN STATE - sending proc back to ready state...\n");
                    fcntl(running_ready_p, F_SETFL, F_LOCK);
                    write(running_ready_p, &temp, sizeof(Proc));
                }
                else if (block_proc)
                {
                    //cout<<temp<<"\n";
                    printf("\nRUN STATE - Sending proc to blocked state...\n");
                    fcntl(running_blocked, F_SETFL, F_LOCK);
                    write(running_blocked, &temp, sizeof(Proc));
                    printf("\nSENT PROC!\n");
                }
                timeout = false;
                block_proc = false;
            }

            time_t end_t = time(NULL);

            out<<"\nLog End Time: "<<ctime(&end_t)<<"\n\n\nMuhammad Farjad Ilyas\n18I-0436\nOS Assignment #2\n";
            strcpy(buffer, out.str().c_str());
            write(fd, buffer, strlen(buffer));

            cout<<out.str();
            out.str("");

            close(running_exit);
            int status = 10;
            waitpid(exit_pid, &status,0);

            printf("\nTHE STATUS IS: %d\n\n",int(WEXITSTATUS(status)));

            close(running_blocked);
            wait(NULL);

            close(ready_running);
            close(running_ready);
            close(running_ready_p);

            printf("\nClosing run state...\n");
        }

        //Child
        else if (exit_pid == 0)
        {
            printf("Creating exited state\n");
            execl("./exit_state", "./exit_state", NULL);
        }
        else
            printf("--- [[ Fork failed in new_state.cpp Line 47 ]] ---\n");
    }

    //Child
    else if (blocked_pid == 0)
    {
        printf("Creating blocked state\n");
        execl("./blocked_state", "./blocked_state", NULL);
    }
    else
        printf("--- [[ Fork failed in new_state.cpp Line 56 ]] ---\n");

    printf("Closing run_state\n");

    return 0;
}