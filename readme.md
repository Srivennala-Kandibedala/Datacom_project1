# Project 1: A Broadcast Chat Application

# Makefile functionalities
- `make` will create two binary files chatserver and chatclient.
    - Both these files are created on current directory

- `make clean` will remove chatserver and chatclient binaries.

# Execution steps
- chatserver
    - usage: chatserver

- chatclient
    - usage: chatclient <servhost> <servport>

#### Chat functions
- After a client has established a connection to the server it can send a message to server and other connected clients.
- The message from a client is displayed on the server along with its address and port number.