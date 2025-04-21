# IPC Pipe Exercises

## Exercise 1: Using Pipe for Communication Between Parent and Child Processes
**Requirements:** Write a program that uses a pipe for communication between two processes.  
- Create a pipe and fork a child process.  
- The parent process will send a text string into the pipe.  
- The child process will read the text string from the pipe and print it to the screen.  
**Hint:** Use the `pipe()` function to create a pipe, `fork()` to create a child process, and `read()`/`write()` for communication between processes.

To run the program:
```
cd Ex1
```
```
make
```
```
./pipeParent2Child
```

---

## Exercise 2: Using Multiple Pipes for Communication Between Multiple Processes
**Requirements:** Extend the previous exercise to use multiple pipes for three processes.  
- Create two pipes: one for communication from the parent process to child process 1, and another from child process 1 to child process 2.  
- The parent process sends a message to child process 1.  
- Child process 1 receives the message, modifies it (e.g., appends a string to the end), and sends it to child process 2.  
- Child process 2 receives the message and prints it to the screen.  
**Hint:** Use multiple pipes and ensure processes are not blocked when reading and writing data.

To run the program:
```
cd Ex2
```
```
make
```
```
./multiPipe
```

---

## Exercise 3: Count Characters via Pipe
**Requirements:** Write a program that uses a pipe to count the number of characters in a string.  
- Create a pipe and fork a child process.  
- The parent process will send a character string into the pipe.  
- The child process will read the string from the pipe, count the number of characters, and print the result.  
**Hint:** Use the `strlen()` function to count the characters in the string read from the pipe.

To run the program:
```
cd Ex3
```
```
make
```
```
./countCharPipe
```
