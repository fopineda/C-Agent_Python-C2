/**
 * commands.h contains the definitions and exported functions utilized by other modules.
 * @file commands.h
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#if defined(COMMANDS_DEBUG) || defined(RR_DEBUG_ALL)
#include <stdio.h>
#define COMMANDS_DEBUG_PRINT(msg, ...) printf(msg, ##__VA_ARGS__);
#else
#define COMMANDS_DEBUG_PRINT(msg, ...) 
#endif
#include <string.h>
#include <inttypes.h>

// min size of any command stream
#define COMMAND_STREAM_MIN_SIZE sizeof(uint32_t) * 3

typedef struct command_struct
{
    char* cmd;
    uint32_t cmd_len;
    char* args;
    uint32_t  args_len;

} Command;

typedef struct response_struct
{
    int32_t ret_code;
    char *msg;
    uint32_t msg_len;
} Response;

Command *deserialize_command(uint32_t msg_size, char *msg_stream);
Command *alloc_command(char *cmd, uint32_t cmd_len, char *args, uint32_t args_len);
void free_command(Command *cmd);

uint32_t serialize_response(Response *rsp, char **stream_out);
Response *alloc_response(int32_t ret_code, char *msg, uint32_t msg_len);
void free_response(Response *rsp);

#endif