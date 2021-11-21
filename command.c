#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
int ppid = -1;

int main(int argc, char *argv[]){
    printf("Starting the program" );
    int c;
    int statusZ = 0; // assign 0 = stop, -1 = decreasing, and 1 = increasing
    int statusX = 0;
    char last_str[2];
    //int parentId = atoi(argv[1]);
    //kill(parentId, SIGUSR1);
    //use system call to make terminal send all keystrokes directly to stdin
    system ("/bin/stty raw");
    printf("Starting the program" );

    int fd_CMX, fd_CMZ;
    char * CMX = "CMX_pipe";
    char * CMZ = "CMZ_pipe";
    mkfifo(CMX, 0666);
    mkfifo(CMZ, 0666);
    char send_string[2];
    fd_CMX = open(CMX, O_WRONLY);
    fd_CMZ = open(CMZ, O_WRONLY);
    // signal(SIGALRM, timerInterupt);
    
    while((c=getchar())!= 'q') {
        /* type a period to break out of the loop,qq since CTRL-D won't work raw */
        fd_CMX = open(CMX, O_WRONLY);
        fd_CMZ = open(CMZ, O_WRONLY);
        switch (c) {
        case 'y':
            //printf("\rZ increasing                                                  ");
            statusZ = 1;
            printf("\r Status <X-axis:%d> <Z-axis:%d> (-1 = decreasing, 0 = stopping, and 1 = increasing)   ", statusX, statusZ);
            write(fd_CMZ, &c, sizeof(char));
            close(fd_CMZ);
            //kill(parentId, SIGUSR1);
            break;
        case 'h':
            //printf("\rZ stopping                                                  ");
            statusZ = 0;
            printf("\r Status <X-axis:%d> <Z-axis:%d> (-1 = decreasing, 0 = stopping, and 1 = increasing)   ", statusX, statusZ);
            write(fd_CMZ, &c, sizeof(char));
            close(fd_CMZ);
            //kill(parentId, SIGUSR1);
            break;
        case 'n':
            //printf("\rZ decreasing                                                ");
            statusZ = -1;
            printf("\r Status <X-axis:%d> <Z-axis:%d> (-1 = decreasing, 0 = stopping, and 1 = increasing)   ", statusX, statusZ);
            write(fd_CMZ, &c, sizeof(char));
            close(fd_CMZ);
            //kill(parentId, SIGUSR1);
            break;
        case 't':
            //printf("\rX decreasing                                                ");
            statusX = 1;
            printf("\r Status <X-axis:%d> <Z-axis:%d> (-1 = decreasing, 0 = stopping, and 1 = increasing)   ", statusX, statusZ);
            write(fd_CMX, &c, sizeof(char));
            close(fd_CMX);
            //kill(parentId, SIGUSR1);
            break;
        case 'g':
            //printf("\rX stopping                                                  ");
            statusX = 0;
            printf("\r Status <X-axis:%d> <Z-axis:%d> (-1 = decreasing, 0 = stopping, and 1 = increasing)   ", statusX, statusZ);
            write(fd_CMX, &c, sizeof(char));
            close(fd_CMX);
            //kill(parentId, SIGUSR1);
            break;
        case 'b':
            //printf("\rX increasing                                                ");
            statusX = -1;
            printf("\r Status <X-axis:%d> <Z-axis:%d> (-1 = decreasing, 0 = stopping, and 1 = increasing)   ", statusX, statusZ);
            write(fd_CMX, &c, sizeof(char));
            close(fd_CMX);
            //kill(parentId, SIGUSR1);
            break;
        default:
            printf("\rpress y h n (+, s, -) for Z-axis and t g b (-, s, +) for X-axis || press q for exit");
            break;
        }// end of swtich cases

        // printf("Sending complete");

  }//end of while loop (getchar)

  // use system call to set terminal behaviour to more normal behaviour
  system ("/bin/stty cooked");
  
  return 0;
}
