## Simple file transfer
The client-server application for sending files from client to server.

##### Client.
Client run command:
>client <server_ip> <server_port> <path_to_file>

##### Server.
Server run command:
>server 
>>  [-v | --version] 
    [-h | --help] 
    [-p=\<port> | --port=\<port>]
    [-d | --debug]

if socket port is not set, will be used default port "5000".

Server default launch as daemon.
run **server -d** for run as console aplication.
