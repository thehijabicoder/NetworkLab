### 1. [q2_qb_server.c](https://github.com/thehijabicoder/NetworkLab/blob/master/ass3/q2/q2_ab_server.c)  
###    [q2_ab_client.c](https://github.com/thehijabicoder/NetworkLab/blob/master/ass3/q2/q2_ab_client.c)  
#### Question :  
Write a client/server program with the socket interface. The client program may send out the words to the server once the user presses "Enter" and the server will respond to these words with any meaningless terms. However, the server will close the connection once it receives the word "Bye". Also, once someone keys in "GivemeyourVideo", the server will immediately send out a 50 MB data file with message size of 500 bytes.  
&nbsp;  
a) Calculate and record the data transmission rate every 0.1 second for a 50 MB data transmission with message size of 500 bytes.    
b) Plot to display the results   
- [plot.py](https://github.com/thehijabicoder/NetworkLab/blob/master/ass3/plot.py) is used to plot the graphs.


### 2. [q2_c_server.c](https://github.com/thehijabicoder/NetworkLab/blob/master/ass3/q2/q2_c_server.c)  
###    [q2_c_client.c](https://github.com/thehijabicoder/NetworkLab/blob/master/ass3/q2/q2_c_client.c)  
#### Question :  
  c) Create a socket that implements stop and wait ARQ protocol and analyze the RTT for varying message sizes.  
