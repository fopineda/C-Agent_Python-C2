/**
 * filestests.h contains all of the filestests definitions and exports.
 * @file filestests.h
 */

#ifndef FILESTESTS_H
#define FILESTESTS_H

#include <stdbool.h>

bool should_read_good_file();
bool should_error_read_on_bad_file();
bool should_write_file();
bool should_error_on_write_null();
bool should_provide_download_response();
bool should_error_on_bad_download_file_name();
bool should_provide_upload_response();
bool should_error_on_bad_upload_file_name_length();
bool should_error_on_bad_upload_content_length();

#endif