/**
 * coretests.c contains all of the tests for the core module
 * @file coretests.c
 */

#include <tests.h>
#include <coretests/coretests.h>
#include <core.h>
#include <strings.h>
#include <stdlib.h>

/**
 * @brief Test if the core module checkin command returns a valid checkin response.
 * @return bool representing success or failure
 */
bool should_provide_checkin_response()
{
    char *test_name = "should_provide_checkin_response";
    TEST_START(test_name)
    Response *rsp = NULL;
    int checkin_ret_code = 0;
    char *checkin_msg = "roadrunner checkin";
    rsp = checkin_command();
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(checkin_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(checkin_msg, rsp->msg, strlen(checkin_msg), "expected a different checkin message")
    free_response(rsp);
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the core module sleep command executes the sleep and returns a good response.
 * @return bool representing success or failure
 */
bool should_execute_good_sleep_cmd()
{
    char *test_name = "should_execute_good_sleep_cmd";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    char *command = "sleep";
    char sleep_args[2];
    unsigned int command_len, args_len;
    int good_ret_code = 0;
    char *good_msg = "road runner slept for 1 second(s)";
    sprintf(sleep_args, "%d", 1);
    command_len = strlen(command);
    args_len = strlen(sleep_args);
    cmd = alloc_command(command, command_len, sleep_args, args_len);
    rsp = sleep_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(good_ret_code, rsp->ret_code, "expected a good return code");
    ASSERT_STRING_EQUALS(good_msg, rsp->msg, strlen(good_msg),"expected a good execution message");
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the core module sleep command gracefully handles bad arguments passed to it.
 * @return bool representing success or failure
 */
bool should_error_on_bad_sleep_args()
{
    char *test_name = "should_error_on_bad_sleep_args";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int error_ret_code = 1;
    char *null_args_msg = "arguments provided are NULL";
    char *invalid_args_msg = "arguments are not of the correct type";
    cmd = alloc_command("sleep", 5, "null_args", 9);
    free(cmd->args);
    cmd->args = NULL;
    rsp = sleep_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(error_ret_code, rsp->ret_code, "expected an error return code");
    ASSERT_STRING_EQUALS(null_args_msg, rsp->msg, strlen(null_args_msg),"expected a NULL args message");
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    cmd = alloc_command("sleep", 5, "bad_args", 8);
    rsp = sleep_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(error_ret_code, rsp->ret_code, "expected an error return code");
    ASSERT_STRING_EQUALS(invalid_args_msg, rsp->msg, strlen(invalid_args_msg),"expected an invalid args message");
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}