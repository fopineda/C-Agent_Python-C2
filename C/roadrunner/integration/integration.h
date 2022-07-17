/**
 * integration.h contains definitions for the integration tests.
 * @file integration.h
 */

#ifndef INTEGRATION_H
#define INTEGRATION_H

#include <stdio.h>
#include <commands.h>
#include <stdint.h>

#define INTEGRATION_HOST "localhost"
#define INTEGRATION_PORT 1337
#define MIN_RESPONSE_SIZE sizeof(int) + sizeof(size_t)

#endif