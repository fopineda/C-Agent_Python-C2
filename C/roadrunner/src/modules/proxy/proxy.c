/**
 * proxy.c contains the functionality for the proxy command.
 * @file proxy.c
 */

#include <proxy.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <utils.h>
#include <fcntl.h>
#include <stdio.h>

static int set_non_blocking(int sock_fd);
static int connect_to_target(char *target_host, char *target_port);
static int open_proxy(uint16_t proxy_port);
static int listen_for_connections(int proxy_fd);
static int perform_proxy(int epoll_fd, int conn_fd, int target_fd);
static int setup_epoll(int target_fd, int conn_fd);
static pid_t proxy(char *target_host, char *target_port, uint16_t local_port);
static uint32_t deserialize_target_host(char *proxy_args, char **target_host_out);
static uint32_t deserialize_target_and_local_ports(char *proxy_args, uint32_t target_host_len, char **target_port_out, 
                                                uint16_t *local_port_out);

/**
 * @brief Helper function for putting sockets into a non-blocking state.
 * @param sock_fd (in) the socket file descriptor to modify
 * @return an integer return code with 0 being success and 1 for failure
 */
static int set_non_blocking(int sock_fd)
{
    // get the flags currently associated with the socket
    // add the non blocking flag
    // set the new flags to the socket
	return 0;
}

/**
 * @brief Connect to the proxy target.
 * @param target_host (in) the target IP or name
 * @param target_port (in) the target port number
 * @return a socket file descriptor
 */
static int connect_to_target(char *target_host, char *target_port)
{
    int sock_fd = 0;
    // setup addr for host/port
    // connect to the target with the current sock
    return sock_fd;
}

/**
 * @brief Open the local port that will serve as the proxy.
 * @param proxy_port (in) the local port number to listen on
 * @return a socket file descriptor
 */
static int open_proxy(uint16_t proxy_port)
{
    int proxy_fd;
 
    // Create socket file descriptor
    // set the socket operation to resuse addresses and ports already in use
    // bind the socket to the address
 
    return proxy_fd;
}

/**
 * @brief Listen for incoming connections on the proxy socket.
 * @param proxy_fd (in) the proxy socket file descriptor
 * @return the new connection file descriptor
 */
static int listen_for_connections(int proxy_fd)
{
    int conn_fd = 0;

    // listen for clients, this is not async to reduce complexity    
    // accept incoming connection
    // set the new connection as non-blocking

    return conn_fd;
}

/**
 * @brief Perform the actual actions of passing traffic between the two sockets.
 * @param epoll_fd (in) the epoll file descriptor
 * @param conn_fd (in) the connection file descriptor
 * @param target_fd (in) the target file descriptor
 * @return a return code indicating a normal close (0) or error (1)
 */
static int perform_proxy(int epoll_fd, int conn_fd, int target_fd)
{
    
    // loop through all events and handle them
    // check the descriptor on the event
    // read and send all data
    // dont want to close until we cant read or send from one of the sockets
        
    return 1;
}

/**
 * @brief Setup the epoll to handle events from both target and connection sockets.
 * @param target_fd (in) the target file descriptor
 * @param conn_fd (in) the connection file descriptor
 * @return the epoll file descriptor
 */
static int setup_epoll(int target_fd, int conn_fd)
{
    int epoll_fd = 0;

    // setup epoll for async
    // create our two events, one for the proxy socket and one for the conn socket
    // add the events to the epoll
    
    return epoll_fd;
}

/**
 * @brief Setup and run the proxy process.
 * @param target_host (in) the target ip address or hostname
 * @param target_pot (in) the target port number as a string
 * @param local_port (in) the local port to listen on as a short
 * @return the pid of the child process 
 */
static pid_t proxy(char *target_host, char *target_port, uint16_t local_port)
{

    pid_t ret_val;

    // fork into a new child process
    // open our proxy port
    // now that we have a proxy connection, connect to the target
    // return the child pid for use later, specifically to kill child processes

    return ret_val;
}

/**
 * @brief Deserialize the target host string from the command arguments.
 * @param proxy_args (in) the proxy arguments string
 * @param target_host_out (out) the pointer to pass out the target_host string
 *  NOTE: Calling function will be responsible for freeing target_host_out
 *  NOTE: Uses double pointer, so you can alloc memory and still maintain pointer
 * @return the size of the target_host string
 */
static uint32_t deserialize_target_host(char *proxy_args, char **target_host_out)
{
    uint32_t target_host_len;
    return target_host_len;
}

/**
 * @brief Deserialize the target and local ports from the command arguments.
 * @param proxy_args (in) the proxy arguments string
 * @param target_host_len (in) the size of the target_host_string so it can be skipped
 * @param target_port_out (out) a pointer to pass out the target port string
 *  NOTE: Calling function will be responsible for freeing target_port_out
 *  NOTE: Uses double pointer, so you can alloc memory and still maintain pointer
 * @param local_port_out (out) a pointer to pass out the local port short
 * @return the size of the target_port string
 */
static uint32_t deserialize_target_and_local_ports(char *proxy_args, uint32_t target_host_len, char **target_port_out, 
                                                uint16_t *local_port_out)
{
    uint32_t target_port_len;
    return target_port_len;
}

/**
 * @brief Invoke the proxy port command.
 * @param cmd (in) the command structure
 * @return a Response structure
 */
Response *proxy_command(Command *cmd)
{
    // original line
    // Response *rsp = NULL; 
    
    // temporary line added line as valgrind was failing/killing inegration test without allocating response   
    Response *rsp = alloc_response(0, "temporary response", strlen("temporary response")+1);

    return rsp;
}