/**
 * files.c contains all of the functionality for the download and upload commands.
 * @file files.c
 */

#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <string.h>
#include <arpa/inet.h>

static uint32_t deserialize_upload_file_path(char *upload_args, char **file_path_out);
static uint32_t deserialize_upload_file_contents(char *upload_args, uint32_t file_path_len, char **contents_out);

/**
 * @brief opens file specified by parameter passed, puts the contents in contents_out and returns the size.
 * @param filename filename to open
 * @param contents_out a NULL pointer that will contain the contents of the file or NULL on error
 * @return an integer representing the number of bytes read
 */ 
uint32_t read_file(char *filename, char **contents_out)
{
    FILE *h_file;
    char *resized_buffer;
    int ch;
    uint32_t buffer_size = READ_BUFFER_INCREMENT;
    uint32_t read_counter = 0;
    if(*contents_out != NULL)
    {
        FILES_DEBUG_PRINT("error: contents_out was not NULL!\n")
        return read_counter;
    }
    // open the file handle in read bytes mode
    h_file = fopen(filename, "rb");
    // check to see if we got a valid handle back
    if (h_file == NULL)
    {
        FILES_DEBUG_PRINT("error: failed to read file\n")
        return read_counter;
    }
    // allocate our initial read buffer
    *contents_out = calloc(buffer_size, sizeof(char));
    do {
        // grab a character from the file
        ch = fgetc(h_file);
        if(EOF != ch)
        {
            // add the character to the buffer at the current counter
            (*contents_out)[read_counter] = (char) ch;
            read_counter += 1;
        }
        // check to see if we are running out of room in the buffer
        if (read_counter >= buffer_size - 1)
        {
            // increase the buffer size by the specified increment
            buffer_size += READ_BUFFER_INCREMENT;
            // reallocate a new buffer
            resized_buffer = realloc(*contents_out, buffer_size);
            // check to see if the realloc worked
            if(resized_buffer == NULL)
            {
                // we have memory issues so debug print and bail
                FILES_DEBUG_PRINT("error: realloc failed to allocate more memory\n")
                *contents_out = NULL;
                read_counter = 0;
                return read_counter;
            }
            // set contents buffer to the potentially new pointer
            *contents_out = resized_buffer;
        }
    } while (ch != EOF);
    if (feof(h_file))
    {
        FILES_DEBUG_PRINT("reached end of stream on write\n")
    }
    if (ferror(h_file))
    {
        FILES_DEBUG_PRINT("error: failed to read whole file\n")
    }
    fclose(h_file);
    // should return null if broken, otherwise pointer to file data.
    return read_counter;
}

/**
 * @brief Write the contents of a stream to a specified file.
 * @param filename the name of the file to write to
 * @param contents a pointer to the contents that will be written
 * @param contents_size the size of the contents
 * @return returns the number of bytes written to the file
 */
uint32_t write_file(char *filename, char *contents, uint32_t contents_size)
{
    uint32_t write_counter = 0;

    // if the contents are NULL, don't even try to create the file with fopen
    if (contents == NULL || strlen(contents) == 0){
        return 0;
    } 

    // open the file handle in write bytes mode
    FILE* fp;
    fp = fopen(filename, "wb");

    // if  (aguement given; check tests) fp is NULL return counter to be zero, otherwise write to file with returing file size
    if (fp == NULL){
        return 0;
    }   
    else{

        write_counter = fwrite(contents, 1, contents_size, fp);
        fclose(fp);
        return write_counter;
    }
       
}

/**
 * @brief Download the file specified in the Command args.
 * @param cmd the download command with arguments
 * @return a Response where the message is the file byte stream
 */
Response *download_file_command(Command *cmd)
{

    Response *rsp = NULL;

    // creates a contents buffer ptr to store the contents being give out from read_file
    char *contents = NULL;

    // uses read_file to get size (sz) of the file and write to the contents buffer
    int sz = read_file(cmd->args, &contents);
    // printf("\n  sz: %d", sz);


    // if the read_file gave back a zero (i.e file was not able to be read or file size is 0), then return error otherwise send response with file contents
    if (contents == NULL){
        char *error_msg = "error downloading file";
        
        //set rsp->msg to the error mesdsage along with other requirments
        rsp = alloc_response(1, error_msg, strlen(error_msg));
        
    }
    else{
        //printf("%s\n", contents ); // TODO: Delete later

        //set rsp->msg to the fille contents along with other requirments
        rsp = alloc_response(0, contents, sz);    
    }

    // since contents may be null check first if the ptrhas something, then free it other do nothing
    checkfree(contents);
    // Send contents
    return rsp;

}

/**
 * @brief deserialize the file path and file path length from upload arguments.
 * @param upload_args the full upload arguments passed in the command structure
 * @param file_path_out a NULL pointer that will contain the file path on success or NULL on error
 * @return the length of the file path
 */
static uint32_t deserialize_upload_file_path(char *upload_args, char **file_path_out)
{
    int32_t i_file_path_size = 0;
    int upload_args_offset = 0;
    char* p_file_path_array = NULL;


    // file path size
    // get the file path size first so you can them get the correct amount of bytes of the actual file path
    memcpy(&i_file_path_size, upload_args + upload_args_offset, sizeof(uint32_t));
    uint32_t i_file_path_size_ntohl = ntohl(i_file_path_size);
    // printf("\n  dufp: i_file_path_size_ntohl: %d", i_file_path_size_ntohl); // TODO: Delete later
    upload_args_offset = upload_args_offset + 4;

    // file path
    p_file_path_array = (char*) calloc(i_file_path_size_ntohl, sizeof(char));
    memcpy(p_file_path_array, upload_args + upload_args_offset,i_file_path_size_ntohl);
    upload_args_offset = upload_args_offset + i_file_path_size_ntohl;
    // printf("\n  dufp: p_file_path_array: %s", p_file_path_array); // what you want // TODO: Delete later

    // allocate the buffer
    *file_path_out = (char*) calloc(i_file_path_size_ntohl, sizeof(char));
    // write to the file_path_out buffer
    memcpy(*file_path_out, p_file_path_array, i_file_path_size_ntohl);

    free(p_file_path_array);

    // refer to the README.md to inform how to deserialize just the upload path
    return i_file_path_size_ntohl;
}

/**
 * @brief deserialize the file contents and contents length from upload arguments.
 * @param upload_args the full upload arguments passed in the command structure
 * @param file_path_len length of the already deserialize file path
 * @param contents_out a NULL pointer that will contain the file contents on success or NULL on error
 * @return the length of the file path
 */
static uint32_t deserialize_upload_file_contents(char *upload_args, uint32_t file_path_len, char **contents_out)
{
    uint32_t content_len;
    int32_t i_file_contents_size = 0;
    int upload_args_offset = file_path_len;
    char* p_file_contents_array = NULL;
    uint32_t i_file_contents_size_ntohl = 0;

    // printf("\n  dufc: file_path_len: %d", file_path_len); // TODO: Delete later
    // printf("\nahhhh: %d", upload_args_offset + file_path_len); // TODO: Delete later
    // printf("\n upload_args_offset: %d", upload_args_offset); // TODO: Delete later

    // file contents size
    // get the file path size first so you can them get the correct amount of bytes of the actual file path
    memcpy(&i_file_contents_size, upload_args + upload_args_offset, sizeof(uint32_t));
    i_file_contents_size_ntohl = ntohl(i_file_contents_size);
    // printf("\n  dufc: i_file_contents_size_ntohl: %d", i_file_contents_size_ntohl); // TODO: Delete later
    upload_args_offset = upload_args_offset + 4;
    
    // file path
    p_file_contents_array = (char*) calloc(i_file_contents_size_ntohl, sizeof(char));
    memcpy(p_file_contents_array, upload_args + upload_args_offset,i_file_contents_size_ntohl);
    upload_args_offset = upload_args_offset + i_file_contents_size_ntohl;
    // printf("\n  dufc: p_file_contents_array: %s", p_file_contents_array); // what you want // TODO: Delete later
    

    // allocate the buffer (added +1 to account for the null terminator)
    *contents_out = (char*) calloc(i_file_contents_size_ntohl+1, sizeof(char));
    // write to the file_path_out buffer
    memcpy(*contents_out, p_file_contents_array, i_file_contents_size_ntohl);
    // refer to the README.md to inform how to deserialize just the file contents

    // free contents buffer used to read in the contents
    free(p_file_contents_array);

    return i_file_contents_size_ntohl;
}

/**
 * @brief Upload the file specified in the command arguments.
 * @param cmd the command structure
 * @return a response indicating upload success or failure
 */
Response *upload_file_command(Command *cmd)
{
    Response *rsp = NULL;
    char* file_path;
    char* contents;
    uint32_t i_file_path_length = 0;
    uint32_t i_file_contents_length = 0;
    uint32_t contents_size_write = 0;

    // Deserialize path and contents
    // printf("\ncmd len: %d\ncmd: %s\nargs len: %d\nargs: %x\n", cmd->cmd_len, cmd->cmd, cmd->args_len, cmd->args); // x for the cmd.args might be wrong lol // TODO: Delete later
    // pass in the cmd.args which you can get the file size and file path (return the file path size to use to add to offset?)
    i_file_path_length = deserialize_upload_file_path(cmd->args, &file_path);
    // printf("\nufc: i_file_path_length: %d", i_file_path_length); // TODO: Delete later
    i_file_contents_length = deserialize_upload_file_contents(cmd->args, i_file_path_length+4, &contents); // added four to account for the uint31 (4 bytes) not included in the i_file_path_length {uint32:file_path_size}{char[file_path_size]:file_path}
    // printf("\nufc: i_file_contents_length: %d", i_file_contents_length); // TODO: Delete later
    // printf("\nufc: file_path: %s", file_path); // TODO: Delete later
    // printf("\nufc: file_path: %s", contents); // TODO: Delete later
    
    
    // Write file
    contents_size_write = write_file(file_path, contents,i_file_contents_length);
    // printf("\nufc: contents_size_write: %d", contents_size_write); // TODO: Delete later

    // if write_file couldn't write anything ot came back zero then... otherwise create the successful response
    if (contents_size_write == 0){
        // if the filename is bad (i.e 0 or nothing) then create the rsp with it's own error message
        if (i_file_path_length == 0){
            rsp = alloc_response(1, "error with upload file path", strlen("error with upload file path"));
        }
        // else the only other error, the file contents is bad (i.e 0 or nothing) then create the rsp with it's own error message
        else{
            rsp = alloc_response(1, "error with upload file contents", strlen("error with upload file contents"));
        }
        
    }
    else{
        rsp = alloc_response(0, "upload successful", strlen("upload successful"));
    }

    free(file_path);
    free(contents);

    return rsp;
}
