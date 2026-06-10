# C Programming — Systems & Embedded Linux
 
A daily practice repository documenting my journey from C fundamentals to embedded Linux systems programming. Every folder contains a focused project or exercise, with notes on what was learned and how the code was written.
 
**Goal:** mastering low-level C, POSIX system calls, and Linux internals through hands-on projects.
 
**Background:** Mechatronics Engineer transitioning into embedded Linux software.
 
---
 
## Repository Structure
 
```
C_Programming/
├── File_IO/
│   ├── write.c         # Creates a file and writes to it using system calls
│   ├── read.c          # Reads from a file and outputs to stdout using system calls
│   └── output.txt      # Generated at runtime — not tracked by git
|__ grain_examble/
|   |__ grain.c  # a problem solving project that solves how to find the amount of grains on a chess tables if we double the grain with every increase of the chess square
|   |__ grain.h
|__ Mycp/
|   |__ mycp.c # This program combines everything from write.c and read.c into one real utility. It works exactly like the Unix cp command, built entirely from raw system calls.
|
└── README.md
```
 
---
 
## Projects
 
### 📁 File_IO — POSIX File I/O System Calls
> **Source:** *The Linux Programming Interface*, Chapter 4 — Michael Kerrisk
 
The goal of this project was to understand how Linux handles file input and output at the system call level — bypassing the standard C library (`fopen`, `fprintf`) and working directly with the kernel using `open()`, `read()`, `write()`, and `close()`.
 
#### Files
 
| File | Description |
|---|---|
| `write.c` | Creates `output.txt` and writes a string using `write()` |
| `read.c` | Opens `output.txt`, reads its content, and prints to stdout using `write()` |
 
#### Key Concepts Learned
 
- **File descriptors** — every open file is represented by a small integer (`fd`). The kernel uses this to track the file internally. `0`, `1`, `2` are always stdin, stdout, stderr.
- **`open()` flags** — flags like `O_WRONLY`, `O_CREAT`, `O_TRUNC` are bitmasked together using `|` to control exactly how a file is opened or created.
- **`read()` return value** — returns the number of bytes actually read, `0` on end-of-file, and `-1` on error. These three cases must all be handled.
- **`write()` partial writes** — `write()` may write fewer bytes than requested. The return value must be compared against the expected byte count, not just checked for `-1`. This is especially important in embedded systems when writing to hardware buffers.
- **`STDOUT_FILENO`** — stdout is just file descriptor `1`. You can write to it with `write(STDOUT_FILENO, buffer, n)` exactly like any other file, which demonstrates the UNIX principle: *everything is a file*.
- **Error handling discipline** — every system call returns `-1` on failure and sets `errno`. Using `perror()` prints a human-readable error message. In embedded systems, unchecked errors cause silent failures that are extremely hard to debug.
#### What Was Optimized vs. a Naive Approach
 
| Naive approach | Optimized approach used here |
|---|---|
| Use `fopen()` / `fprintf()` | Raw system calls `open()` / `write()` — no stdio buffering layer |
| Only check `write() == -1` | Also check for partial write: `numWritten != expected` |
| Read entire file at once | Read in a loop — handles files larger than the buffer |
| Ignore `close()` return value | Always check `close()` — it can flush buffered writes and fail |
 
#### How to Build and Run
 
**Requirements:** GCC, Linux (tested on Ubuntu 22.04+)
 
```bash
# Clone the repo
git clone https://github.com/Abdelrahman-Abdellatif/C_Programming.git
cd C_Programming/File_IO
 
# Compile
gcc -Wall -Wextra -o write write.c
gcc -Wall -Wextra -o read read.c
 
# Run
./write           # Creates output.txt with content
./read            # Reads output.txt and prints to terminal
```
 
Expected output after running `./read`:
```
Hello from system calls
```

---
#### Key Concepts Learned
 
- **File descriptors** — every open file is represented by a small integer (`fd`). The kernel uses this to track the file internally. `0`, `1`, `2` are always stdin, stdout, stderr.
- **`open()` flags** — flags like `O_WRONLY`, `O_CREAT`, `O_TRUNC` are bitmasked together using `|` to control exactly how a file is opened or created.
- **`read()` return value** — returns the number of bytes actually read, `0` on end-of-file, and `-1` on error. These three cases must all be handled.
- **`write()` partial writes** — `write()` may write fewer bytes than requested. The return value must be compared against the expected byte count, not just checked for `-1`. This is especially important in embedded systems when writing to hardware buffers.
- **`STDOUT_FILENO`** — stdout is just file descriptor `1`. You can write to it with `write(STDOUT_FILENO, buffer, n)` exactly like any other file, which demonstrates the UNIX principle: *everything is a file*.
- **Error handling discipline** — every system call returns `-1` on failure and sets `errno`. Using `perror()` prints a human-readable error message. In embedded systems, unchecked errors cause silent failures that are extremely hard to debug.
 
---
#### What Was Optimized vs. a Naive Approach
 
| Naive approach | Optimized approach used here |
|---|---|
| Only check `write() == -1` | Also check for partial write: `numWritten != expected` |
| Read entire file at once | Read in a loop — handles files larger than the buffer |
| Ignore `close()` return value | Always check `close()` — it can flush buffered writes and fail |

---
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

--- 
## Daily Log
 
| Date | Folder | Topic |
|---|---|---|
| 2025-06-9 | `File_IO` | POSIX system calls: `open`, `read`, `write`, `close` — Chapter 4 TLPI |
| 2025-06-9 | `grain_examble` | Problem Solving|
| 2025-06-10 | `Mycp` |  mycp.c  Takes two filenames as arguments, reads source in 1024-byte chunks, writes to destination |
 
---
 
## Resources
 
- *The Linux Programming Interface* — Michael Kerrisk (primary reference)
- *Mastering Embedded Linux Programming* — Frank Vasquez & Chris Simmonds
- * Linux in a Nutshell: A Desktop Quick Reference Book by Ellen Siever and Robert Love
- [Linux man pages](https://man7.org/linux/man-pages/) — always open in a second tab
---
 
## About
 
This repository is part of a structured self-study plan and Embedded_Linux Diploma which I take it With AMIT Learning to become a Junior Embedded Linux Engineer. Every project is based on real concepts used in production embedded systems (IoT devices, industrial controllers, networking hardware).
 



