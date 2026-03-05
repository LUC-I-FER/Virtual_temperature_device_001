#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DRIVER_MAGIC 'T'
#define IOCTL_SET_TEMP      _IOW(DRIVER_MAGIC, 1, int)
#define IOCTL_GET_TEMP      _IOR(DRIVER_MAGIC, 2, int)
#define IOCTL_SET_INTERVAL  _IOW(DRIVER_MAGIC, 3, int)

int main()
{
    int fd = open("/dev/driver_002_0", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    int temp = 100;
    ioctl(fd, IOCTL_SET_TEMP, &temp);

    int current;
    ioctl(fd, IOCTL_GET_TEMP, &current);
    printf("Current temp: %d\n", current);

    int interval = 1000;
    ioctl(fd, IOCTL_SET_INTERVAL, &interval);

    close(fd);
    return 0;
}