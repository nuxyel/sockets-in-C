# Sockets in C

This repository contains a collection of network programming scripts written in C, focusing on the use of raw sockets. These projects were created as part of a deep-dive study into low-level networking and security concepts, inspired by courses from Solyd.

The goal here is to understand the fundamentals of TCP connections, data transfer, and remote command execution from the ground up, without the abstractions provided by higher-level languages.

## Compilation

These scripts are written in C and must be compiled before execution. You can use GCC (GNU Compiler Collection), which is standard on most Linux systems.

To compile each script, use the following command structure:

```bash
gcc -o <executable_name> <source_file.c>
```

*Example:*

```bash
gcc -o server_tcp server_tcp.c
```

-----

## Scripts Overview

Here is a breakdown of each script, its purpose, how to compile it, and how to use it.

### 1\. `server_tcp.c`

A basic, single-client TCP chat server. It waits for one client to connect and then enters a loop to exchange messages with that client. This server is the direct counterpart to `client_tcp.c`.

**Features:**

  * Listens on port `8080`.
  * Accepts a single client connection.
  * Engages in a turn-based chat, receiving a message before sending one.

**Compilation:**

```bash
gcc -o server_tcp server_tcp.c
```

**Usage:**
Run the compiled executable in your terminal. It will wait for a client to connect.

```bash
./server_tcp
```

-----

### 2\. `client_tcp.c`

A simple, interactive TCP chat client designed to connect to `server_tcp.c`. It establishes a connection and enters a loop to send a message and receive a response from the server.

**Features:**

  * Connects to `127.0.0.1` on port `8080`.
  * Allows the user to send messages from the terminal.
  * Prints responses received from the server.

**Compilation:**

```bash
gcc -o client_tcp client_tcp.c
```

**Usage:**
After starting `server_tcp`, run the client in another terminal to connect to it.

```bash
./client_tcp
```

-----

### 3\. `server_ssh.c`

**Disclaimer:** This is a **Simple Remote Shell Server**, not a true SSH server. It does not implement the SSH protocol. Its name is used to reflect its function as a tool for remote shell access.

This server is more advanced. It listens for connections, authenticates clients with a hardcoded password, and then executes any command sent by the client on the server machine.

**Features:**

  * **Remote Command Execution**: Uses `popen()` to execute commands sent by the client.
  * **Basic Authentication**: Protects access with a simple, hardcoded password.
  * **Multi-Client Handling**: Can accept and handle multiple clients sequentially (one after the other).

**Compilation:**

```bash
gcc -o remote_shell server_ssh.c
```

**Usage:**

1.  Run the compiled server on a machine. It will listen on port `8080`.
    ```bash
    ./remote_shell
    ```
2.  From another machine (or terminal), connect using `netcat` or `client_tcp`. First, you must send the password (`senha123`) followed by a newline.
    ```bash
    # Using netcat
    nc 127.0.0.1 8080
    # After connecting, type:
    senha123
    # You should now be authenticated and can send commands.
    ls -la
    whoami
    ```

### **Security Warning**

The code in this repository, especially `server_ssh.c`, is for **educational purposes ONLY**. It is intentionally simple and contains serious security vulnerabilities.

  * **Command Injection**: The use of `popen()` with un-sanitized client input makes it highly vulnerable to command injection.
  * **Plaintext Authentication**: The password is sent in cleartext over the network and is hardcoded in the source.
    **Do NOT use this code in a production environment.**

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

by _r3n4n_
