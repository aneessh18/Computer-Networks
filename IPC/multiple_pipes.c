#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#define READ_END 0
#define WRITE_END 1

int main( int argc ,char * argv[])  // getting the programs
{
    argc--;
    int fd[argc][2];
    int pid[argc],c=0;
    while(c<argc)
    {
        if(pipe(fd[c])<0)
        {
            printf("pipe error");
            exit(0);
        }
        pid[c]=fork();
        if(pid[c]<0)
        {
            printf("fork error\n");
            exit(0);
        }
        else if(pid[c]==0)
        {
            if(c==0) // first process
            {
                close(fd[c][READ_END]);
            }
            else // take input from output
            {
                dup2(fd[c-1][READ_END],0);
            }

            if(c==argc-1) // last process
            {
                close(fd[c][WRITE_END]);
            }
            else // flush output to input
            {
                dup2(fd[c][WRITE_END],1);
            }
            
            // close all the opened pipes
            for(int i=0;i<=c;i++)
            {
                close(fd[i][READ_END]);
                close(fd[i][WRITE_END]);
            }
            char *args[100];
            args[0]=NULL;
            char path[100];
            strcpy(path,"./");
            strcat(path,argv[c+1]);
            printf("%s\n",path);
            execvp(path, args);
        }
        else
        {
            c++;
            wait(NULL);
            if(c==argc-1)
            {
                for(int i=0;i<=c;i++)
                {
                     close(fd[i][READ_END]);
                    close(fd[i][WRITE_END]);
                }
            }
        }
        
    }
}