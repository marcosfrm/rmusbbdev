#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libudev.h>

int main(int argc, char **argv)
{
    struct udev *context = udev_new();
    struct udev_device *blkdev, *usbdev;
    struct stat sb;
    char *devname, *syspath;
    int fd;

    if (argc != 2)
    {
        fprintf(stderr, "uso: %s /dev/dispositivo\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    devname = strdup(argv[1]);

    if (stat(devname, &sb) != 0 || !S_ISBLK(sb.st_mode))
    {
        fprintf(stderr, "Dispositivo inexistente ou nao de bloco.\n");
        exit(EXIT_FAILURE);
    }

    if (strncmp(devname, "/dev/", 5) == 0)
    {
        devname += 5;
    }

    blkdev = udev_device_new_from_subsystem_sysname(context, "block", devname);
    if (blkdev != NULL)
    {
        usbdev = udev_device_get_parent_with_subsystem_devtype(blkdev, "usb", "usb_device");
    }

    if (usbdev != NULL)
    {
        // https://github.com/torvalds/linux/commit/253e05724f9230910344357b1142ad8642ff9f5a
        if (asprintf(&syspath, "%s%s", udev_device_get_syspath(usbdev), "/remove") > 0)
        {
            fd = open(syspath, O_WRONLY);
            if (fd >= 0)
            {
                printf("Removendo dispositivo USB... ");
                if (write(fd, "1", 1) == 1)
                {
                    printf("sucesso.\n");
                }
                else
                {
                    printf("falha.\n");
                }
                close(fd);
            }
        }
    }

    return EXIT_SUCCESS;
}
