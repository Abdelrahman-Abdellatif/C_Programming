/* This file practice chapter 4 from The Linux proramming language book, and we are using system-calls. Tasks are : 
Create a new file cllaed output.txt
write excally this text into it: "Hello form system calls\n"
close the file
if anything fails, print the error with perror() and exit
*/
#include <fcntl.h> /* open(), O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC */
#include <unistd.h>  /* read(), write(), close()*/
#include <sys/stat.h> /*S_IRUSR, S_IWUSR (file permissions)*/
#include <stdio.h>    /*only for using perror() to print errors*/
#include <stdlib.h>   /*EXIT_SUCCESS, EXIT_FAILURE*/
#include <string.h> // for strlen()

int main(int argc, char *argv[]){

    /* open() --> open exciting file or creates a new file and open it
    int open(const char *pathname, int flags) 
    */
    // declare the file descriptor variable:
    int fd;
    const char *text = "Hello from system calls\n";
    size_t text_len = strlen(text);

    // open/create the file
    /*we used O_WRONLY file access mode becouse we want only to write
    we used O_CREAT flag to create a file if it is not excited
    this flag: O_TRUNC will remove all the content of the file and write it as clean start
    premission: S_IRUSR | S_IWUSR
    */

    fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening/creating file");
        exit(EXIT_FAILURE);
    }

    /*write()--> system call is used to write data to an open file
    ssize_t write(int fd, void *buffer, size_t count)
    fd --> is the file descriptor opened
    *buffer : is the location of the wanted text to be writtin in the file
    count --> how many bytes need to be written
    write() returns -1 on failure
    */
    // write data to file
    ssize_t numWritten = write(fd, text, strlen(text));
    // check for a complete systemcall failure
    if(numWritten == -1){
        perror("Error writing to file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // check for partial systemcall failure

    if((size_t)numWritten != text_len){
        const char *err_msg = "Critical: Partial write occured\n";
        
        // using write system call to send this error message to the stderr stream (2)
        write(STDERR_FILENO, err_msg, strlen(err_msg));
        close(fd);
        exit(EXIT_FAILURE);
    }

    // close the file
    if (close(fd)==-1){
        perror("Error closeing the file");
        exit(EXIT_FAILURE);
    }


    return EXIT_SUCCESS;
}