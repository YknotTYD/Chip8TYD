//read_file.c

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int read_file(char **kronk_buffer, char *filepath)
{
    int fildes;
    int lenght;
    int readsult;

    fildes = open(filepath, O_RDONLY);
    if (fildes == -1) {
        return 0;
    }
    lenght = lseek(fildes, 0, SEEK_END);
    lseek(fildes, 0, SEEK_SET);
    *kronk_buffer = malloc(lenght + 1);
    if (kronk_buffer == 0) {
        close(fildes);
        return 0;
    }
    readsult = read(fildes, *kronk_buffer, lenght);
    if (readsult == 0) {
        free(kronk_buffer);
    }
    (*kronk_buffer)[lenght] = 0;
    close(fildes);
    return lenght;
}
