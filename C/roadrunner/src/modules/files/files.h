/**
 * files.h contains all of the files definitions and exports.
 * @file files.h
 */

#ifndef FILES_H
#define FILES_H

#if defined(FILES_DEBUG) || defined(RR_DEBUG_ALL)
#include <stdio.h>
#define FILES_DEBUG_PRINT(msg, ...) printf(msg, ##__VA_ARGS__);
#else
#define FILES_DEBUG_PRINT(msg, ...) 
#endif

#include <commands.h>
#define READ_BUFFER_INCREMENT 1024
#define DOWNLOAD_CMD_STR  "download"
#define UPLOAD_CMD_STR  "upload"



uint32_t read_file(char *filename, char **contents_out);
uint32_t write_file(char *filename, char *contents, uint32_t contents_size);
Response *download_file_command(Command *cmd);
Response *upload_file_command(Command *cmd);

#endif