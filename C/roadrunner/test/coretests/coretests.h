/**
 * coretests.h contains all of the coretests definitions and exports.
 * @file coretests.h
 */

#ifndef CORETESTS_H
#define CORETESTS_H

#include <stdbool.h>
bool should_provide_checkin_response();
bool should_error_on_bad_sleep_args();
bool should_execute_good_sleep_cmd();
#endif