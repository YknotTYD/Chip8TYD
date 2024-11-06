//read_file.c

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

char *read_file(char *filepath)
{
    char *kronk_buffer;
    int fildes = open(filepath, O_RDONLY);
    int length = lseek(fildes, 0, SEEK_END);

    if (fildes == -1) {
        return 0;
    }
    lseek(fildes, 0, SEEK_SET);
    kronk_buffer = malloc(length);
    read(fildes, kronk_buffer, length);
    close(fildes);
    return kronk_buffer;
}
