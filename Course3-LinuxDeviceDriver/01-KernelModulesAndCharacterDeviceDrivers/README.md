# Kernel Modules and Character Device Drivers

## Kernel module
- A kernel module is a piece of code you can load into the Linux kernel (the core of the operating system) to add new functionality without restarting the system.
- Think of it like a USB plug-in for your computer—you plug it in to add a feature (e.g., controlling hardware) and unplug it when done.

## Character Device Driver
- A character device driver is a kernel module that creates a character device, which acts like a file (e.g., `/dev/m_cdev`) that user programs can interact with.
- User programs can write data to the device (send to the kernel) or read data from it (get from the kernel).
- It’s called a “character” device because it handles streams of data (not fixed-size blocks like a hard drive).
- Example: Writing “hello” to `/dev/m_cdev` sends it to the kernel, and reading from `/dev/m_cdev` might return “hello” back to the user.

## Why Use These?
- Kernel modules let you add custom functionality (like talking to hardware or creating devices) to Linux.
- Character device drivers provide a way for user programs (e.g., a command-line tool) to communicate with the kernel, making it easy to send or receive data.

## Building a character device driver

