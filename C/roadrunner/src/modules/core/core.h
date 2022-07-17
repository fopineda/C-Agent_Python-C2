/**
 * core.h contains all of the core definitions and exports.
 * @file core.h
 */

#ifndef CORE_H
#define CORE_H

#if defined(CORE_DEBUG) || defined(RR_DEBUG_ALL)
#include <stdio.h>
#define CORE_DEBUG_PRINT(msg, ...) printf(msg, ##__VA_ARGS__);
#else
#define CORE_DEBUG_PRINT(msg, ...) 
#endif

#include <commands.h>

#define SLEEP_CMD_STR  "sleep"

Response *checkin_command();
Response *sleep_command(Command *cmd);

#endif
