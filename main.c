// NOTE: The Maestro's serial mode must be set to "USB Dual Port".
 
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
 
#ifdef _WIN32
#define O_NOCTTY 0
#else
#include <termios.h>
#endif
 
// Gets the position of a Maestro channel.
// See the "Serial Servo Commands" section of the user's guide.
int maestroGetPosition(int fd, unsigned char channel) {
	unsigned char command[] = {0x90, channel};
	if(write(fd, command, sizeof(command)) == -1) {
		perror("error writing");
		return -1;
	}
	 
	unsigned char response[2];
	if(read(fd,response,2) != 2) {
		perror("error reading");
		return -1;
	}
	 
	return response[0] + 256*response[1];
}
 
// Sets the target of a Maestro channel.
// See the "Serial Servo Commands" section of the user's guide.
// The units of 'target' are quarter-microseconds.
int maestroSetTarget(int fd, unsigned char channel, unsigned short target) {
	unsigned char command[] = {0x84, channel, target & 0x7F, target >> 7 & 0x7F};
	if (write(fd, command, sizeof(command)) == -1) {
		perror("error writing");
		return -1;
	}
	return 0;
}
 
int main(char* argv[], int argc) {
#ifdef _WIN32
	const char * device = "\\\\.\\USBSER000";  // Windows, "\\\\.\\COM6" also works
#else
	const char * device = "/dev/ttyACM0";  // Linux
#endif

	printf("Opening serial port...\n");
	int fd = open(device, O_RDWR | O_NOCTTY);
	if (fd == -1) {
		perror(device);
		return 1;
	}
	printf("Serial port opened.\n"); 
 
#ifndef _WIN32
	struct termios options;
	tcgetattr(fd, &options);
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	options.c_oflag &= ~(ONLCR | OCRNL);
	tcsetattr(fd, TCSANOW, &options);
#endif
	 
	printf("Reading position...\n");
	int position = maestroGetPosition(fd, 0);
	printf("Current position is %d.\n", position);

	float i=0.0f;

	int min = 2300;
	int max = 12000;
	int steps = 4;

	maestroSetTarget(fd, 0, min);
	for(i=0; i<steps; i++){
		int target = (int)((i/steps)*(max-min) + min);
		printf("Setting target to %d\n", target);
		maestroSetTarget(fd, 0, target);
		sleep(1);
	}
	maestroSetTarget(fd, 0, min);
	return 0;
}
