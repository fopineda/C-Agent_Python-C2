#include <tests.h>
#include <filestests/filestests.h>
#include <files.h>
#include <sys.h>
#include <strings.h>



bool should_provide_download_response()
{
    char *test_name = "should_provide_download_response";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int good_ret_code = 1;
    char *download_msg = "I AM A TEST FILE";
    char *file_path = "./test/data/test_file.txt";
    cmd = alloc_command("download", file_path);
    rsp = download_file_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(good_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(download_msg, rsp->msg, strlen(download_msg), "expected the contents of the test file")

    //CB - - sys
    rsp = get_hostname_command();
    LOCAL_PRINT("hostname: %s\n", rsp->msg);
    pid_stat_t *test_pid = (pid_stat_t*)calloc(1, sizeof(pid_stat_t));
    parse_pid_stat("/proc/15310/stat\0", test_pid);
    parse_pid_owner("/proc/15310/loginuid\0", test_pid);
    parse_pid_cmdline("/proc/15310/cmdline\0", test_pid);
    //return_plist_line("15310");
    //get_process_list(cmd);
    //cmd = alloc_command("plist", "27246");
    //get_process_list(cmd);
    get_netstat(cmd);

    //CB - - end

    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}


bool should_error_on_bad_download_file_name()
{
    char *test_name = "should_error_on_bad_download_file_name";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int error_ret_code = -1;
    char *error_msg = "error downloading file";
    char *file_path = "./test/data/not_a_test_file.txt";
    cmd = alloc_command("download", file_path);
    rsp = download_file_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(error_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(error_msg, rsp->msg, strlen(error_msg), "expected a download error message")
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}
