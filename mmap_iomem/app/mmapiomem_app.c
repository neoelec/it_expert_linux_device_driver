#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/vt.h>

#define DEVICE_FILENAME		"/dev/mmapiomem"

#define MMAP_SIZE		0x8000

int main(int argc, char *argv[])
{
	int ttydev;
	int dev;
	int loop, loop2;
	char *ptrdata;

	ttydev = open("/dev/tty0", O_RDWR | O_NDELAY);
	if (ttydev >= 0) {
		printf("ACT %d\n", ioctl(ttydev, VT_ACTIVATE, 9));
		printf("ACT %d\n", ioctl(ttydev, VT_ACTIVATE, 1));
		close(ttydev);
	}

	dev = open(DEVICE_FILENAME, O_RDWR | O_NDELAY);
	if (dev >= 0) {
		ptrdata = (char *)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE,
				MAP_SHARED, dev, 0);
		if (ptrdata) {
			for (loop2 = 0; loop2 < 5; loop2++) {
				for (loop = 0; loop < 80 * 25 ; loop++)
					ptrdata[loop * 2] = loop2 + '0';
				ptrdata[0] = 'O';
				ptrdata[2] = 'K';
				sleep(1);
			}
			munmap(ptrdata, MMAP_SIZE);
		}
		close(dev);
	}

	return 0;
}
