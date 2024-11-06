/*
** EPITECH PROJECT, 2024
** NOP
** File description:
** void;
*/

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

char *read_file(char *filename)
{
    static const int buffer_size = 16000;
    int fildes;
    char buffer[buffer_size];
    int length;
    int total_length=0;
    char *content;

    fildes=open(filename, O_RDONLY);

    content=malloc(lseek(fildes, 0, SEEK_END));
    lseek(fildes, 0, SEEK_SET);
    
    length=read(fildes, buffer, buffer_size);

    for (; length; length = read(fildes, buffer, buffer_size)) {
        for (int i=0; i<length; i++) {
            content[total_length+i]=buffer[i];
        }
        total_length+=length;
    }
        
    close(fildes);

    return content;
}

/*int main(void)
{
    char *text;

    text=read_file("read_file.c");
    printf("%s", text);
    
    return 0;
}*/
