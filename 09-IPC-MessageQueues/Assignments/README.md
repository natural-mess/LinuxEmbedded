# IPC Message Queues Exercises

## Exercise 1: Send and Receive a Simple Message
**Requirements:** Write a program using POSIX message queues to send and receive a simple message.  
- Create a message queue using the `mq_open()` function.  
- Send a message containing a character string from the parent process to the child process.  
- The child process will receive the message and print its content.  
**Hints:**  
- Use the `mq_msg` structure to define the message.  
- Use `mq_send()` and `mq_receive()` functions to send and receive the message.  
- Don’t forget to close and remove the message queue after use.

---

## Exercise 2: Count Characters via POSIX Message Queue
**Requirements:** Extend the previous exercise so that the child process counts the number of characters in the received message.  
- The parent process will send a character string into the message queue.  
- The child process will receive the string, count the characters, and send the result back to the parent process via a different message queue.  
- The parent process will receive the result and print the counted number of characters.  
**Hints:**  
- Create two message queues: one for sending the character string and another for sending the character count result.  
- Use `strlen()` to count the characters in the string.

---

## Exercise 3: Communication Between Multiple Processes
**Requirements:** Write a program using POSIX message queues for communication between three processes.  
- Create a message queue for communication.  
- The parent process will send a character string into the message queue.  
- Child process 1 will receive the string, convert it to uppercase, and send it back via the message queue.  
- Child process 2 will receive the modified string from child process 1 and print it to the screen.  
**Hints:**  
- Use `mq_send()` and `mq_receive()` functions to send and receive messages.  
- Ensure processes are not blocked when reading and writing data.  
- Use `mq_close()` and `mq_unlink()` to close and remove the message queue after completion.
