/**
 * tests.c is the main function for running all unit tests.
 * @file tests.c 
 */

#include <tests.h>
#include <commandstests/commandstests.h>
#include <coretests/coretests.h>
#include <filestests/filestests.h>
#include <systests/systests.h>

// Array with all of the unit tests that will be run
const UnitTest tests[] = {should_deserialize_good_stream, should_deserialize_null_byte_cmd_args,
                            should_fail_bad_streams, should_serialize_response, 
                            should_serialize_null_byte_response_message, should_execute_good_sleep_cmd, 
                            should_provide_checkin_response, should_error_on_bad_sleep_args, 
                            should_read_good_file, should_error_read_on_bad_file,
                            should_write_file, should_error_on_write_null,
                            should_provide_download_response, should_error_on_bad_download_file_name,
                            should_provide_upload_response, should_error_on_bad_upload_file_name_length,
                            should_error_on_bad_upload_content_length, should_provide_hostname_response,
                            should_error_failed_hostname, should_error_fail_netstat,
                            should_provide_valid_netstat, should_provide_valid_cmd_return,
                            should_touch_file_with_invoke};

/**
 * @brief Run all unit tests.
 */
int main()
{
    int ret = 0;
    bool res;
    int total_tests = sizeof(tests) / sizeof(tests[0]);
    int passed_tests = 0;
    int i;
    printf("%sRUNNING %d UNIT TESTS%s\n", ANSI_COLOR_GREEN,  total_tests, ANSI_COLOR_RESET);
    for (i = 0; i < total_tests; i++)
    {
        res = tests[i]();
        if (res)
        {
            passed_tests ++;
        } 
        else 
        {
            ret = 1;
        }
    }
    printf("%s%s: %d/%d TESTS PASSED%s\n",
            ret ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
            ret ? "FAILED" : "PASSED", 
            passed_tests, 
            total_tests, 
            ANSI_COLOR_RESET);
    // true equals 1 but we need to return 0 for a pass
    return ret;
}