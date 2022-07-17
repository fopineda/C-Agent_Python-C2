/**
 * integration.c contains all of the networking and integration tests for the agent.
 * @file integration.c
 */

#include <integration.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utils.h>

static uint32_t serialize_command(Command *cmd, char **out_stream);
static Response *recv_response(int conn_fd);
static uint32_t send_command(int conn_fd, char *cmd_stream, uint32_t cmd_len);
static void print_bytes(char *byte_str, uint32_t len);
static int send_cmd_recv_rsp(int conn_fd, Command *cmd);
static uint32_t generate_upload_args(char *file_name, uint32_t file_name_length, char *content, uint32_t content_length, 
                                    char **buffer_out);
static uint32_t generate_proxy_args(char *target_host, uint32_t target_host_len, char *target_port, uint32_t target_port_len,
                                    uint16_t local_port, char **buffer_out);
static int run_tests(int conn_fd);

/**
 * @brief Helper function for building command streams for testing.
 * @param cmd the command string
 * @param cmd_len the command length you wish to send
 * @param args the args strings
 * @param args_len the args length you wish to send
 * @param out_stream 
 * @return the size of the command stream
 */
static uint32_t serialize_command(Command *cmd, char **out_stream)
{
    uint32_t stream_size = sizeof(uint32_t) * 3 + cmd->cmd_len + cmd->args_len;
    uint32_t net_stream_size = htonl(stream_size);
    uint32_t net_cmd_size = htonl(cmd->cmd_len);
    uint32_t net_args_size = htonl(cmd->args_len);
    uint32_t mem_offset = 0;
    *out_stream = calloc(stream_size, sizeof(char));
    memcpy(*out_stream, &net_stream_size, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*out_stream + mem_offset, &net_cmd_size, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*out_stream + mem_offset , cmd->cmd, cmd->cmd_len);
    mem_offset += cmd->cmd_len;
    memcpy(*out_stream + mem_offset, &net_args_size, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*out_stream + mem_offset, cmd->args, cmd->args_len);
    return stream_size;
}

/**
 * @brief Receieve a response from the agent.
 * @param conn_fd the connection file descriptor
 * @return a pointer to a Response struct
 */
static Response *recv_response(int conn_fd)
{
    int32_t rsp_code;
    uint32_t valread, total_read, read_size, msg_size, net_msg_size, rsp_msg_size, buffer_ptr;
    char *buffer, *rsp_msg;
    Response *rsp = NULL;
    total_read = 0;
    read_size = sizeof(uint32_t);
    buffer = calloc(read_size, sizeof(char));
    printf("**C2** receiving response\n");
    // read the total message size
    do
    {
        valread = read(conn_fd, buffer + total_read, read_size - total_read);
        if (valread == -1)
        {
            perror("**C2** error: failed to read from socket\n");
            goto clean_up;
        }
        if(valread == 0)
        {
            perror("**C2** error: unexpected EOF on socket read\n");
            goto clean_up;
        }
        total_read += valread;
        printf("**C2** total_read: %d\n", total_read);
    } while (total_read < read_size);
    memcpy(&net_msg_size, buffer, read_size);
    msg_size = ntohl(net_msg_size);
    printf("**C2** message size: %d\n", msg_size);
    if(msg_size < MIN_RESPONSE_SIZE)
    {
        perror("**C2** error: response is less than min response size\n");
        goto clean_up;
    }
    buffer = realloc(buffer, sizeof(char) * msg_size);
    memset(buffer, 0, msg_size);
    read_size = msg_size - sizeof(uint32_t);
    total_read = 0;
    // now read the response
    do
    {
        valread = read(conn_fd, buffer + total_read, read_size - total_read);
        if (valread == -1)
        {
            perror("**C2** error: failed to read from socket\n");
            goto clean_up;
        }
        if(valread == 0)
        {
            perror("**C2** error: unexpected EOF on socket read\n");
            goto clean_up;
        }
        total_read += valread;
        printf("**C2** total_read: %d\n", total_read);
    } while (total_read < read_size);
    // parse the response buffer into its parts
    buffer_ptr = 0;
    memcpy(&rsp_code, buffer, sizeof(uint32_t));
    rsp_code = ntohl(rsp_code);
    buffer_ptr += sizeof(uint32_t);
    memcpy(&rsp_msg_size, buffer + buffer_ptr, sizeof(uint32_t));
    rsp_msg_size = ntohl(rsp_msg_size);
    buffer_ptr += sizeof(uint32_t);
    // if our message is greater than size zero alloc and memcpy, otherwise alloc a 1 byte buffer with null byte
    if (rsp_msg_size > 0)
    {
        rsp_msg = calloc(rsp_msg_size, sizeof(char));
        memcpy(rsp_msg, buffer + buffer_ptr, rsp_msg_size);
    }
    else
    {
        rsp_msg = calloc(1, sizeof(char));
    }
    rsp = alloc_response(rsp_code, rsp_msg, rsp_msg_size);
    checkfree(rsp_msg);
clean_up:
    checkfree(buffer);
    return rsp;
}

/**
 * @brief Send a command to the agent.
 * @param conn_fd the connection file descriptor
 * @param cmd_stream the serialized command
 * @param cmd_len the length of the serialized command
 * @return the number of bytes sent
 */
static uint32_t send_command(int conn_fd, char *cmd_stream, uint32_t cmd_len)
{
    uint32_t sent = 0;
    uint32_t total_sent = 0;
    printf("**C2** sending %u bytes\n", cmd_len);
    do
    {
        sent = send(conn_fd, cmd_stream + total_sent, cmd_len - total_sent, 0);
        if (sent == -1)
        {
            perror("**C2** error: failed to write to socket.\n");
            break;
        }
        total_sent += sent;

    } while (total_sent < cmd_len);
    return total_sent;
}

/**
 * @brief A helper function for printing out the byte streams that may contain null bytes.
 * @param byte_str the byte string to print
 * @param len the length of the byte string
 */
static void print_bytes(char *byte_str, uint32_t len)
{
    int i;
    printf("**C2** byte representation: ");
    for (i = 0; i < len; i++)
    {
        printf("0x%x", byte_str[i]);
        if (i < len - 1)
        {
            printf(" ");
        }
    }
    printf("\n**C2** char representation: ");
    for (i = 0; i < len; i++)
    {
        printf("%c", byte_str[i]);
    }
}

/**
 * @brief Send and receive a response from the agent.
 * @param conn_fd the connection file descriptor
 * @param cmd the pointer to the command to send
 * @param cmd_len the size of the command
 * @param args_len the size of the arguments
 * @return returns the return code passed back from the agent (0 = good, 1 = bad)
 */
static int send_cmd_recv_rsp(int conn_fd, Command *cmd)
{
    Response * rsp = NULL;
    char * cmd_stream = NULL;
    uint32_t cmd_size;
    int ret_code;
    printf("**C2** sending command = cmd: %s args: %s\n", cmd->cmd, cmd->args);
    cmd_size = serialize_command(cmd, &cmd_stream);
    send_command(conn_fd, cmd_stream, cmd_size);
    printf("**C2** command sent\n");
    checkfree(cmd_stream);
    rsp = recv_response(conn_fd);
    if (rsp == NULL)
    {
        perror("**C2** error: response is null\n");
        return 1;
    }
    printf("**C2** response = ret_code: %d msg:\n", rsp->ret_code);
    print_bytes(rsp->msg, rsp->msg_len);
    printf("\n");
    ret_code = rsp->ret_code;
    free_response(rsp);
    return ret_code;
}

/**
 * @brief Generate the upload arguments that need to be specially serialized.
 * @param file_name the name (or path and name) of the file to be uploaded
 * @param file_name_length the length of the file name string
 * @param content the contents of the file
 * @param content_length the length of the content to send
 * @param buffer_out a pointer to pass out the serialized arguments
 * @return returns the size of the buffer_out that was generated
 */
static uint32_t generate_upload_args(char *file_name, uint32_t file_name_length, char *content, uint32_t content_length, 
                                    char **buffer_out)
{
    uint32_t net_file_name_length, net_content_length, mem_offset, args_size;
    net_file_name_length = htonl(file_name_length);
    net_content_length = htonl(content_length);
    args_size = sizeof(uint32_t) * 2 + file_name_length + content_length;
    *buffer_out = calloc(args_size, sizeof(char));
    memcpy(*buffer_out, &net_file_name_length, sizeof(uint32_t));
    mem_offset = sizeof(uint32_t);
    memcpy(*buffer_out + mem_offset, file_name, file_name_length);
    mem_offset += file_name_length;
    memcpy(*buffer_out + mem_offset, &net_content_length, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*buffer_out + mem_offset, content, content_length);
    return args_size;
}

/**
 * @brief Generate the proxy arguments that need to be specially serialized.
 * @param target_host the target IP address or hostname
 * @param target_host_len the length of the target_host
 * @param target_port the target Port as a string
 * @param target_port_len the length of the target_port
 * @param local_port the local port for the proxy to listen too as a unsigned short
 * @param buffer_out a pointer to pass out the serialized arguments
 * @return returns the size of the buffer_out that was generated
 */
static uint32_t generate_proxy_args(char *target_host, uint32_t target_host_len, char *target_port, uint32_t target_port_len,
                                    uint16_t local_port, char **buffer_out)
{
    uint32_t net_target_host_length, net_target_port_length, mem_offset, args_size;
    net_target_host_length = htonl(target_host_len);
    net_target_port_length = htonl(target_port_len);
    args_size = sizeof(uint32_t) * 2 + target_host_len + target_port_len + sizeof(uint16_t);
    *buffer_out = calloc(args_size, sizeof(char));
    memcpy(*buffer_out, &net_target_host_length, sizeof(uint32_t));
    mem_offset = sizeof(uint32_t);
    memcpy(*buffer_out + mem_offset, target_host, target_host_len);
    mem_offset += target_host_len;
    memcpy(*buffer_out + mem_offset, &net_target_port_length, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*buffer_out + mem_offset, target_port, target_port_len);
    mem_offset += target_port_len;
    local_port = htons(local_port);
    memcpy(*buffer_out + mem_offset, &local_port, sizeof(uint16_t));
    return args_size;
}

/**
 * @brief Send each known command to the agent.
 * @param conn_fd the connection file descriptor
 * @return an integer return code (0 = good, 1 = bad)
 */
static int run_tests(int conn_fd)
{
    printf("**C2** running tests\n");
    Command *cmd;
    Response *rsp;
    char *upload_file, *file_contents, *target_host, *target_port;
    char *upload_args, *proxy_args;
    int ret_code = 0;
    uint16_t local_port;
    uint32_t args_size;

    upload_args = NULL;
    proxy_args = NULL;
    
    // get checkin response
    rsp = recv_response(conn_fd);
    if (rsp == NULL)
    {
        perror("**C2** error: checkin response is null\n");
        goto cleanup_bail;
    }
    printf("**C2** response = ret_code: %d msg: %s\n", rsp->ret_code, rsp->msg);
    free_response(rsp);
    rsp = NULL;
    // test sleep command
    cmd = alloc_command("sleep", 6, "1", 2);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       goto cleanup_bail;
    }
    free_command(cmd);
    // test download command
    cmd = alloc_command("download", 9, "./test/data/test_file.txt", 26);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       goto cleanup_bail;
    }
    free_command(cmd);
    // test upload command
    upload_file = "./test/data/test_file.txt";
    file_contents = "I AM A TEST FILE";
    upload_args = NULL;
    args_size = generate_upload_args(upload_file, 26, file_contents, 16, &upload_args);
    cmd = alloc_command("upload", 7, upload_args, args_size);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       goto cleanup_bail;
    }
    free_command(cmd);
    checkfree(upload_args);
    // test hostname command
    cmd = alloc_command("hostname", 9, "", 1);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       goto cleanup_bail;
    }
    free_command(cmd);
    // test proclist command
    cmd = alloc_command("proclist", 9, "", 1);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       //goto cleanup_bail;
    }
    free_command(cmd);
    // test netstat command
    cmd = alloc_command("netstat", 8, "", 1);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       goto cleanup_bail;
    }
    // test invoke command
    cmd = alloc_command("invoke", 7, "ls", 3);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       goto cleanup_bail;
    }
    free_command(cmd);
    //test proxy command
    target_host = "localhost";
    target_port = "8888";
    local_port = 9999;
    proxy_args = NULL;
    args_size = generate_proxy_args(target_host, 10, target_port, 5, local_port, &proxy_args);
    cmd = alloc_command("proxy", 6, proxy_args, args_size);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       //goto cleanup_bail;
    }
    free_command(cmd);
    checkfree(proxy_args);
    //test shutdown command
    cmd = alloc_command("shutdown", 9, "", 1);
    ret_code = send_cmd_recv_rsp(conn_fd, cmd);
    if (ret_code)
    {
       goto cleanup_bail;
    }
    free_command(cmd);
    return ret_code;
cleanup_bail:
    checkfree(upload_args);
    checkfree(proxy_args);
    free_command(cmd);
    return ret_code;
}

/**
 * @brief The main function for the integration tests.
 * @param argc the number of arguments
 * @param argv an array of arguments
 * @return an integer return code
 */
int main(int argc, char const* argv[])
{
    int server_fd, conn_fd, ret_code;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    struct timeval timeout;      
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("**C2** error getting socket file descriptor\n");
        exit(1);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("**C2** error with setsockopt\n");
        exit(1);
    }
    if (setsockopt (server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                sizeof timeout) < 0)
    {
        perror("**C2** error with setsockopt recv timeout\n");
        exit(1);
    }

    if (setsockopt (server_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                sizeof timeout) < 0)
    {
        perror("**C2** error with setsockopt send timeout\n");
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(INTEGRATION_PORT);
 
    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
        perror("**C2** error with socket bind\n");
        exit(1);
    }
    if (listen(server_fd, 3) < 0) {
        perror("**C2** error listening for new connections\n");
        exit(1);
    }
    if ((conn_fd = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
        perror("**C2** error accepting connection\n");
        exit(1);
    }
    printf("**C2** receieved connection.\n");
    ret_code = run_tests(conn_fd);
    close(conn_fd);
    close(server_fd);
    exit(ret_code);
}
