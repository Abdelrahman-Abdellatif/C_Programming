/* ============================================     Part 2  Read it back

This program will: 

* Opens output.txt for reading
* Reads its content into a buffer
* Writes that buffer to stdout using write()  not printf()
* Closes the file
 ==================================================*/

#include <fcntl.h>      // open(), O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <unistd.h>     // read(), write(), close()
#include <sys/stat.h>   // S_IRUSR, S_IWUSR (file permissions)
#include <stdio.h>      // only for perror() to print errors
#include <stdlib.h>     // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h> // for strlen()
#define MAX_READ    100

int main(int argc, char *argv[]){

    // declare the file descriptor variable:
    int fd;
    // will read the content into buffer:
    char buff[MAX_READ];
    ssize_t numBytes;

    fd= open("output.txt", O_RDONLY);
    if (fd == -1){
        perror("Error reading the file");
        exit(EXIT_FAILURE);
    }
    /* The read() system call reads data from the open file referred to by the descriptor fd.
    ssize_t read(int fd, void *buffer, size_t count);
    the count argument spicifes the maximum number of bytes to read
    buffer is the address of memory which the input data will be placed
    Returns number of bytes read, 0 on EOF, or -1 on error
    */
    // read the file content in a loop to untill End-of-File (0) is reached

    while((numBytes = read(fd, buff, MAX_READ)) > 0){  /* bigger than 0 means it is still reading, = 0 means EOF */
        // write the current chunk to stdout and check for partial writes/errors
        if (write(STDOUT_FILENO, buff, numBytes)!= numBytes){
            const char * err_msg = "write to stdout failed or was partial\n";
            write(STDERR_FILENO, err_msg , strlen(err_msg));
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    // check if loop terminated becouse of an  error rather than hitting EOF
    if (numBytes == -1){
        perror("Failed to read file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // close the file
    if(close(fd) == -1){
        perror("Failed to close the file");
        exit(EXIT_FAILURE);
    }
    
    return EXIT_SUCCESS;


}