# C Programming — Systems & Embedded Linux
 
A daily practice repository documenting my journey from C fundamentals to embedded Linux systems programming. Every folder contains a focused project or exercise, with notes on what was learned and how the code was written.
 
**Goal:** mastering low-level C, POSIX system calls, and Linux internals through hands-on projects.
 
**Background:** Mechatronics Engineer transitioning into embedded Linux software.
 
---
 
## Repository Structure
 
```
C_Programming/
├── File_IO/
│   ├── write.c         # Creates a file and writes to it using system calls , Creates `output.txt` and writes a string using `write()`
│   ├── read.c          # Reads from a file and outputs to stdout using system calls, Opens `output.txt`, reads its content, and prints to stdout using `write()`
│   └── output.txt      # Generated at runtime — not tracked by git
|__ grain_examble/
|   |__ grain.c  # a problem solving project that solves how to find the amount of grains on a chess tables if we double the grain with every increase of the chess square
|   |__ grain.h
|__ Mycp/
|   |__ mycp.c # This program combines everything from write.c and read.c into one real utility. It works exactly like the Unix cp command, built entirely from raw system calls.
|
|-- OpenMotion_Host_Application/
|  |
|  -- host_app.c
|
└── README.md
```
 
---
 
## Projects
 
### 📁 File_IO — POSIX File I/O System Calls
> **Source:** *The Linux Programming Interface*, Chapter 4 — Michael Kerrisk
 
The goal of this project was to understand how Linux handles file input and output at the system call level — bypassing the standard C library (`fopen`, `fprintf`) and working directly with the kernel using `open()`, `read()`, `write()`, and `close()`.
  
---

### `mycp.c` — File Copy Utility (The Full Picture)
 
This program combines everything from `write.c` and `read.c` into one real utility. It works exactly like the Unix `cp` command, built entirely from raw system calls.
 
```bash
./mycp source.txt destination.txt
```
  
--- 
## Daily Log
 
| Date | Folder | Topic |
|---|---|---|
| 2025-06-9 | `File_IO` | POSIX system calls: `open`, `read`, `write`, `close` — Chapter 4 TLPI |
| 2025-06-9 | `grain_examble` | Problem Solving|
| 2025-06-10 | `Mycp` |  mycp.c  Takes two filenames as arguments, reads source in 1024-byte chunks, writes to destination |
| 2025-06-25 | `OpenMotion_Host_Application` |  host_app.c  Reads G-code instructions from a file, converts motion commands into stepper-motor parameters, communicates with an STM32 over a Linux serial port, and waits for motion completion before sending the next command.

 
---
 
## Resources
 
- *The Linux Programming Interface* — Michael Kerrisk (primary reference)
- *Mastering Embedded Linux Programming* — Frank Vasquez & Chris Simmonds
- * Linux in a Nutshell: A Desktop Quick Reference Book by Ellen Siever and Robert Love
- [Linux man pages](https://man7.org/linux/man-pages/) — always open in a second tab
---
 
## About
 
This repository is part of a structured self-study plan and Embedded_Linux Diploma which I take it With AMIT Learning to become a Junior Embedded Linux Engineer. Every project is based on real concepts used in production embedded systems (IoT devices, industrial controllers, networking hardware).
 



