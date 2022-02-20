# Math Server
### Question
Implement a fully concurrent application with a TCP server acting as a dummy ”math” server and client programs allowing concurrent connection and message transfer.  
The server should be a multi-process server that will fork a process for every new client it receives and it should be able to handle addition, multiplication, subtraction, and division operations on two integer operands.
The protocol between the client and server is as follows.  
• The client connects to the server, and then asks the user for input. The user enters a simple arithmatic expression string (e.g., ”1 + 2”, ”5 - 6”, ”3 * 4”, ”8/2”). The user’s input is sent to the server via the connected socket.  
• The server reads the user’s input from the client socket, evaluates the expression, and sends the result back to the client.  
• The client should display the server’s reply to the user, and prompt the user for the next input, until the user terminates the client program with Ctrl+C.  
