#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/types.h>
#include "util.h"
#include<time.h>
//#include<pthread>

using namespace std;


//PACKAGE STRUCT for Inter-Thread Communication
//Holds all three queues

struct Package
{
    BlockedItem temp;
    BlockQueue input_q;
    BlockQueue output_q;
    BlockQueue printer_q;
};


//------------------ [[ INITIALIZING PTHREAD VARIABLES ]] -------------

pthread_t i_tid, o_tid, p_tid;

pthread_cond_t i_cond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t o_cond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t p_cond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock;  

//--------------------------------------------------------------------




// Input Thread Function - unlocked when the relevant process has been read into temp
// The process is then enqueued into the Input Queue

void * InputHandler(void * arg)
{
    Package * pck = (Package *)arg;

    bool lp_cont = true;
    int rn_stat = -1, r_block_t;


    while (lp_cont)
    {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&i_cond, &lock);

        (*pck).input_q.enqueue(pck->temp);

        printf("------------------= INPUT BLOCK QUEUE =----------------\n");
        (*pck).input_q.display();
        printf("------------------==----------------\n\n");

        pthread_mutex_unlock(&lock); 
    }

}


//Output Thread Function

void * OutputHandler(void * arg)
{
    Package * pck = (Package *)arg;

    bool lp_cont = true;
    int rn_stat = -1, r_block_t;

    while (lp_cont)
    {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&o_cond, &lock);

        (*pck).output_q.enqueue(pck->temp);

        printf("------------------= OUTPUT BLOCK QUEUE =----------------\n");
        (*pck).output_q.display();
        printf("------------------==----------------\n\n");

        pthread_mutex_unlock(&lock); 
    } 
}


// Printer Thread Function

void * PrinterHandler(void * arg)
{
    Package * pck = (Package *)arg;

    bool lp_cont = true;
    int rn_stat = -1, r_block_t;

    while (lp_cont)
    {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&p_cond, &lock);

        (*pck).printer_q.enqueue(pck->temp);

        printf("------------------= PRINTER BLOCK QUEUE =----------------\n");
        (*pck).printer_q.display();
        printf("------------------==----------------\n\n");

        pthread_mutex_unlock(&lock); 
    }

}



int main()
{
    srand((unsigned)time(0));

    printf("Welcome to the blocked state!\n");


    //OPENING FIFO PIPES

    int running_blocked = open("RUNNING_BLOCKED", O_RDONLY | O_NONBLOCK);
    int blocked_ready = open("BLOCKED_READY", O_WRONLY | O_TRUNC);



    //----------------- [[ INITIALIZING VARIABLES ]] --------------------

    bool lp_cont = true;
    int rn_stat, r_block_t, block_id = 0;
    Package pck;

    if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    int i_in, o_in, p_in;

    //-------------------------------------------------------------------



    //Creating three threads to handle three queues

    i_in = pthread_create(&i_tid,NULL,InputHandler, &pck);
    o_in = pthread_create(&o_tid, NULL, OutputHandler, &pck);
    p_in = pthread_create(&p_tid, NULL, PrinterHandler, &pck);




    //------------------------------- [[ BLOCKED STATE I/O LOOP ]] -------------------------

    while (lp_cont)
    {

        //MUTEX LOCK ENFORCED

        pthread_mutex_lock(&lock);

        rn_stat = read(running_blocked, &pck.temp.process, sizeof(Proc));

        switch(rn_stat)
        {
            case(-1):
                break;

            //PIPE IS CLOSED FROM THE OTHER END -- END OF COMMUNCATION
            case(0):
                
                printf("\nBLOCKED STATE -- EXITING OUT OF IN/OUT LOOP\n");
                pthread_cancel(i_tid);
                pthread_cancel(o_tid);
                pthread_cancel(p_tid);
                lp_cont = false;
                close(running_blocked);
                close(blocked_ready);

                printf("Closing blocked state\n");
                exit(0);
                
                break;



            // READ SUCCESSFUL - PROC RECEIVED FROM RUNNING

            default:
                r_block_t = 15 + rand()%11;            // Setting random block time BETWEEN 15-25s
                block_id = rand() % 3;

                pck.temp.setBlockDetails(r_block_t);


                // SEND SIGNAL TO APPROPRIATE THREAD - FOR ENQUEUEING

                switch(block_id)
                {
                    case(0):
                        printf("\nBLOCKED STATE - INPUT QUEUE: Received %s, blocked for: %d\n", pck.temp.process.name, r_block_t);
                        pthread_cond_signal(&i_cond);
                        break;
                    case(1):
                        printf("\nBLOCKED STATE - OUTPUT QUEUE: Received %s, blocked for: %d\n", pck.temp.process.name, r_block_t);
                        pthread_cond_signal(&o_cond);
                        break;
                    case(2):
                        printf("\nBLOCKED STATE - PRINTER QUEUE: Received %s, blocked for: %d\n", pck.temp.process.name, r_block_t);
                        pthread_cond_signal(&p_cond);
                        break;
                    default:
                        printf("\nBLOCK STATE ERROR! - UNDEFINED QUEUE\n\n");
                        break;
                }
                break;
        }

        pthread_mutex_unlock(&lock);

        
        //SLEEP TO IMPLEMENT BLOCKED TIME
        
        sleep(1);
        
        pthread_mutex_lock(&lock);



        // INCREMENT BLOCKED TIME OF ALL PROCESSES

        pck.input_q.incBlockTime();
        pck.output_q.incBlockTime();
        pck.printer_q.incBlockTime();
        


        //EXTRACT UNBLOCKED PROCESSES FROM ALL QUEUES AND WRITE TO READY STATE

        Vector<BlockedItem> unblocked_l = pck.input_q.extractUnblocked();

        for (int i = 0 ; i < unblocked_l.Size(); ++i)
        {
            printf("\nBLOCKED STATE - Sending process to READY..\n");
            write(blocked_ready, &unblocked_l[i].process, sizeof(Proc));
        }

        unblocked_l = pck.output_q.extractUnblocked();

        for (int i = 0 ; i < unblocked_l.Size(); ++i)
        {
            printf("\nBLOCKED STATE - Sending process to READY..\n");
            write(blocked_ready, &unblocked_l[i].process, sizeof(Proc));
        }

        unblocked_l = pck.printer_q.extractUnblocked();

        for (int i = 0 ; i < unblocked_l.Size(); ++i)
        {
            printf("\nBLOCKED STATE - Sending process to READY..\n");
            write(blocked_ready, &unblocked_l[i].process, sizeof(Proc));
        }

        pthread_mutex_unlock(&lock);
    }

    return 0;
}
