#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <memory.h>

// 0 is for read and 1 is for write 
int main()
{
    int parentToChild[2];

    int childToParent[2];


    char message1[] = "message from child to parent\n";
    char message2[] = "message from parent to child\n";
    char readBuffer[120];
    pipe(parentToChild);
    pipe(childToParent);
    int processId , bytes;

    if((processId = fork())== -1)
    {
        printf("error while creating a child process");
    }

if(processId == 0) // child process 
{
    close(childToParent[0]);  // closing the read buffer of child to parent
    write(childToParent[1] , message1 , strlen(message1)+1);

    close(parentToChild[1]);   // closing the write buffer of parent to child
    bytes = read(parentToChild[0] , readBuffer , sizeof(readBuffer));
    printf("%s",readBuffer);

}

if(processId !=0 ) // parent process 
{
    close(parentToChild[0]);   // closing the read buffer of parent to child
    write(parentToChild[1] , message2 , strlen(message2)+1);
    close(childToParent[1]);    // closing the write buffer of child to parent
    bytes = read(childToParent[0] , readBuffer , sizeof(readBuffer));
    printf("%s", readBuffer);
}

return 0;
}