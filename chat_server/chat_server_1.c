#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    FILE * fp=popen("./process2","r"); // taking input from process2
    dup2(fileno(fp),0);
    char str[100];
    scanf("%s",str);
   printf("%s\n",str);
    fp=popen("./process1","w"); // process1 is taking input we are not changing stdout
     fprintf(fp,"%s\n",str);
    fp=popen("./process3","w"); // process3 is taking input
    fprintf(fp,"%s\n",str);
    fp=popen("./process4","w"); // process4 is taking input
    fprintf(fp,"%s\n",str);
    fp=popen("./process5","w"); // process5 is taking input
    fprintf(fp,"%s\n",str);

    return 0;
}