#ifndef LOGGER_H

#define LOGGER_H
/**
 * Initalize Logger options (Should only be called once at the start):
 * - console_enable : if false(0) disables output on console. Otherwise, will
 *output on console.
 * - file_enable : if false(0) disables output on the file named file_name.
 *Otherwise, will output on the file.
 * - file_name : The name of the file to write to. Will overwrite the file.
 *Won't matter if file_enable is false.
 **/
void init_logger(char console_enable, char file_enable, char *file_name);
/*
 * - Prints the msg string to the valid streams.
 */
void log_msg(char *msg);
/*
 * Closes the logger and releases all taken resources.
 */
void close_logger(void);
/*
 * Flushs output streams
 */
void log_flush(void);
#endif
