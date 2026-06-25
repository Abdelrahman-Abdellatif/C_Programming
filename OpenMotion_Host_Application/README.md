# OpenMotion Host Application

| File         | Purpose                                                                                                                                                                                                              |
| ------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `host_app.c` | Reads G-code instructions from a file, converts motion commands into stepper-motor parameters, communicates with an STM32 over a Linux serial port, and waits for motion completion before sending the next command. |

---

# `host_app.c` — Linux G-Code Motion Controller Host

This program acts as the PC-side controller for the OpenMotion project.

The application reads a G-code file line-by-line, extracts supported movement commands, converts distances and feedrates into motor control parameters, packs those values into a custom binary protocol, and transmits them to an STM32 microcontroller through a UART serial connection.

Unlike simply streaming commands, the host waits for an acknowledgement and a movement-complete notification before continuing. This guarantees synchronization between the Linux application and the physical machine.

The overall communication flow is:

```text
G-code File
     |
     v
Linux Host Application
     |
     v
Serial Port (115200 baud)
     |
     v
STM32 Firmware
     |
     v
Stepper Motor
```

---

# Libraries Used

The program relies on several standard Linux and C libraries.

### `stdio.h`

Provides standard input/output functions.

Used for:

```c
printf()
fprintf()
fopen()
fgets()
fclose()
```

These functions handle console output and text file processing.

---

### `stdlib.h`

Provides utility functions and common macros.

Used for:

```c
EXIT_SUCCESS
EXIT_FAILURE
```

These macros provide standard operating-system return codes.

---

### `string.h`

Provides string manipulation functions.

Used for:

```c
strcmp()
strncmp()
strchr()
strcspn()
strlen()
strerror()
```

These functions are used for command-line argument checking, G-code parsing, and error message formatting.

---

### `unistd.h`

Provides access to POSIX system calls.

Used for:

```c
read()
write()
close()
```

These functions communicate directly with the Linux kernel.

---

### `fcntl.h`

Provides file-control constants and file opening flags.

Used for:

```c
open()
O_RDWR
O_NOCTTY
O_SYNC
```

These flags define how the serial device is opened.

---

### `termios.h`

Provides Linux terminal and serial-port configuration structures.

Used for:

```c
struct termios
tcgetattr()
tcsetattr()
cfsetospeed()
cfsetispeed()
tcflush()
```

This library is responsible for configuring the UART connection into raw binary mode.

---

### `stdint.h`

Provides fixed-width integer types.

Used for:

```c
uint8_t
uint32_t
```

These guarantee exact byte sizes regardless of platform.

This is important when building binary packets.

---

### `errno.h`

Provides access to Linux system error codes.

Used for:

```c
errno
strerror(errno)
```

This allows human-readable reporting of operating-system failures.

---

# Protocol Constants

The host and STM32 communicate using predefined byte values.

```c
CMD_MOVE              0x01
STATUS_OK             0x55
STATUS_MOVE_COMPLETE  0x77
ERR_INVALID_COMMAND   0x99
ERR_INVALID_PARAMETER 0xAA
```

These values form the protocol language between both systems.

For example:

```text
Host ---> CMD_MOVE
STM32 --> STATUS_OK
STM32 --> STATUS_MOVE_COMPLETE
```

This allows both devices to understand each other using simple binary messages.

---

# System Calls Used

A major objective of this project was understanding Linux system calls.

Unlike standard library functions, system calls cross the boundary between user space and kernel space.

---

## `open()`

```c
int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
```

Opens the serial device.

Returns a file descriptor which acts as a handle to the device.

### Flags Used

#### `O_RDWR`

Open for both reading and writing.

The application must send commands and receive responses.

#### `O_NOCTTY`

Prevents the serial device from becoming the controlling terminal.

Without this flag, terminal-related signals may interfere with communication.

#### `O_SYNC`

Forces writes to occur synchronously.

Data is pushed directly toward the device rather than sitting in software buffers.

---

## `read()`

```c
read(fd, &response, 1);
```

Reads bytes from the serial port.

The process blocks until data arrives.

This behavior is essential because the host must wait for the STM32 acknowledgement before continuing.

The program uses `read()` twice:

1. Wait for command acknowledgement
2. Wait for movement completion

---

## `write()`

```c
write(fd, packet, 10);
```

Sends the 10-byte command packet to the STM32.

The return value is checked:

```c
if(write(fd, packet, 10) != 10)
```

This ensures all bytes were transmitted successfully.

A successful write does not necessarily mean the STM32 processed the command; it only means Linux accepted the data for transmission.

---

## `close()`

```c
close(fd);
```

Releases the file descriptor.

Without closing descriptors, resources remain allocated inside the kernel.

---

# Function Breakdown

---

## `print_help()`

```c
void print_help(const char *prog_name)
```

Displays usage instructions.

Example:

```bash
./host_app /dev/ttyACM0 file.gcode
```

This function improves usability by explaining command syntax and supported G-code commands.

---

## `configure_serial_port()`

```c
int configure_serial_port(int fd)
```

Configures the serial port for raw UART communication.

This is one of the most important functions in the program.

Linux serial ports are heavily processed by default because they were originally designed for human terminals.

Those features must be disabled when sending binary packets.

---

### `tcgetattr()`

```c
tcgetattr(fd, &tty);
```

Retrieves current driver settings from the kernel.

The existing configuration is copied into a local structure.

This prevents accidentally destroying unrelated driver settings.

---

### Baud Rate Configuration

```c
cfsetospeed(&tty, B115200);
cfsetispeed(&tty, B115200);
```

Sets both transmit and receive speeds.

115200 baud means:

```text
115200 bits/second
```

Both the Linux host and STM32 must use the same baud rate.

---

### UART Format

The program configures:

```text
8 Data Bits
No Parity
1 Stop Bit
```

Often called:

```text
8N1
```

This is the most common UART configuration in embedded systems.

---

### Raw Mode

Several terminal-processing features are disabled:

```c
ICANON
ECHO
ISIG
IXON
IXOFF
OPOST
```

This prevents Linux from modifying bytes before transmission.

Without raw mode:

```text
0x0A
```

could be automatically converted into:

```text
0x0D 0x0A
```

which would corrupt the packet.

---

### `tcsetattr()`

```c
tcsetattr(fd, TCSANOW, &tty);
```

Pushes the modified configuration back into the kernel.

`TCSANOW` means the changes take effect immediately.

---

## `send_and_wait_move()`

```c
int send_and_wait_move(...)
```

Responsible for the complete command-execution cycle.

This function:

1. Builds a packet
2. Sends packet
3. Waits for ACK
4. Waits for completion

---

### Packet Construction

The protocol frame is exactly 10 bytes.

```text
Byte 0 : Command ID
Byte 1 : Direction

Byte 2-5 : Steps
Byte 6-9 : Delay
```

Layout:

```text
+------+-----------+
| Byte | Purpose   |
+------+-----------+
| 0    | CMD_MOVE  |
| 1    | Direction |
| 2-5  | Steps     |
| 6-9  | Delay     |
+------+-----------+
```

---

### Bit Shifting

The STM32 expects 32-bit integers split into four bytes.

Example:

```c
packet[2] = (steps >> 24) & 0xFF;
```

This extracts the most significant byte.

Example:

```text
0x12345678
```

becomes:

```text
0x12
0x34
0x56
0x78
```

This ensures both systems interpret the same numerical value.

---

### `tcflush()`

```c
tcflush(fd, TCIOFLUSH);
```

Clears old receive and transmit data from the serial buffers.

This prevents stale bytes from previous operations from being mistaken as responses.

---

### ACK Synchronization

After transmission:

```c
read(fd, &response, 1);
```

The STM32 immediately responds:

```text
STATUS_OK
```

This confirms the packet was received and accepted.

---

### Completion Synchronization

The host then performs a second blocking read.

```c
read(fd, &response, 1);
```

This may block for several seconds.

The STM32 only replies after the motor physically finishes moving.

Response:

```text
STATUS_MOVE_COMPLETE
```

This guarantees the host never gets ahead of the machine.

---

# Main Program Flow

---

## 1. Validate Command-Line Arguments

The program supports:

```bash
./host_app port file.gcode
```

and

```bash
./host_app --help
```

Invalid arguments immediately terminate execution.

---

## 2. Open G-code File

```c
fopen(filename,"r");
```

The file is opened in read-only mode.

Failure immediately aborts execution.

---

## 3. Open Serial Device

```c
open(...)
```

Creates the communication channel with the STM32.

Linux treats serial devices as files.

For example:

```text
/dev/ttyACM0
```

behaves like a file descriptor.

---

## 4. Configure UART

```c
configure_serial_port(fd);
```

Applies raw communication settings.

---

## 5. Read File Line-by-Line

```c
while(fgets(...))
```

Processes one instruction at a time.

This keeps memory usage low regardless of file size.

---

## 6. Parse G-code

Supported command:

```text
G1
```

Supported parameters:

```text
X
F
```

Example:

```text
G1 X25 F600
```

Meaning:

```text
Move 25 mm at 600 mm/min
```

---

## 7. Motion Planning Calculations

Distance conversion:

```c
steps = mm × STEPS_PER_MM
```

Given:

```text
STEPS_PER_MM = 80
```

Example:

```text
10 mm
```

becomes:

```text
800 steps
```

---

### Feedrate Conversion

Formula:

```text
Steps/Second =
(Feedrate / 60)
× STEPS_PER_MM
```

Then:

```text
Delay =
1,000,000 / StepsPerSecond
```

The result becomes the timer delay sent to the STM32.

---

## 8. Send Motion Command

The host packages:

```text
Direction
Steps
Delay
```

into a binary packet.

The packet is transmitted using `write()`.

---

## 9. Wait for Motion Completion

The host blocks until:

```text
STATUS_MOVE_COMPLETE
```

is received.

This creates deterministic machine synchronization.

---

## 10. Cleanup

At end-of-file:

```c
close(fd);
fclose(gcode_file);
```

All resources are released correctly.

---

# What Was Learned From This Project

* Linux serial ports are configured using the Termios API.
* Devices in Linux are treated as files and accessed through file descriptors.
* System calls such as `open()`, `read()`, `write()`, and `close()` provide direct communication with kernel-managed devices.
* UART communication requires matching baud rate and framing settings on both ends.
* Raw mode is necessary when transmitting binary protocols.
* Fixed-width integers (`uint8_t`, `uint32_t`) prevent packet-format inconsistencies.
* Binary protocols often require splitting multi-byte integers using bit shifting.
* Blocking I/O can be used to synchronize software with physical machine movement.
* G-code instructions can be converted into motor-control parameters through mathematical motion-planning calculations.
* A command/acknowledge/complete protocol provides reliable communication between a Linux host and an embedded microcontroller.
