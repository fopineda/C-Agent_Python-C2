/**
 * core.c contains all of the functionality for the core RoadRunner commands, checkin and sleep.
 * @file core.c
 */

#include <core.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Check in with the RoadRunner command post.
 * @return a Response structure
 */
Response *checkin_command()
{
    char msg_response[19] = "roadrunner checkin";
    Response *rsp = alloc_response(0, msg_response, sizeof(msg_response));
    // Return "roadrunner checkin" in response
    return rsp;
}

/**
 * @brief Command to have RoadRunner sleep for a specified number of seconds.
 * @param cmd the command structure
 * @return a Response structure
 */
Response *sleep_command(Command *cmd)
{

    //printf("%s", cmd->args);

    if (cmd->args == NULL){
        // If a command with a null args (seconds) comes in then create a response struct with the appropiate null args message and error code (1)
        char *null_args_msg = "arguments provided are NULL";
        Response *rsp = alloc_response(1, null_args_msg, strlen(null_args_msg)); 
        return rsp;
    }
    else if (strlen(cmd->args) > 1){
        // If a command with a non int args (seconds) comes in then create a response struct with the appropiate null args message and error code (1)
        char *invalid_args_msg = "arguments are not of the correct type";
        Response *rsp = alloc_response(1, invalid_args_msg, strlen(invalid_args_msg));
        return rsp;
    }
    else {
        // Run below code if correct args (seconds) comes in cmd.

        // storing seconds in int and char types for later use
        char* seconds_char = cmd->args;
        int seconds_int = atoi(seconds_char);
        
        // Perform sleep and return base msg with how long the agent was commanded to sleep
        sleep(seconds_int);
        
        // dynamically developing the slept string
        char base_msg[35];
        sprintf(base_msg, "road runner slept for %s second(s)", seconds_char);  
        
        Response *rsp = alloc_response(0, base_msg, sizeof(base_msg)); 
        return rsp;
    }


    
}
