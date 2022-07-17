/**
 * commands.c contains all of the functionality for serializeing and deserializing commands and responses.
 * @file commands.c
 */

#include <commands.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <utils.h>


int deserialize_short(int i_offset, char* p_src_buff, unsigned short *p_out){
    memcpy(p_out, p_src_buff[i_offset], sizeof(unsigned short));
    *p_out = ntohs(*p_out);
    return sizeof(unsigned short);
}

int deserialize_byte_array(int i_offset, char* p_src_buff, char* p_out, int i_length){
    memcpy(p_out, &p_src_buff[i_offset], i_length);

    return (sizeof(char) * i_length);
}


/**
 * @brief Deserialize a message stream of bytes into a Command structure.
 * @param msg_size (in) the total message stream size
 * @param msg_stream (in) a pointer to the message stream
 * @return a pointer to a Command structure or NULL on error
 */
Command *deserialize_command(uint32_t  msg_size, char *msg_stream)
{  

    // printf("\n  dc: msg_size: %d", msg_size); // TODO: Delte later
    // printf("\n  dc: sizeof(msg_stream): %d", sizeof(msg_stream)); // TODO: Delte later
    // Initializing variables to be used for storing values
    uint32_t mem_offset = 0;
    uint32_t i_msg_stream_size = 0;
    uint32_t i_command_length = 0;
    char* p_command_array = NULL;
    uint32_t i_args_length = 0;
    char* p_args_array = NULL;

    
    // total message size or msg_stream size 
    memcpy(&i_msg_stream_size, msg_stream+mem_offset, sizeof(uint32_t));
    uint32_t i_msg_stream_size_ntohl = ntohl(i_msg_stream_size);
    mem_offset = mem_offset + 4;
    // printf("\n  dc: i_msg_stream_size: %d", i_msg_stream_size); // TODO: Delte later
    // printf("\n  dc: i_msg_stream_size_ntohl: %d", i_msg_stream_size_ntohl); // what you want // TODO: Delte later


    // command length
    memcpy(&i_command_length, msg_stream+mem_offset, sizeof(uint32_t));
    uint32_t i_command_length_ntohl = ntohl(i_command_length);
    mem_offset = mem_offset + 4;
    // printf("\n\n  dc: i_command_length: %d", i_command_length); // TODO: Delete later
    // printf("\n  dc: i_command_length_ntohl: %d", i_command_length_ntohl);  // what you want // TODO: Delete later

    // command
    p_command_array = (char*) calloc(i_command_length_ntohl, sizeof(char));
    memcpy(p_command_array, msg_stream+mem_offset,i_command_length_ntohl);
    mem_offset = mem_offset + i_command_length_ntohl;
    // printf("\n  dc: p_command_array: %s", p_command_array); // what you want // TODO: Delete later
    // printf("\n  dc: strlen(p_command_array): %d", strlen(p_command_array)); // TODO: Delete later
    // printf("\n  dc: sizeof(p_command_array) %d", sizeof(p_command_array)); // TODO: Delete later


    // args length
    memcpy(&i_args_length, msg_stream+mem_offset, sizeof(uint32_t));
    uint32_t i_args_length_ntohl = ntohl(i_args_length);
    mem_offset = mem_offset + 4;
    // printf("\n\n  dc: i_args_length: %d", i_args_length); // TODO: Delete later
    // printf("\n  dc: i_args_length_ntohl: %d", i_args_length_ntohl); // TODO: Delete later



    // args
    p_args_array = (char*) calloc(i_args_length_ntohl, sizeof(char));
    memcpy(p_args_array, msg_stream+mem_offset,i_args_length_ntohl);
    mem_offset = mem_offset + i_args_length_ntohl;
    // printf("\n  dc: p_args_array: %s", p_args_array); // what you want // TODO: Delete laterv
    // printf("\n  dc: strlen(p_args_array): %d", strlen(&p_args_array)); // TODO: Delete later
    // printf("\n  dc: sizeof(p_args_array) %d\n", sizeof(p_args_array));// TODO: Delete later
    
    
    // calculate the size of the bytes gives for message size, len of command, len of args which all are unint32 (4 bytes)
    int response_unint32_total = sizeof(uint32_t) * 3; 
    // printf("\nresponse_unint32_total: %d", response_unint32_total); // TODO: Delete later
    // printf("\ncomparing msg_size with: %d", response_unint32_total +  i_command_length_ntohl + i_args_length_ntohl); // TODO: Delete later
    // if the msg_size does not equal the total number of int bytes plus the bytes allocated for the command and args then return null
    if (msg_size != (response_unint32_total +  i_command_length_ntohl + i_args_length_ntohl)){
        return NULL;
    }
    
    Command *deserialize_cmd = alloc_command(p_command_array, i_command_length_ntohl, p_args_array, i_args_length_ntohl);
    
    free(p_command_array);
    free(p_args_array);
    
    return deserialize_cmd;
}

/**
 * @brief Allocate the memory and store data in a new Command structure.
 * @param cmd (in) pointer to the cmd bytes
 * @param cmd_len (in) the size of the cmd
 * @param args (in) pointer to the args bytes
 * @param args_len (in) the size of the args
 * @return a pointer to an empty Command structure or NULL on error
 */
Command *alloc_command(char *cmd, uint32_t cmd_len, char *args, uint32_t args_len)
{
    Command *new_cmd = calloc(1, sizeof(Command));
    // if we get a null then there are memory issues and we need to bail
    if(new_cmd == NULL)
    {
        return new_cmd;
    }
    new_cmd->cmd = calloc(cmd_len, sizeof(char));
    memcpy(new_cmd->cmd, cmd, cmd_len);
    new_cmd->cmd_len = cmd_len;
    new_cmd->args = calloc(args_len, sizeof(char));
    memcpy(new_cmd->args, args, args_len);
    new_cmd->args_len = args_len;
    return new_cmd;
}

/**
 * @brief Free a Command structure and its components.
 * @param cmd a pointer to a Command structure
 */
void free_command(Command *cmd)
{
    if(cmd != NULL)
    {
        checkfree(cmd->cmd)         
        checkfree(cmd->args)
        free(cmd);
    }
}



/**
 * @brief Serialize a Response structure into a byte stream.
 * @param rsp (in) a pointer to a Response structure
 * @param stream_out (out) a NULL char pointer where the stream will be passed back out
 * @return the total size of the stream
 */
uint32_t serialize_response(Response *rsp, char **stream_out)
{
    // initialize variables               
    uint32_t msg_len = rsp->msg_len;
    uint32_t total_size = msg_len + 12;
    *stream_out  = (char*) calloc(total_size, sizeof(char));

    // total_message_size
    uint32_t converted_total_size = htonl(total_size); 
    memcpy(*stream_out, &converted_total_size, sizeof(uint32_t));

    // ret_code
    uint32_t converted_ret_code = htonl(rsp->ret_code);   
    memcpy(*stream_out+4, &converted_ret_code, sizeof(uint32_t));

    // message_length
    uint32_t converted_msg_len = htonl(msg_len); 
    memcpy(*stream_out+8, &converted_msg_len, sizeof(uint32_t));

    // message
    memcpy(*stream_out+12, rsp->msg, msg_len);


    // printf("\nstream_out: %s", *stream_out); // TODO: Delete later
    

    // printf("\n total_size: %d", total_size);
    uint32_t total_size_ntohl = ntohl(total_size);
    // printf("\n total_size_ntohl: %d", total_size_ntohl);
    return total_size;
}

/**
 * @brief Allocate memory for a Response structure.
 * @param ret_code (in) the integer return code
 * @param msg (in) the message stream
 * @param msg_len (in) the size of the message stream
 * @return a pointer to a Response structure or NULL on error
 */
Response *alloc_response(int32_t ret_code, char* msg, uint32_t msg_len)
{
    Response *rsp;
    rsp = calloc(1, sizeof(Response));

    // if we get a null then there are memory issues and we need to bail
    if (rsp == NULL)
    {
        return rsp;
    }

    rsp->ret_code = ret_code;
    rsp->msg_len = msg_len;

    // need to include the null terminator
    rsp->msg = calloc(msg_len, sizeof(char));
    memcpy(rsp->msg, msg, msg_len);

    return rsp;
}

/**
 * @brief free a Response structure and its components.
 * @param rsp pointer to a Response structure
 */
void free_response(Response *rsp)
{
    
    // check to see if the Response structure is NULL
    if(rsp != NULL)
    {
        // free the message stream
        checkfree(rsp->msg)
        // free the whole structure  
        free(rsp);
    }
}

