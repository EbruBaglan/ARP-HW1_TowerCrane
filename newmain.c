#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

pid_t commID;
pid_t insID;
pid_t MXID;
pid_t MZID;
pid_t signal_PID;
int inssig = 0;
int commsig = 0;
int MXsig = 0;
int MZsig = 0;
int buff_PID = 0;
char pid_str[10];

void receive_PIDs(int signal, siginfo_t *info, void *context){
    signal_PID = info->si_pid;
    buff_PID = signal_PID;

    if (signal_PID == insID){
        inssig = 1;

        // if the signal is from Inspection Console and it is SIGUSR2, then nemwmain.c sends SIGUSR2 to MX and MZ
        // example code: https://www.linuxquestions.org/questions/programming-9/catching-multiple-signals-in-a-single-handler-in-linux-c-877096/
        if (signal == SIGUSR2){
            kill(MXID, SIGUSR2);
            kill(MXID, SIGUSR2);
        }
    }
    // upon receiving signals, the flags are set to 1 for watchdog purposes.        
    else if (signal_PID == commID)  commsig = 1;
    else if (signal_PID == MXID)    MXsig = 1;
    else if (signal_PID == MZID)    MZsig = 1;
    else printf("Unkknown signal to watchdog");

    while(1){
    pause();//waiting for signal
    }
}

int open_with_konsole(const char * path_konsole, char* path_code){
    pid_t child_pid = fork();
    switch (child_pid){
    case 0:
        /* child process */
        snprintf(pid_str, sizeof(pid_str), "%d", getppid());
        execl(path_konsole, path_konsole, "-e", path_code, pid_str, NULL);
        return 0;
        break;
    
    default:
        break;
    }
}

int open_WITHOUT_konsole(char * path_code){
    pid_t child_pid = fork();
    switch (child_pid){
    case 0:
        /* child process */
        snprintf(pid_str, sizeof(pid_str), "%d", getppid());
        execl(path_code, path_code, pid_str, NULL);
        return 0;
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[]){
  // signal handling parameters are set
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = receive_PIDs;
  sigaction(SIGUSR1, &sa, NULL);

  // to be able to deal with 2 different signals, SIGUSR2 is also added to sigaction
  // example code: https://www.linuxquestions.org/questions/programming-9/catching-multiple-signals-in-a-single-handler-in-linux-c-877096/
  sigaction(SIGUSR2, &sa, NULL);

  commID = open_with_konsole("/usr/bin/konsole", "./command");
  printf("Command process was spwaned with PID: %d\n", commID);

  insID = open_with_konsole("/usr/bin/konsole", "./inspection");
  printf("Inspection process was spwaned with PID: %d\n", insID);

  MXID = open_WITHOUT_konsole("./MX");
  printf("MX was spwaned with PID: %d\n", MXID);

  MZID = open_WITHOUT_konsole("./MZ");
  printf("MZ was spwaned with PID: %d\n", MZID);

  while(1){
    pause();//waiting for signal
  }
}