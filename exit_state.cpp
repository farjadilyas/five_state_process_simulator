#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/types.h>
#include "util.h"

using namespace std;




int main()
{
    printf("Welcome to the exited state\n");



    // DECLARING VARIABLES & OPENING PIPE FROM RUNNING

    bool lp_cont = true;
    int rn_in = -1, cur_avg_turnaround_t = 0, cur_avg_waiting_t = 0, cur_throughput = 0, final_avg_turnaround_t = 0, final_avg_waiting_t = 0, final_throughput = 0,cur_t_lim = 30;
    Proc temp;

    int running_exit = open("RUNNING_EXIT", O_RDONLY);
    fcntl(running_exit, F_SETFL, O_NONBLOCK);




    // HANDLING NUMBER OF PROCESSES & ALGO----------------------

    int num_proc_stat = -1, num_proc = 0, algo_stat, algo = 0;

    while (num_proc_stat != 4)
        num_proc_stat = read(running_exit, &num_proc, sizeof(int));

    while (algo_stat != 4)
        algo_stat = read(running_exit, &algo, sizeof(int));

    //---------------------------------------------------------



    // SAVE START_T FOR UPDATING LOG EVERY 30s

    time_t start_t = time(NULL);


    // DECLARE STRINGSTREAM OBJECT TO APPEND TEXT FILE OUTPUTS TO BEFORE WRITING TO FILE

    stringstream out;
    out<<setfill('=')<<setw(69)<<"=\n"<<"||\t\t\tPROCESSES STATISTICS\t\t\t  ||\n"<<setw(70)<<"=\n\n"<<setfill(' ');
    out<<"Log Start Time: "<<ctime(&start_t)<<"\nScheduling Algorithm: ";
    switch(algo)
    {
        case(FCFS): out<<"First Come First Serve (FCFS)\n";         break;
        case(RR):   out<<"Round Robin (RR)\n";                      break;
        case(SJF):  out<<"Shortest Job First (SJF)\n";              break;
        case(SRTF): out<<"Shortest Remaining Time First (SRTF)\n";  break;
        default:    printf("ERROR IN EXIT'S ALGO\nExiting...\n");   exit(0); 
    }
    out<<left<<setfill(' ')<<"\n\n\n| "<<setw(13)<<"Time Period"<<"| "<<setw(23)<<"Avg. Waiting Time"<<"| "<<setw(26)<<"Avg. Turnaround Time"<<"| "<<setw(14)<<"Throughput"<<"| "<<"\n";
    out<<setfill('-')<<setw(84)<<"-"<<setfill(' ')<<"\n"<<setprecision(2)<<fixed;
    
    
    //VECTOR TO HOLD EXITED PROCESS DETAILS
    
    Vector<Proc> f_proc(num_proc);



    //LOOP RUNS NUM_PROC TIMES
    
    for (int i = 0 ; i < num_proc; ++i)
    {

        //RUN TILL PROCESS READ FROM RUNNING

        while (rn_in == -1 || rn_in == 0)
        {
            rn_in = read(running_exit, &temp, sizeof(Proc));
        }


        //APPEND STATISTICS OF PREVIOUS 30 seconds if current process finished beyond that

        if (time(NULL) - start_t >= cur_t_lim)
        {
            out<<left<<"| "<<setw(4)<<cur_t_lim - 30<<"- "<<setw(7)<<cur_t_lim<<"| "
            <<setw(23)<<cur_avg_waiting_t/float(cur_throughput)<<"| "
            <<setw(26)<<cur_avg_turnaround_t/float(cur_throughput)<<"| "
            <<setw(14)<<cur_throughput<<"| "<<"\n";
            out<<setfill('-')<<setw(84)<<"-"<<setfill(' ')<<"\n";

            cur_t_lim += 30;
            cur_avg_waiting_t = 0;
            cur_avg_turnaround_t = 0;
            cur_throughput= 0;
        }


        //Update overall statistics

        final_avg_turnaround_t += temp.completion_t - temp.arrival_t;
        final_avg_waiting_t+= temp.waiting_t;
        ++final_throughput;


        //Update current time block statistics

        cur_avg_turnaround_t += temp.completion_t - temp.arrival_t;
        cur_avg_waiting_t += temp.waiting_t;
        ++cur_throughput;


        //Add process to Exited Process Vector

        f_proc.push_back(temp);
        printf("RN_IN is: %i\n",rn_in);
        rn_in = -1;
    }


    //Append any time blocks that didnt have any processes
    
    while (temp.completion_t > cur_t_lim)
    {
        out<<left<<"| "<<setw(4)<<cur_t_lim - 30<<"- "<<setw(7)<<cur_t_lim<<"| "
        <<setw(23)<<0/float(1)<<"| "
        <<setw(26)<<0/float(1)<<"| "
        <<setw(14)<<0/float(1)<<"| "<<"\n";
        out<<setfill('-')<<setw(84)<<"-"<<setfill(' ')<<"\n";
        
        cur_t_lim += 30;
    }


    //Append last time block (couldn't be done before.. logic only appends previous blocks)

    out<<left<<"| "<<setw(4)<<cur_t_lim - 30<<"- "<<setw(7)<<cur_t_lim<<"| "
    <<setw(23)<<cur_avg_waiting_t/float(cur_throughput)<<"| "
    <<setw(26)<<cur_avg_turnaround_t/float(cur_throughput)<<"| "
    <<setw(14)<<cur_throughput<<"| "<<"\n";
    out<<setfill('-')<<setw(84)<<"-"<<setfill(' ')<<"\n";


    //CLOSE FILE DESCRIPTOR

    close(running_exit);



    //-------------------------------------- [[ POST PROCESSING: WRITE TO RUN TIME STATS TO "process_stats.txt" ]] -------------------------------

    time_t end_t = time(NULL);

    out<<"\nCumulative Statistics: \n\nAverage Waiting time: "<<final_avg_waiting_t/float(final_throughput)
    <<"\n\nAverage Turnaround Time: "<<final_avg_turnaround_t/float(final_throughput)
    <<"\n\nTotal Throughput: "<<final_throughput<<"\n";

    out<<"\nLog End Time: "<<ctime(&end_t)<<"\n\n\nMuhammad Farjad Ilyas\n18I-0436\nOS Assignment #2\n";

    char buffer[4096];

    strcpy(buffer, out.str().c_str());

    int fd = open("process_stats.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    int stat = write(fd, buffer, strlen(buffer));

    out.str("");

    //-----------------------------------------------------------------------------------------------------------------------------------------



    
    

    //------------------------------ [[ POST PROCESSING: WRITE EXITED PROC INFO TO "exited_processes.txt" ]] -----------------------------------------
    
   
    printf("\n\n\nThe exited processes are: \n\n");
    
    out<<setfill('=')<<setw(69)<<"="<<"\n"<<"||\t\t\tEXITED PROCESSES\t\t\t  ||\n"<<setw(70)<<"="<<setfill(' ');
    out<<"\n\nLog Start Time: "<<ctime(&start_t)<<"\nScheduling Algorithm: ";
    switch(algo)
    {
        case(FCFS): out<<"First Come First Serve (FCFS)\n";         break;
        case(RR):   out<<"Round Robin (RR)\n";                      break;
        case(SJF):  out<<"Shortest Job First (SJF)\n";              break;
        case(SRTF): out<<"Shortest Remaining Time First (SRTF)\n";  break;
        default:    printf("ERROR IN EXIT'S ALGO\nExiting...\n");   exit(0); 
    }

    out<<setfill(' ')<<setw(10)<<left<<"\n\nP_NAME"<<setw(13)<<"ARRIVAL_T"<<setw(11)<<"BURST_T"<<setw(10)<<"WAIT_T"
    <<setw(12)<<"FINISH_T"<<setw(16)<<"TURNAROUND_T"<<"\n";
    for (int i = 0 ; i < f_proc.Size(); ++i)
    {
        out<<f_proc[i];
    }

    out<<"\nLog End Time: "<<ctime(&end_t)<<"\n\n\nMuhammad Farjad Ilyas\n18I-0436\nOS Assignment #2\n";
    cout<<out.str();

    fd = open("exited_processes.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    strcpy(buffer, out.str().c_str());

    stat = write(fd, buffer, strlen(buffer));

    //----------------------------------------------------------------------------------------------------------------------------------------------


    printf("\nClosing exit_state\n");

    exit(32);
}