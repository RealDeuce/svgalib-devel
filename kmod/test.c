#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "svgalib_helper.h"

int main(int argc, char **argv)
{
	int fd;
	io_t io;

	fd = open("/dev/svga_helper/1", O_RDWR);
	if (fd < 0)
		exit(1);
	io.port = 0x3d5;
	io.val = 0;
	if (ioctl(fd, SVGAHELPER_INB, &io) != 0) {
		printf("err: %d\n", errno);
		exit(1);
	}
	printf("Go value: %u\n", io.val);
	close(fd);
	return 0;
}
