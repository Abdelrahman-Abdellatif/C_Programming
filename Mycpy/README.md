| `mycp.c` | Takes two filenames as arguments, reads source in 1024-byte chunks, writes to destination |
 
---
 
### `mycp.c` — File Copy Utility (The Full Picture)
 
This program combines everything from `write.c` and `read.c` into one real utility. It works exactly like the Unix `cp` command, built entirely from raw system calls.
 
```bash
./mycp source.txt destination.txt
```
 
---
 
#### How it works — step by step
 
**1. Argument validation with `switch(argc)`**
 
`argc` is the count of command-line arguments. When you run `./mycp source.txt destination.txt`, `argc` equals 3 — the program name counts as argument 0.
 
Instead of a simple `if (argc != 3)`, a `switch` statement was used to give the user a specific, helpful error message for each wrong case:
 
```c
switch (argc) {
    case 1: // no arguments at all
    case 2: // source given but destination missing
    case 3: // correct — continue
    default: // too many arguments
}
```
 
Error messages are written using `write(STDERR_FILENO, ...)` instead of `printf` — because `STDERR_FILENO` (file descriptor 2) is the correct channel for error output in Unix. This means error messages still appear even if the user redirects stdout to a file.
 
**2. Opening two file descriptors**
 
```c
src_fd = open(argv[1], O_RDONLY);
des_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
```
 
Two separate file descriptors are needed — one for reading, one for writing. They are just integers the kernel gives back to represent the open files.
 
The destination file permissions `S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH` translate to `644` in Unix notation — owner can read and write, group and others can only read. This matches what a normal copied file should look like on a real system.
 
**3. The copy loop — the heart of the program**
 
```c
while ((numBytes = read(src_fd, buffer, BUF_SIZE)) > 0) {
    if (write(des_fd, buffer, numBytes) != numBytes) {
        // partial write — handle error
        goto cleanup;
    }
}
```
 
`read()` fills the buffer with up to 1024 bytes at a time and returns how many bytes it actually read. The loop continues as long as that number is greater than zero. When `read()` hits the end of the file it returns `0`, which stops the loop naturally.
 
The buffer size of `1024` bytes (`BUF_SIZE`) is deliberate — in embedded systems you never read an entire file into memory at once because the device may not have enough RAM. Reading in fixed chunks works on files of any size.
 
The `write()` return value is compared against `numBytes`, not just checked for `-1`. This is because `write()` can succeed but write fewer bytes than requested — called a **partial write**. This happens when writing to hardware buffers, network sockets, or storage under memory pressure. Catching partial writes is standard practice in embedded Linux development.
 
**4. Cleanup with `goto`**
 
```c
goto cleanup;
 
// ...
 
cleanup:
    close(src_fd);
    close(des_fd);
    exit(EXIT_FAILURE);
```
 
Without `goto`, the same close-both-files block would need to be written three or four times across different error paths. If the cleanup logic ever changes, every copy must be updated — and one will always be missed.
 
`goto` is the standard pattern for cleanup in C and is used extensively in the Linux kernel source code. It is not bad practice here — it is the correct tool for this exact situation. One cleanup block, jumped to from any error path.
 
---
 
#### What was learned from this project
 
- A real program needs to handle not just success but every failure path — missing arguments, failed opens, failed reads, failed writes, and failed closes
- `STDERR_FILENO` is the correct place to write error messages, not stdout
- `goto` for cleanup is a legitimate and professional C pattern, not a shortcut
- Reading in fixed-size chunks instead of loading the whole file is the embedded systems mindset — always think about memory constraints
- Partial writes are a real problem, not a theoretical one — always compare the return value of `write()` against the expected byte count
