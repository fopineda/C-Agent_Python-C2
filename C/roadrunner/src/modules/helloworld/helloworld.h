/**
 * helloworld.h contains all of the helloworld definitions and exports.
 * @file helloworld.h
 */

#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#if defined(HELLOWORLD_DEBUG) || defined(RR_DEBUG_ALL)
#include <stdio.h>
#define HELLOWORLD_DEBUG_PRINT(msg, ...) printf(msg, ##__VA_ARGS__);
#else
#define HELLOWORLD_DEBUG_PRINT(msg, ...) 
#endif

// declare our function, when another file includes this header file they will see that the function is
// defined in our helloworld.c file.

void say_hello();

#endif
