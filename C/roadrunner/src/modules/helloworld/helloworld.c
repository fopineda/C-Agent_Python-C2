/**
 * helloworld.c contains the functionality for the RoadRunner say_hello function.
 * @file helloworld.c
 */

//include our header file which has our function declaration
#include <helloworld.h>

/**
 * @brief A simple function for testing to see if the agent is functioning, prints a hello message to stdout.
 */
void say_hello() 
{
    //use the debug statements for this module to print "RoadRunner says hello!" to the console
    HELLOWORLD_DEBUG_PRINT("RoadRunner says hello!")
}