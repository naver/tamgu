# Socket

The `Socket` class handles HTML socket interactions between a client and a server.

## Methods

### Client Side

1. `close()`: closes the socket.
2. `close(clientid)`: closes the communication with the specified `clientid`.
3. `connect(string hostname, int port)`: connects a client to a specific host on a specific port.
4. `createServer(int port, int nbclients)`: creates a server on the local host with a specific port.
5. `createServer(string hostname, int port, int nbclients)`: creates a server on a host with a specific port.
6. `get()`: gets one character from a socket.
7. `get(int clientid)`: gets one character from a socket with the specified `clientid`.
8. `getFrame(string name)`: returns a frame object remote handle of the specified `name`.
9. `getFunction(string name)`: returns a function remote handle of the specified `name`.
10. `getHostname()`: returns the current host name. The socket does not need to be activated to get this information.
11. `read()`: reads a Tamgu object on the socket.
12. `read(int clientid)`: reads a Tamgu object on the socket with the specified `clientid`.
13. `receive(int nb)`: reads `nb` characters from a socket.
14. `receive(int clientid, int nb)`: reads `nb` characters from the socket with the specified `clientid`.
15. `run(int client, string stopstring)`: puts the server in run mode. The server can now accept Remote Method Invocation (RMI) mode.
16. `send(int clientid, string s)`: writes a simple string on the socket with the specified `clientid`.
17. `send(string s)`: writes a simple string on the socket.

### Server Side

18. `setTimeout(int i)`: sets the timeout in seconds for both writing and reading on the socket. Use this instruction to avoid blocking on a read or write. A value of -1 cancels the timeout.
19. `wait()`: the server waits for a client to connect. It returns the client identifier, which will be used to communicate with the client.
20. `write(clientid, o1, o2...)`: writes Tamgu objects on the socket with the specified `clientid`.
21. `write(o1, o2...)`: writes Tamgu objects on the socket.

## Example: Server Side

```python
// Server side
int clientid;
Socket s; // create a socket
string name = s.getHostname();
println("Local server:", name);
// Create a server on socket 2020, with at most 5 connections
s.createServer(2020, 5);
// Wait for a client connection
while (true) {
  // Accept up to 5 connections
  clientid = s.wait();
  // Read a message from the client (should be done in a thread to handle more connections)
  string message = s.read(clientid);
  message += " and returned";
  // Write a message to the client
  s.write(clientid);
  // Close the connection
  s.close(clientid);
}
// Kill the server
s.close();
```

## Example: Client Side

```python
// Client side
Socket s; // create a socket
string name = s.getHostname();
println("Local server:", name);
// Create a server on socket 2020
s.connect(name, 2020);
// Write a message to the server
string message = "Hello";
s.write(message);
// Read a message from the server
message = s.read();
println(message);
// Close the connection
s.close();
```