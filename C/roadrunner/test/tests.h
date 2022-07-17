/**
 * tests.h contains all of the definitions and exports needed to execute and create unit tests.
 * @file tests.h
 */

#ifndef TESTS_H
#define TESTS_H


#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// define the unit test function pointer
typedef bool (*UnitTest)(void);

#define TEST_START(name) printf("%sTEST RUNNING: %s%s\n", ANSI_COLOR_YELLOW, name, ANSI_COLOR_RESET);
#define TEST_PASSED(name)                                                                   \
    printf("%sTEST PASSED: %s%s\n", ANSI_COLOR_GREEN, name, ANSI_COLOR_RESET);              \
    return true;
#define TEST_FAILED(name)                                                                   \
    printf("%sTEST FAILED: %s\n%s", ANSI_COLOR_RED, name, ANSI_COLOR_RESET);                \
    return false;
#define FAILURE_MESSAGE "\x1b[31mFAILURE: %s\x1b[0m\n"


/*
 * Assert that a value is NULL.
 * @param x value
 * @param msg failure message
 */
#define ASSERT_NULL(x, msg)                                                 \
    if (x != NULL)                                                          \
    {                                                                       \
        printf(FAILURE_MESSAGE, msg);                                       \
        printf("%s%d != NULL%s\n", ANSI_COLOR_RED, x, ANSI_COLOR_RESET);    \
        TEST_FAILED(test_name);                                             \
    }

/*
 * Assert that a value is not NULL.
 * @param x value
 * @param msg failure message
 */
#define ASSERT_NOT_NULL(x, msg)                                             \
    if (x == NULL)                                                          \
    {                                                                       \
        printf(FAILURE_MESSAGE, msg);                                       \
        printf("%s%d == NULL%s\n", ANSI_COLOR_RED, x, ANSI_COLOR_RESET);    \
        TEST_FAILED(test_name);                                             \
    }

/*
 * Assert that a value is true.
 * @param x value
 * @param msg failure message
 */
#define ASSERT_TRUE(x, msg)                                                 \
    if (x != true)                                                          \
    {                                                                       \
        printf(FAILURE_MESSAGE, msg);                                       \
        printf("%s%d != True%s\n", ANSI_COLOR_RED, x, ANSI_COLOR_RESET);    \
        TEST_FAILED(test_name);                                             \
    }

/*
 * Assert that a value is false.
 * @param x value
 * @param msg failure message
 */
#define ASSERT_FALSE(x, msg)                                                \
    if (x != false)                                                         \
    {                                                                       \
        printf(FAILURE_MESSAGE, msg);                                       \
        printf("%s%d != False%s\n", ANSI_COLOR_RED, x, ANSI_COLOR_RESET);   \
        TEST_FAILED(test_name);                                             \
    }

/*
 * Assert that two values are equal.
 * @param x value1
 * @param y value2
 * @param msg failure message
 */
#define ASSERT_EQUALS(x, y, msg)                                            \
    if (x != y)                                                             \
    {                                                                       \
        printf(FAILURE_MESSAGE, msg);                                       \
        printf("%s%d != %d%s\n", ANSI_COLOR_RED, x, y, ANSI_COLOR_RESET);   \
        TEST_FAILED(test_name);                                             \
    }

/*
 * Assert that two values are not equal.
 * @param x value1
 * @param y value2
 * @param msg failure message
 */
#define ASSERT_NOT_EQUALS(x, y, msg)                                        \
    if (x == y)                                                             \
    {                                                                       \
        printf(FAILURE_MESSAGE, msg);                                       \
        printf("%s%d == %d%s\n", ANSI_COLOR_RED, x, y, ANSI_COLOR_RESET);   \
        TEST_FAILED(test_name);                                             \
    }

/*
 * Assert that two strings are equal.
 * @param x string1
 * @param y string2
 * @param len string length
 * @param msg failure message
 */
#define ASSERT_STRING_EQUALS(x, y, len, msg)                                \
    if (strncmp(x, y, len) != 0)                                            \
    {                                                                       \
        printf(FAILURE_MESSAGE, msg);                                       \
        printf("%s%s != %s%s\n", ANSI_COLOR_RED, x, y, ANSI_COLOR_RESET);   \
        TEST_FAILED(test_name);                                             \
    }

/*
 * Assert that two strings are not equal.
 * @param x string1
 * @param y string2
 * @param len string length
 * @param msg failure message
 */
#define ASSERT_STRING_NOT_EQUALS(x, y, len, msg)                            \
    if (strncmp(x, y, len) == 0)                                            \
    {                                                                       \
        printf(FAILURE_MESSAGE, msg);                                       \
        printf("%s%s == %s%s\n", ANSI_COLOR_RED, x, y, ANSI_COLOR_RESET);   \
        TEST_FAILED(test_name);                                             \
    }

/*
 * Assert that two byte arrays are equal.
 * @param x byte array 1
 * @param y byte array 2
 * @param len byte array length
 * @param msg failure message
 */
#define ASSERT_BYTES_EQUALS(x, y, len, msg)                                     \
    int i;                                                                      \
    for (i = 0; i < len; i++)                                                   \
    {                                                                           \
        if(x[i] != y[i])                                                        \
        {                                                                       \
            printf(FAILURE_MESSAGE, msg);                                       \
            printf("%s%s != %s%s\n", ANSI_COLOR_RED, x, y, ANSI_COLOR_RESET);   \
            TEST_FAILED(test_name);                                             \
        }                                                                       \
    }

/*
 * Assert that two byte arrays are not equal.
 * @param x byte array 1
 * @param y byte array 2
 * @param len byte array length
 * @param msg failure message
 */
#define ASSERT_BYTES_NOT_EQUALS(x, y, len, msg)                                 \
    int i;                                                                      \
    int matched = 0                                                             \
    for (i = 0; i < len; i++)                                                   \
    {                                                                           \
        if(x[i] == y[i])                                                        \
        {                                                                       \
            matched ++;                                                         \
        }                                                                       \
    }                                                                           \
    if (matched == len)                                                         \
    {                                                                           \
        printf(FAILURE_MESSAGE, msg);                                           \
        printf("%s%s == %s%s\n", ANSI_COLOR_RED, x, y, ANSI_COLOR_RESET);       \
        TEST_FAILED(test_name);                                                 \
    }

#endif