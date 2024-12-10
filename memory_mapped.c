#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"
#include "error_functions.h"

/*void errExit(const char *format){
    perror(format);
    exit(EXIT_FAILURE);
}*/

int main(int argc, char *argv[])
{
    char *mmaped_ptr;
    int fd;
    struct stat sb;

    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        exit(EXIT_FAILURE);
    }
    
    fd = open(argv[1], O_RDWR);
    if (fd == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &sb) == -1){
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    mmaped_ptr = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmaped_ptr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    char *readbuffer = malloc(6 * sizeof(char));
    if(pid == 0){ //Child process
        //printf("Child process (pid=%d); mmap address: %p\n", getpid(), mmaped_ptr);
        memcpy(mmaped_ptr, "01234", 5);
        msync(mmaped_ptr, sb.st_size, MS_SYNC);
        memcpy(readbuffer, mmaped_ptr + 4096, 5);
        readbuffer[5] = '\0'; //Null-terminate the string
        printf("Child process (pid=%d); read from mmap_ptr [4096]: %s\n", getpid(), readbuffer);
        

    } else { //Parent process
        //wait(NULL); //Wait for the child process to complete
        //printf("Parent process (pid=%d); mmap address: %p\n", getpid(), mmaped_ptr);
        memcpy(mmaped_ptr + 4096, "56789", 5);
        msync(mmaped_ptr, sb.st_size, MS_SYNC);
        memcpy(readbuffer, mmaped_ptr, 5);
        readbuffer[5] = '\0'; //Null-terminate the string
        printf("Parent process (pid=%d); read from mmap_ptr [0]: %s\n", getpid(), readbuffer);
    }
    free(readbuffer);
    
    // Unmap the memory before exiting
    if (munmap(mmaped_ptr, sb.st_size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    return 0;


    /*if (write(STDOUT_FILENO, mmaped_ptr, sb.st_size) != sb.st_size)
        fatal("partial/failed write");
    exit(EXIT_SUCCESS);*/
}