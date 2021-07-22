/* App to write to device*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "tboost_char_device.h"

int main()
{
	int fd, i;
	char my_message[40];
	unsigned long size;

	strcpy(my_message, "welcome to linux world");

	/* open the device for read/write/lseek */
	printf("[%d] - Opening device tboost_cdrv\n", getpid() );
	fd = open( "/dev/First_Char", O_RDWR );
	if( fd < 0 ) {
		printf("Device could not be opened\n");
		return 1;
	}	
	printf("Device opened with ID [%d]\n", fd);
	
	printf("Writing [%s]\n", my_message );
	/* write the contents of my buffer into the device */
	size = (unsigned long)write( fd, my_message, strlen(my_message) );
	printf("Bytes written %d\n", size);
	bzero( my_message, 20 );
	//lseek(fd,0,SEEK_SET);
	read(fd, my_message, 40 );
	printf("Written [%s] \n", my_message);
	
	/* Close the device */
	close(fd);
	
	/* Thats all folks */
	exit(0);
}


