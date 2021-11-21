#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

int z_pos, z_max = 200;

// The instructions are passed as char through the pipe
char command;

// This is SIGUSR2 handler for the signal coming from newmain.c, which is stop signal from Inspection Console.
void stop_handler(int sig){
    command == 'h';
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    /* set sa to zero using the memset() C library function */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &stop_handler;
    sigaction (SIGUSR2, &sa, NULL);

    int parentId = atoi(argv[1]);
    // deactivated the watchdog purposed codes to deal with SIGUSR2.
    //kill(parentId, SIGUSR1);

    // fds, dummy = the file descriptors set needed for select.
    // dummy is the passed set, whereas fds is used to reset it.
    // nfd = maximum file descriptors' number.
    fd_set fds, dummy;
    int res, fd_IMZ1, fd_CMZ, fd_IZreset;
    int nfd, nfd2;

    // Naming the pipes for Inspection, Command and reset pipe
    char * IMZ = "IMZ_pipe";
    mkfifo(IMZ, 0666);

    char * CMZ = "CMZ_pipe";
    mkfifo(CMZ, 0666);

    char * IZreset = "IZ_reset_pipe";
    mkfifo(IZreset, 0666);

    // Opening the pipes for Inspection, Command and reset pipe with error handling
    fd_CMZ = open(CMZ, O_RDONLY);
    if (fd_CMZ == -1){
        perror("MZ, open() failure, line 29 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }

    fd_IMZ1 = open(IMZ,O_WRONLY);
    if (fd_IMZ1 == -1){
        perror("MZ, open() failure, line 36 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }
    
    // This pipe is used to receive reset command only.
    fd_IZreset = open(IZreset,O_RDONLY);
    if (fd_IZreset == -1){
        perror("MX, open() failure, line 36 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }
    
    // Setting needed variables for select() system call.
    // FD_ZERO clears fds (file descriptor set) initially.
    // FD_SET is defined to include the pipes to make selection from.
    FD_ZERO(&fds);
    FD_SET(fd_CMZ, &fds);
    FD_SET(fd_IZreset, &fds);

    while(1){
        // dummy is reset with fds file descriptor set.
        dummy = fds;

        // Timeout parameters necessary for select() are assigned
        // 1.0 s is used as timeout value.
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        // The below line is a fancy assignment way of assigning the higher number of file descriptor to nfd.
        // If fd_IMX1 is bigger than fd_CMX, then nfd = fd_IMX1, else nfd = fd_CMX
        nfd2 = fd_IMZ1 > fd_CMZ ? fd_IMZ1 : fd_CMZ;
        nfd = nfd2 > fd_IZreset ? nfd2 : fd_IZreset;
        
        // select() is called with necessary parameters. Note that maximum
        // file descriptor number is increased by one, and error handling is made.
        res = select(nfd + 1, &dummy, NULL, NULL, &tv);
        if (res == -1){
            perror("MZ, select() failure, line 45 ");
            sleep(3);
            exit(EXIT_FAILURE);
        }
        
        if (FD_ISSET(fd_CMZ, &dummy))
        {
            // In case of existing data, open() is called to read it. Error handling is made for read().
            res = read(fd_CMZ, &command, sizeof(char));
            if (res == 0){
                perror("MX, read() failure, line 50 ");
                sleep(3);
                exit(EXIT_FAILURE);
            }
            else if (res == -1){
                perror("MX, read() failure, line 50 ");
                sleep(3);
                exit(EXIT_FAILURE);
            }
        }
        
        if (FD_ISSET(fd_IZreset, &dummy))
        {
            // In case of existing data, open() is called to read it. Error handling is made for read().
            res = read(fd_IZreset, &command, sizeof(char));
            if (res == 0){
                perror("MX, read() failure, line 50 ");
                sleep(3);
                exit(EXIT_FAILURE);
            }
            else if (res == -1){
                perror("MX, read() failure, line 50 ");
                sleep(3);
                exit(EXIT_FAILURE);
            }
        }

        // Depending on command received, x_pos is updated to its new value and
        // sent using write() system call. Error handling is made.    
        switch(command){
            // reset comand
            case 'r':
                z_pos = 0 ;
                res = write(fd_IMZ1, &z_pos, sizeof(int));
                if (res == 0){
                    perror("MX, write() failure ");
                    sleep(3);
                    exit(EXIT_FAILURE);
                }
                else if (res == -1){
                    perror("MX, write() failure ");
                    sleep(3);
                    exit(EXIT_FAILURE);
                }
                break;
            
            case 'y':
                z_pos = z_pos + 10 + rand()%3; // error is introduced.
                if (z_pos >= z_max) z_pos = z_max;
                res = write(fd_IMZ1, &z_pos, sizeof(int));
                if (res == 0){
                    perror("MZ, write() failure ");
                    sleep(3);
                    exit(EXIT_FAILURE);
                }
                else if (res == -1){
                    perror("MZ, write() failure ");
                    sleep(3);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'h':
                res = write(fd_IMZ1, &z_pos, sizeof(int));
                if (res == 0){
                    perror("MZ, write() failure ");
                    sleep(3);
                    exit(EXIT_FAILURE);
                }
                else if (res == -1){
                    perror("MZ, write() failure ");
                    sleep(3);
                    exit(EXIT_FAILURE);
                }
                break;
            
            case 'n':
                z_pos = z_pos - 10 - rand()%3; // error is introduced.
                if (z_pos <= 0) z_pos = 0;
                res = write(fd_IMZ1, &z_pos, sizeof(int));
                if (res == 0){
                    perror("MZ, write() failure ");
                    sleep(3);
                    exit(EXIT_FAILURE);
                }
                else if (res == -1){
                    perror("MZ, write() failure ");
                    sleep(3);
                    exit(EXIT_FAILURE);
                }
                break;

            default:
                break;
        }
        //kill(parentId, SIGUSR1);
    }
    
    // Closing the pipes, and unlinking the files with error handling.
    res = close(fd_IMZ1);
    if (res == -1){
        perror("MZ, close() failure, line 208 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }
    res = close(fd_CMZ);
    if (res == -1){
        perror("MZ, close() failure, line 214 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }

    res = close(fd_IZreset);
    if (res == -1){
        perror("MZ, close() failure, line 221 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }

    res = unlink(IMZ);
    if (res == -1){
        perror("MZ, unlink() failure, line 228 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }
    res = unlink(CMZ);
    if (res == -1){
        perror("MZ, unlink() failure, line 234 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }

    res = unlink(IZreset);
    if (res == -1){
        perror("MZ, unlink() failure, line 241 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }

}