# Chat Application

This document provides a detailed overview of a multi-threaded TCP-based chat application. The application allows multiple machines to connect, exchange messages, and manage connections via a command-line interface.

## Table of Contents

- [Chat Application](#chat-application)
  - [Table of Contents](#table-of-contents)
  - [Design and Structure of the Program](#design-and-structure-of-the-program)
    - [Overall Design](#overall-design)
    - [Program Structure](#program-structure)
    - [File Design and Structure](#file-design-and-structure)
      - [`main.c`](#mainc)
      - [`chatDrv/chat.c`](#chatdrvchatc)
      - [`server/server_socket.c`](#serverserver_socketc)
      - [`client/client_socket.c`](#clientclient_socketc)
      - [`ui/ui.c`](#uiuic)
      - [`include/chat.h`](#includechath)
      - [`Makefile`](#makefile)
  - [Detailed Code Explanation](#detailed-code-explanation)
    - [`main.c`](#mainc-1)
    - [`chatDrv/chat.c`](#chatdrvchatc-1)
    - [`server/server_socket.c`](#serverserver_socketc-1)
    - [`client/client_socket.c`](#clientclient_socketc-1)
    - [`ui/ui.c`](#uiuic-1)
    - [`include/chat.h`](#includechath-1)
    - [`Makefile`](#makefile-1)
  - [How to Run the Program](#how-to-run-the-program)
    - [Prerequisites](#prerequisites)
    - [Compilation](#compilation)
    - [Running the Application](#running-the-application)
    - [Cleanup](#cleanup)

## Design and Structure of the Program

### Overall Design
The chat application is a peer-to-peer system where each instance can act as both a server (accepting incoming connections) and a client (initiating outgoing connections). It uses TCP sockets for reliable communication and POSIX threads (`pthread`) for concurrency, enabling simultaneous command handling and message exchange. The program is modular, separating concerns into distinct files for maintainability and clarity.

- **Key Features**:
  - Command-line interface with commands: `help`, `myip`, `myport`, `connect`, `list`, `terminate`, `send`, `exit`.
  - Multi-threaded server to handle multiple client connections.
  - Handshake protocol to exchange listening ports between peers.
  - Thread-safe connection management using a mutex-protected client list.

- **Concurrency**:
  - Main thread: Runs the UI command handler.
  - Server thread: Accepts incoming connections.
  - Per-connection threads: Handle communication with each peer.

- **Data Structures**:
  - `ClientInfo`: Stores socket FD, IP address, and listening port for each connection.
  - `ClientData`: Passes socket FD and server flag to chat threads.
  - Global `clients[]` array and `client_count`: Track active connections, protected by `client_mutex`.

### Program Structure
The project is organized into the following directories and files:

- **`main.c`**: Entry point, initializes the server, and starts the UI thread.
- **`chatDrv/chat.c`**: Core chat logic, including thread management and message send/receive functions.
- **`server/server_socket.c`**: Server-side socket setup and connection handling.
- **`client/client_socket.c`**: Client-side socket setup for initiating connections.
- **`ui/ui.c`**: User interface and command handler.
- **`include/chat.h`**: Header file with shared definitions, structs, and prototypes.
- **`Makefile`**: Build script to compile the program.

### File Design and Structure

#### `main.c`
- **Purpose**: Program entry point, initializes the server socket and UI.
- **Structure**: Parses command-line arguments, starts the server, and spawns a UI thread in an infinite loop.

#### `chatDrv/chat.c`
- **Purpose**: Manages the chat thread lifecycle and provides message send/receive utilities.
- **Structure**: Defines global client list and mutex, implements the chat thread, and includes socket I/O functions.

#### `server/server_socket.c`
- **Purpose**: Sets up the server socket and handles incoming connections.
- **Structure**: Initializes the server socket, starts a connection thread, and spawns per-client threads with handshake logic.

#### `client/client_socket.c`
- **Purpose**: Initiates outgoing connections to other peers.
- **Structure**: Connects to a server, performs a handshake, and starts a chat thread.

#### `ui/ui.c`
- **Purpose**: Provides the command-line interface and command processing.
- **Structure**: Implements command handlers and message display logic, using `getifaddrs()` for IP retrieval.

#### `include/chat.h`
- **Purpose**: Centralizes definitions and prototypes.
- **Structure**: Includes headers, macros, structs, and function declarations.

#### `Makefile`
- **Purpose**: Automates compilation.
- **Structure**: Defines build rules and dependencies.

---

## Detailed Code Explanation

### `main.c`

Initializes the server socket with a user-specified port and spawns a detached UI thread.

- `pthread_detach(ui_thread)`: Makes the ui_thread independent, meaning its resources are automatically freed when it terminates, and `main()` doesn’t need to `pthread_join()` it. This is crucial because `main()` doesn’t wait for the UI thread to finish—it just keeps running.
- Keeps the `main()` thread alive indefinitely, preventing the program from exiting prematurely. `sleep(1)` reduces CPU usage by pausing the thread for 1 second per iteration (instead of a busy loop).

### `chatDrv/chat.c`

Manages client connections and provides thread-safe message sending/receiving functions.

### `server/server_socket.c`

Sets up the server socket and spawns a thread to handle incoming connections.

### `client/client_socket.c`

Initiates a connection to another peer and performs a handshake.

### `ui/ui.c`

Handles user commands like `help`, `myip`, `connect`, etc.

### `include/chat.h`

Defines shared structs, macros, and function prototypes.

### `Makefile`

Compiles the program into the `chat` executable.

---

## How to Run the Program
### Prerequisites
- A Unix-like environment (e.g., Linux, macOS).
- GCC compiler installed.
- `make` utility for building the project.
### Compilation
1. Navigate to the project directory in your terminal.
2. Run the following command to compile the program

```bash
make
```
This will create an executable named `chat`.

### Running the Application
1. Start the chat application on a machine by specifying a port number:
```bash
./chat <port number>
```

For example
```bash
./chat 4322
```

2. The application will display available commands and wait for input.
3. To connect to another instance, use the `connect` command:

```bash
connect <destination IP> <port number>
```

Example:
```bash
connect 192.168.1.100 4322
```
4. Use commands like `list`, `send`, `terminate`, or `exit` as needed.

### Cleanup
To remove the compiled binary:
```bash
make clean
```
