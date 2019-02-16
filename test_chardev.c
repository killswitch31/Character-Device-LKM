#include <unistd.h>     
#include <fcntl.h>      
#include <stdio.h>
#include <stdlib.h>     
#include <string.h>

int main(int args, char *argv[])
{
int BUFFER_SIZE = 256;

char internal_buf[BUFFER_SIZE];
int to_read = 0;

memset(internal_buf,0,BUFFER_SIZE);

if (args < 3) {
    printf("2 Input arguments needed\nTo read 10 bytes: \"%s read 10\" \
    \nTo write string \"hello\": \"%s write hello\"\nExiting..\n", argv[0], argv[0]);
    return 1;
}



if (strcmp(argv[1],"write") == 0) {

    printf("\n\ninput lenght:%d\n", strlen(argv[2]));

    if(strlen(argv[2]) >= BUFFER_SIZE) {
        printf("too long input string, max buffer [%d]\nExiting..\n", BUFFER_SIZE);
        return 2;
    }

    printf("\n\nBegin write operation\n\n");
    memcpy(internal_buf,argv[2], strlen(argv[2]));

    printf("Writing [%s]\n\n", internal_buf);

    FILE * filepointer;
    filepointer = fopen("/dev/chardev", "w+");
    fwrite(internal_buf, sizeof(char) , strlen(argv[2]) + 1, filepointer);
    fclose(filepointer);

} else if (strcmp(argv[1],"read") == 0) {
    printf("\n\nBegin read operation\n\n");

    to_read = atoi(argv[2]);
	//printf("\nto_read:%d\n", to_read);
    FILE * filepointer;
    filepointer = fopen("/dev/chardev", "r");
    int retval = fread(internal_buf, sizeof(char), to_read, filepointer);
    fclose(filepointer);

    printf("Read %d bytes from driver. Driver replied: [%s]\n\n", retval, internal_buf);
} else {
    printf("first argument has to be 'read' or 'write'\nExiting..\n");
    return 1;
}


return 0;
}
