/**
 * filestests.c contains all of the tests for the files module.
 * @file filestests.c
 */

#include <tests.h>
#include <systests/systests.h>
#include <sys.h>
#include <utils.h>
#include <commands.h>
#include <sys/stat.h>

bool ERROR_OUT;

bool should_provide_hostname_response()
{
    char *test_name = "should_provide_hostname_response";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int good_ret_code = 0;
    char *command = "hostname";
    char *args = "";
    char *hostname = "test_host";
    size_t  command_len, args_len;
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    ERROR_OUT = false;
    rsp = hostname_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(good_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(hostname, rsp->msg, strlen(hostname), "expected the test hostname")
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}

bool should_error_failed_hostname()
{
    char *test_name = "should_error_failed_hostname";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    char *command = "hostname";
    char *args = "";
    size_t  command_len, args_len;
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    int bad_ret_code = 1;
    char *error_msg = HOSTNAME_ERROR_MSG;
    ERROR_OUT = true;
    rsp = hostname_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(bad_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(error_msg, rsp->msg, strlen(error_msg), "expected an error message")
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}

bool should_error_fail_process_list()
{
    char *test_name = "should_error_fail_process_list";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    char *command = "proclist";
    char *args = "1";
    size_t command_len, args_len;
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    ERROR_OUT = true;
    rsp = process_list_command(cmd);
    ASSERT_NOT_NULL(rsp, "Response should not be NULL");
    ASSERT_STRING_EQUALS(PROCESS_ERROR_MSG, rsp->msg, rsp->msg_len, "expected error response");    
    free_response(rsp);
    rsp = NULL;
    free_command(cmd);
    cmd = NULL;
    TEST_PASSED(test_name);
}

bool should_provide_valid_process_list()
{
    char *test_name = "should_provide_valid_process_list";
    char *valid_test_str = "root          55166        1 snapd                /usr/lib/snapd/snapd\n";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    char *command = "proclist";
    char *args = "55166";
    size_t  command_len, args_len;
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    ERROR_OUT = false;
    rsp = process_list_command(cmd);
    ASSERT_NOT_NULL(rsp, "Response should not be NULL");    
    ASSERT_STRING_EQUALS(valid_test_str, rsp->msg, rsp->msg_len , "expected a legitimate process list");
    free_response(rsp);
    free_command(cmd);
    rsp = NULL;
    cmd = NULL;
    TEST_PASSED(test_name)
}

bool should_error_fail_netstat()
{
    char *test_name = "should_error_fail_netstat";
    TEST_START(test_name);
    Command *cmd = NULL;
    Response *rsp = NULL;
    char *command = "netstat";
    char *args = "";
    size_t command_len, args_len;
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    ERROR_OUT = true;
    rsp = get_netstat_command(cmd);
    ASSERT_NOT_NULL(rsp, "Response should not be NULL");
    ASSERT_STRING_EQUALS(NETSTAT_ERROR_MSG, rsp->msg, rsp->msg_len, "expected error msg");
    free_response(rsp);
    rsp = NULL;
    free_command(cmd);
    cmd = NULL;
    TEST_PASSED(test_name);
}

bool should_provide_valid_netstat()
{
    char *test_name = "should_provide_valid_netstat";
    TEST_START(test_name);
    Command *cmd = NULL;
    Response *rsp = NULL;
    char *command = "netstat";
    char *args = "";
    size_t command_len, args_len;
    char *netstat_list = "LOCAL                 REMOTE                STATUS     OWNER                     INODE\n\
     127.0.0.53:53            0.0.0.0:0     LISTENING  alice                     40449\n\
      127.0.0.1:1337        127.0.0.1:57204 ESTAB      alice                   1532812\n";
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    ERROR_OUT = false;
    rsp = get_netstat_command(cmd);
    ASSERT_NOT_NULL(rsp, "Response should not be NULL");
    ASSERT_STRING_EQUALS(netstat_list, rsp->msg, rsp->msg_len, "expected a legitimate netstat");
    free_response(rsp);
    rsp = NULL;
    free_command(cmd);
    cmd = NULL;
    TEST_PASSED(test_name)
}

bool should_provide_valid_cmd_return()
{
    char *test_name = "should_provide_valid_cmd_return";
    TEST_START(test_name);
    Command *cmd = NULL;
    Response *rsp = NULL;
    char *command = "invoke";
    char *args = "echo 'Test is successful'";
    size_t command_len, args_len;
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    ERROR_OUT = false;
    rsp = invoke_command(cmd);
    ASSERT_NOT_NULL(rsp, "Response should not be NULL");
    ASSERT_STRING_EQUALS("Test is successful\n", rsp->msg, rsp->msg_len, "expected strings to be equal");
    free_response(rsp);
    rsp = NULL;
    free_command(cmd);
    cmd = NULL;
    TEST_PASSED(test_name);
}

bool should_touch_file_with_invoke()
{
    char *test_name = "should_touch_file_with_invoke";
    TEST_START(test_name);
    Command *cmd = NULL;
    Response *rsp = NULL;
    char *command = "invoke";
    char *file_path = "./test/data/test_file1.txt";
    char *args = "touch ./test/data/test_file1.txt";
    size_t command_len, args_len;
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    ERROR_OUT = false;
    rsp = invoke_command(cmd);
    ASSERT_NOT_NULL(rsp, "Response should not be NULL");
    ASSERT_EQUALS(0, remove(file_path), "File 'testFile' does not exist");
    free_response(rsp);
    rsp = NULL;
    free_command(cmd);
    cmd = NULL;
    TEST_PASSED(test_name);
}