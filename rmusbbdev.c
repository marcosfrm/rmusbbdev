#define _FILE_OFFSET_BITS 64
// asprintf, basename
#define _GNU_SOURCE

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libudev.h>

int main(int argc, char **argv)
{
    struct stat sb;
    struct udev *ucxt;
    struct udev_device *blkdev, *usbdev;
    char *devname, *syspath;
    int fd;

    if (argc != 2)
    {
        fprintf(stderr, "uso: %s dispositivo\n", basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &sb) != 0 || !S_ISBLK(sb.st_mode))
    {
        fprintf(stderr, "Dispositivo inexistente ou não de bloco.\n");
        exit(EXIT_FAILURE);
    }

    devname = realpath(argv[1], NULL);
    if (devname == NULL)
    {
        fprintf(stderr, "Falha ao determinar caminho canônico do dispositivo: %m\n");
        exit(EXIT_FAILURE);
    }

    ucxt = udev_new();
    blkdev = udev_device_new_from_subsystem_sysname(ucxt, "block", basename(devname));
    if (blkdev != NULL)
    {
        usbdev = udev_device_get_parent_with_subsystem_devtype(blkdev, "usb", "usb_device");
        if (usbdev != NULL)
        {
            // https://github.com/torvalds/linux/commit/253e05724f9230910344357b1142ad8642ff9f5a
            if (asprintf(&syspath, "%s/remove", udev_device_get_syspath(usbdev)) > 0)
            {
                fd = open(syspath, O_WRONLY);
                if (fd >= 0)
                {
                    printf("Desconectando porta USB %s... ", udev_device_get_sysname(usbdev));
                    fflush(stdout);
                    if (write(fd, "1", 1) == 1)
                    {
                        printf("sucesso.\n");
                        fflush(stdout);
                    }
                    else
                    {
                        printf("falha.\n");
                        fflush(stdout);
                    }

                    close(fd);
                }

                free(syspath);
            }

            // usbdev é desalocado junto com blkdev
        }

        udev_device_unref(blkdev);
    }

    free(devname);
    udev_unref(ucxt);

    return EXIT_SUCCESS;
}
