# Linux File System Exercises

## Exercise 1
Write a program to open a file and use O_APPEND, after that use seek to go to the beginning of the file and write some data to it.

Since we open text.txt as O_APPEND flag, this flag will force every write operation to append data at the end of the file, no matter what. Even if we use lseek(fd, 0, SEEK_SET) to move the cursor to the beginning of the file, the cursor will move back to the end of the file automatically.

To run the exercise:

```
cd Ex1
```
```
make
```
```
./file_seek
```

---

## Exercise 3
Write a program that takes 3 command-line arguments, structured as follows:
```
$ ./example_program filename num-bytes [r/w] "Hello"
```
Where:
- example_program: Name of the executable file
- filename: Name of the file
- num-bytes: Number of bytes to read/write [r/w]:
    - r -> Perform reading from filename and print to the screen
    - w -> Perform writing to filename
- "Hello": Any content to read/write into filename

To run the file:
- `cd Ex3`
- run `make`
- Usage: `./cmdLineArg <filename> <numBytes> <command> [data]`
i.e:
- `./cmdLineArg fileToRead.txt 2 r`
- `./cmdLineArg fileToWrite.txt 5 w hello`

---

## Exercise 4
Create an arbitrary file and write a segment of data into it. Display on the screen the following information: file type, file name, last modification time, and size. - Use the struct stat.

To execute:
- Run `make`
- Run `./fileStat`
