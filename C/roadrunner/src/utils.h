/**
 * utils.h contains macros and definitions that are used throughout the agent.
 * @file utils.h
 */

#ifndef UTILS_H
#define UTILS_H

#if defined(UTILS_DEBUG) || defined(RR_DEBUG_ALL)
#include <stdio.h>
#define UTILS_DEBUG_PRINT(msg, ...) printf(msg, ##__VA_ARGS__);
#else
#define UTILS_DEBUG_PRINT(msg, ...) 
#endif

#define GOOD_
#define INVALID_FD -1
#define BAD_READ -1
#define MAX_RSP_MSG_SIZE 1000

// here are two example of function like macros
/* lenof calculates the length of an array */
#define lenof(arr) sizeof(arr) / sizeof(arr[0])

/* checkfree checks if a pointer equals NULL and if does not it
    frees the memory and sets it equal to NULL */
#define checkfree(ptr)                          \
    if(ptr != NULL)                             \
    {                                           \
        free(ptr);                              \
        ptr = NULL;                             \
    }

/**
 * Checks to see if its a valid file descriptor and closes it if it is
 */
#define checkclose(fd)                          \
    if(fd != INVALID_FD)                        \
    {                                           \
        close(fd);                              \
    }                                           \
    fd = INVALID_FD;                            

#endif