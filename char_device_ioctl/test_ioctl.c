#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "ioctl_const.h"

int main()
{
	int fd, i;
	char my_buf[4000];
	
	unsigned int size;
	char c;
	char c_array[10]={'1','2','3','4','5','6','7','8'};	
	
	printf("[%d] - Opening device CharDev_ioctl\n", getpid() );

	fd = open("/dev/CharDev_ioctl", O_RDWR); 
	if(fd < 0 ) {
  	   printf("\n\nDevice could not be opened\n\n");
  	   return 1;
	}
	printf("Device opened with ID [%d]\n", fd);
	
	   
	ioctl(fd, BOOST_GET_SIZE, &size);

	printf("Present size of the buffer is %d\n", size);
	size = 4096;
	printf("Setting size of buffer to 4096\n");

	if(ioctl(fd, BOOST_SET_SIZE, &size) < 0 ) {
	   printf("ioctl failed\n");
           return -1;
	}

	ioctl(fd, BOOST_GET_SIZE, &size);
	printf("New size of the buffer is %d\n", size);		

	
	/* lets fill the buffer with character '=' */
	c = 'A';
	ioctl(fd, BOOST_FILL_CHAR, &c);

      	/* test if it worked */
	read(fd, my_buf, 3000);
	printf("New contents\n%s\n", my_buf);
	//read(fd, c_array, 10);
	ioctl(fd, WRITE_VALUE, c_array);	
	ioctl(fd, READ_VALUE, my_buf);	
	printf("New contents after read ioctl\n%s\n", my_buf);	
	close(fd);
	
	exit(0);
}


