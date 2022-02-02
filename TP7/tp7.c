#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

char inbuffer[1024];
int main(){
    int pid = getpid();
    char hostname[100];
    gethostname(hostname,100);
    
    while(1){
        if(fgets(inbuffer,1024,stdin) == NULL) break;
        char *dest = inbuffer;
        while(*dest){
            *dest= toupper( *dest);
            dest++;
        }
        printf("%s, %d, %s", hostname, pid, inbuffer);

    }
    // while(*dest){
    //     *dest= toupper( *dest);
    //     dest++;
    // }
    // printf("%s, %d, %s", hostname, pid, inbuffer);
}