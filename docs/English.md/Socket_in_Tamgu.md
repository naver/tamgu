# Socket in Tamgu

The `socket` type handles HTML socket interactions between a client and a server.

## 1. Methods

### Client Side

1. `close()`: Close the socket.
2. `close(clientid)`: Close the communication with `clientid`.
3. `connect(string hostname, int port)`: Connect a client to a specific host on a specific port.
4. `createserver(int port, int nbclients)`: Create a server on the local host with a specific port.
5. `createserver(string hostname, int port, int nbclients)`: Create a server on a host with a specific port.
6. `get()`: Get one character from a socket.
7. `get(int clientid)`: Get one character from a socket with `clientid`.
8. `getframe(string name)`: Return a frame object remote handle of name `name`.
9. `getfunction(string name)`: Return a function remote handle of name `name`.
10. `gethostname()`: Return the current host name. The socket does not need to be activated to get this information.
11. `read()`: Read a Tamgu object on the socket.
12. `read(clientid)`: Read a Tamgu object on the socket with `clientid`.
13. `receive(int nb)`: Read `nb` characters from a socket.
14. `receive(int clientid, int nb)`: Read `nb` characters from the socket with `clientid`.
15. `run(int client, string stopstring)`: Put the server in run mode. The server can now accept Remote Method Invocation (RMI) mode.
16. `send(int clientid, string s)`: Write a simple string on the socket with `clientid`.
17. `send(string s)`: Write a simple string on the socket.

### Server Side

18. `settimeout(int i)`: Set the timeout in seconds for both writing and reading on the socket. Use this instruction to avoid blocking on a read or on a write. A value of -1 cancels the timeout.
19. `wait()`: The server waits for a client to connect. It returns the client identifier, which will be used to communicate with the client.
20. `write(clientid, o1, o2...)`: Write Tamgu objects on the socket with `clientid`.
21. `write(o1, o2...)`: Write Tamgu objects on the socket.

## Example: Server Side

```cpp
// Server side
int clientid;
socket s; // We create a socket
string name = s.gethostname();
println("Local server:", name);
// We create our server on the socket 2020, with at most 5 connections...
s.createserver(2020, 5);
// We wait for a client connection
while (true) {
    // We can accept up to 5 connections...
    clientid = s.wait();
    // We read a message from the client, it should be done in a thread to handle more connections.
    string message = s.read(clientid);
    message += " and returned";
    // We write a message to the client
    s.write(clientid);
    // We close the connection
    s.close(clientid);
}
// We kill the server
s.close();
```

## Example: Client Side

```cpp
// Client side
socket s; // We create a socket
string name = s.gethostname();
println("Local server:", name);
// We create our server on the socket 2020
```