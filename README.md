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
 
## Daily Log
 
| Date | Folder | Topic |
|---|---|---|
| 2025-06-10 | `File_IO` | POSIX system calls: `open`, `read`, `write`, `close` — Chapter 4 TLPI |
| 2025-06-10 | `grain_examble` | Problem Solving|
 
---
 
## Resources
 
- *The Linux Programming Interface* — Michael Kerrisk (primary reference)
- *Mastering Embedded Linux Programming* — Frank Vasquez & Chris Simmonds
- * Linux in a Nutshell: A Desktop Quick Reference Book by Ellen Siever and Robert Love
- [Linux man pages](https://man7.org/linux/man-pages/) — always open in a second tab
---
 
## About
 
This repository is part of a structured self-study plan and Embedded_Linux Diploma which I take it With AMIT Learning to become a Junior Embedded Linux Engineer. Every project is based on real concepts used in production embedded systems (IoT devices, industrial controllers, networking hardware).
 



