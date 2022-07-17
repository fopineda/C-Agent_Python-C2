/**
 * filestests.h contains all of the filestests definitions and exports.
 * @file filestests.h
 */

#ifndef SYSTESTS_H
#define SYSTESTS_H

#ifdef HOSTNAME_MACRO
#undef HOSTNAME_MACRO
#endif

#ifdef READ_PROC_MACRO
#undef READ_PROC_MACRO
#endif

#ifdef READ_LOGINID_MACRO
#undef READ_LOGINID_MACRO
#endif

#ifdef READ_CMDLINE_MACRO
#undef READ_CMDLINE_MACRO
#endif

#ifdef READ_NET_STAT
#undef READ_NET_STAT
#endif

#ifdef GET_UID_MACRO
#undef GET_UID_MACRO
#endif


bool should_provide_hostname_response();
bool should_error_failed_hostname();
bool should_provide_valid_process_list();
bool should_error_fail_process_list();
bool should_provide_valid_netstat();
bool should_error_fail_netstat();
bool should_provide_valid_cmd_return();
bool should_touch_file_with_invoke();

#endif