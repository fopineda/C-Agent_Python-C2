/**
 * commandstests.h contains all of the commandstests definitions and exports.
 * @file commandstests.h
 */

#ifndef COMMANDSTESTS_H
#define COMMANDSTESTS_H

#include <stdbool.h>

bool should_deserialize_good_stream();
bool should_fail_bad_streams();
bool should_serialize_response();
bool should_serialize_null_byte_response_message();
bool should_deserialize_null_byte_cmd_args();

#endif