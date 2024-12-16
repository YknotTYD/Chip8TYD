//read_file.c

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

char *read_file(char *filepath)
{
    char *kronk_buffer;
    int fildes;
    int length;
    int readsult;

    fildes = open(filepath, O_RDONLY);
    if (fildes == -1) {
        return 0;
    }
    length = lseek(fildes, 0, SEEK_END);
    lseek(fildes, 0, SEEK_SET);
    kronk_buffer = malloc(length);
    if (kronk_buffer == 0) {
        close(fildes);
        return 0;
    }
    readsult = read(fildes, kronk_buffer, length);
    if (readsult == 0) {
        free(kronk_buffer);
    }
    close(fildes);
    return kronk_buffer;
}
