#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int parentId = atoi(argv[1]);
    
    // Declaring the MX and MZ motor positions
    int mx_pos=0, mz_pos=0;
    char c;

    // Declaring nfd = number of file descriptors, res and fd
    int nfd;
    int res, fd_IMX2, fd_IMZ2, fd_IXreset, fd_IZreset;

    // Naming the pipes for MX and MZ
    char * IMX = "IMX_pipe";
    mkfifo (IMX,0666);

    char * IMZ = "IMZ_pipe";
    mkfifo (IMZ,0666);

    char * IXreset = "IX_reset_pipe";
    mkfifo(IXreset, 0666);

    char * IZreset = "IZ_reset_pipe";
    mkfifo(IZreset, 0666);

    // Opening the pipes for MX and MZ, with error handling
    fd_IMX2 = open(IMX,O_RDONLY);
    if (fd_IMX2 == -1){
        perror("Inspection Console, open() failure, line 38 ");
        sleep(3);
        exit(EXIT_FAILURE);
        }
    fd_IMZ2 = open(IMZ,O_RDONLY);
    if (fd_IMZ2 == -1){
        perror("Inspection Console, open() failure, line 44 ");
        sleep(3);
        exit(EXIT_FAILURE);
        }

    // This pipe is used to send reset signal only.
    fd_IXreset = open(IXreset,O_WRONLY);
    if (fd_IXreset == -1){
        perror("MX, open() failure, line 36 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }

    // This pipe is used to send reset signal only.
    fd_IZreset = open(IZreset,O_WRONLY);
    if (fd_IZreset == -1){
        perror("MX, open() failure, line 36 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }
    
    system ("/bin/stty raw");

    for (;;){
        // fds = the file descriptors set needed for select. Type is fd_set.
        // fd_IMX2, fd_IMZ2 = reading ends of named pipes between MX and MZ respectively.
        //use system call to make terminal send all keystrokes directly to stdin
        
        // Below line is used to make terminal send all keystrokes directly to stdin, whenever available.
        //system ("/bin/stty raw");

        //kill(parentId, SIGUSR1);// send active SIG to main
        fd_set fds;
        
        // Setting needed variables for select() system call.
        // FD_ZERO clears fds (file descriptor set) initially.
        // FD_SET is defined to include the pipes to make selection from.
        FD_ZERO(&fds);
        FD_SET(fd_IMX2, &fds);
        FD_SET(fd_IMZ2, &fds);
        // standard input is added for reset and stop buttons
        FD_SET(STDIN_FILENO, &fds);

        // The below line is a fancy assignment way of assigning the higher number of file descriptor to nfd.
        // If fd_IMX is bigger than fd_IMZ, than nfd = fd_IMX2, else nfd = fd_IMZ2
        nfd = fd_IMX2 > fd_IMZ2 ? fd_IMX2 : fd_IMZ2;

        // select() is called with necessary parameters. Note that maximum file descriptor number is increased by one,
        // no time out parameter is used, and we are only interested in the read operators. Error handling is made.
        res = select(nfd + 1, &fds, NULL, NULL, NULL);
        if (res == -1){
            perror("Inspection Console, select() failure, line 65 ");
            sleep(3);
            exit(EXIT_FAILURE);
        }
        
        // IMX pipe is checked if it has data in it.
        if (FD_ISSET(fd_IMX2, &fds))
        {
            // In case of existing data, open() is called to read it. Error handling is made for read().
            res = read(fd_IMX2, &mx_pos, sizeof(int));
            if (res > 0){
                printf("\r     X: %d cm ---- Z: %d cm      Please enter s for emergency, r for reset: ", mx_pos, mz_pos);
                fflush(stdout);
            }
            else if (res == 0){
                perror("Inspection Console, read() failure, line 76");
                exit(EXIT_FAILURE);
            }
            else {
                perror("Inspection Console, read() failure, line 76");
                exit(EXIT_FAILURE);
            }
        }

        // IMZ pipe is checked if it has data in it.
        if (FD_ISSET(fd_IMZ2, &fds)){
            // In case of existing data, open() is called to read it. Error handling is made for read().
            res = read(fd_IMZ2, &mz_pos, sizeof(int));
            if (res > 0){
                // In case of existing data, the values are printed on the screen
                printf("\r     X: %d cm ---- Z: %d cm      Please enter s for emergency, r for reset: ", mx_pos, mz_pos);
                fflush(stdout);
            }
            else if (res == 0){
                perror("Inspection Console, read() failure, line 94");
                exit(EXIT_FAILURE);
            }
            else {
                perror("Inspection Console, read() failure, line 94");
                exit(EXIT_FAILURE);
            }
        }
        
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            
            res = read(STDIN_FILENO, &c,sizeof(c));
            if (res > 0){
                // In case of existing data, the values are printed on the screen
                // printf("From stdin: ");printf("%c\n", c);
                // fflush(stdout);
                // if received input is r, then reset command is sent through pipe.
                // to both MX and MZ.
                if (c == 'r'){
                    write(fd_IXreset, &c, sizeof(c));
                    write(fd_IZreset, &c, sizeof(c));
                }
                // if the input is s, then SIGUSR2 is sent to newmain.c. Then it will handle it.
                else if (c == 's')  kill(parentId,SIGUSR2);
            }
            else if (res == 0){
                perror("Inspection Console, read() failure, line 94");
                exit(EXIT_FAILURE);
            }
            else {
                perror("Inspection Console, read() failure, line 94");
                exit(EXIT_FAILURE);
            }
            
        }
        }
        system ("/bin/stty cooked");
    
    
    // Closing the pipes, and unlinking the files with error handling.
    res = close(fd_IMX2);
    if (res == -1){
        perror("Inspection Console, close() failure, line 134 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }
    res = close(fd_IMZ2);
    if (res == -1){
        perror("Inspection Console, close() failure, line 140 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }

    res = unlink(IMX);
    if (res == -1){
        perror("Inspection Console, unlink() failure, line 147 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }
    res = unlink(IMZ);
    if (res == -1){
        perror("Inspection Console, unlink() failure, line 147 ");
        sleep(3);
        exit(EXIT_FAILURE);
    }
    // Below line is used to reset terminal to its initial state.
    // system ("/bin/stty cooked");
    return 0;
}