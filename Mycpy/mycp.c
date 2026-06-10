/* mycp.c is a simple implementation of the copy file data from source to destination file:
 It should work as follows:
    1. The program should take two command-line arguments: the source file and the destination file
    2. The program should open the source file for reading and the destination file for writing
    3. If the destination file already exists, it should be overwritten, if it does not exist, it should be created
    4. The program should read the contents of the source file and write it to the destination file
    5. The program should handle any errors that may occur during the file operations, such as file not found, permission denied, etc.
    6. The program should close both files after the copying is done
*/
#include <fcntl.h> /* open(), O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC*/
#include <unistd.h> /* read(), write(), close(), STDOUT_FILENO, STDERR_FILENO */
#include <sys/stat.h> /*File permissions macros: S_IRUSR, S_IWUSR, S_IRGRP, S_IROTH*/
#include <stdio.h> /* perror() to handle system errors*/
#include <stdlib.h> /* exit() to handle program termination, EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h> /* strlen() if i want to wrtie a custom error messages*/

#define BUF_SIZE 1024

int main (int argc, char *argv[]){
    /*==============================================================================*/
    // 1. Declare variables
    //    we will need TWO file descriptors (one for source, one for dest)
    int src_fd, des_fd;
    //    we will need a buffer array of BUF_SIZE.
    char buffer[BUF_SIZE];
    //    we will need a variable to track bytes read/written.
    ssize_t numBytes;  /* ssize_t is:  1) an archticture depenedend Their size changes depending on the computer running the code
        2) it is sifgned [can hold positive and negative values] 
        3) it represents the maximum size of any object in memory.
    */

    /*==============================================================================*/

    // 2. Validate Command-Line Arguments
    //    The user must provide exactly 3 items: the program name, source, and dest.
    switch (argc){

        case 1: { /*no argument provided:*/
            const char *err_msg ="Usage: ./mycp <source_file> <destination_file>\n";
            write(STDERR_FILENO, err_msg, strlen(err_msg));
            exit(EXIT_FAILURE);
        }
        case 2: { /*only source file provided */
            const char *err_msg1 = "Error: Missing destination file.\nUsage: ./mycp <source_file> <destination_file>\n";
            write(STDERR_FILENO, err_msg1, strlen(err_msg1));
            exit(EXIT_FAILURE);
        }
        case 3: /* correct number of arguments */
            break; /* continue with the program*/
        
        default: {
            const char *err_msg2 = "Error: Too many arguments provided.\nUsage: ./mycp <source_file> <destination_file>\n";
            write(STDERR_FILENO, err_msg2, strlen(err_msg2));
            exit(EXIT_FAILURE);
        }
    }

/*==============================================================================*/
    // 3. Open the Source File
    //    Open argv[1] for reading only. Handle errors.
    src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1){
        perror ("Error open the source file");
        exit(EXIT_FAILURE); 
    }

    /*==============================================================================*/
    // 4. Open/Create the Destination File
    //    Open argv[2] for writing.
    des_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(des_fd == -1){
        perror ("Error opening the Destination file");
        goto cleanup;
    }
    /*==============================================================================*/
    // 5. The Copy Loop (The Heart of the Program)
    /*    //    Using the loop pattern we practiced, read chunks from the source file descriptor 
    //    and write them directly to the destination file descriptor.
    */
    while ((numBytes = read(src_fd, buffer,BUF_SIZE)) > 0){
        if(write(des_fd, buffer, numBytes) != numBytes){
            const char *err_msg3 = "Write to destination file failed or was partial\n";
            write(STDERR_FILENO, err_msg3, strlen(err_msg3));
            
            goto cleanup;
        }
    }
    /*==============================================================================*/
    // 6. Cleanup & Error Check
    //    Check if the loop ended because of a read failure.
    if (numBytes == -1){
        perror("Failed to read from source and write to des file");
        goto cleanup;
    }
    //    Close BOTH file descriptors securely.

    if (close(src_fd) == -1){
        perror("Failed to close the source file");
        exit(EXIT_FAILURE);
    }
    if (close(des_fd) == -1){
        perror("Failed to close the des file");
        exit(EXIT_FAILURE);
    }

    /*==============================================================================*/
    // 7. Success Exit
    write(STDOUT_FILENO, "File copied successfully.\n", 25);
    return EXIT_SUCCESS;

    cleanup:
        close(src_fd);
        close(des_fd);
        exit(EXIT_FAILURE);


}
