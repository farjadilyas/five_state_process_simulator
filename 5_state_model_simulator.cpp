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
    printf("Welcome to the starter program!\n");

    pid_t new_pid = fork();

    //Parent process
    if (new_pid > 0)
    {
        wait(NULL);
    }

    //Child process - NEW STATE
    else if (new_pid == 0)
    {
        execl("./new_state","./new_state", NULL);
        printf("--- [[ Exec failed in start program ]] ---\n");
    }
    else
        printf("--- [[ Fork failed in q1.cpp ]] ---\n");

    printf("Closing starter program\n");
    return 0;
}