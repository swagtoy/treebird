/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef TB_LOGGER_H
#define TB_LOGGER_H
#include <stdint.h>

enum log_level
{
	LOG_ERROR = 1,
	LOG_WARN  = 2,
	LOG_INFO  = 4,
	LOG_DEBUG = 8,
};

enum log_type
{
	LOG_CONSOLE = 1,
	LOG_SYSLOG  = 2,
	LOG_FILE    = 4,
};

uint8_t log_level_from_str(char* str);
uint8_t  log_type_from_str(char* str);

int error (char const* msg, ...);
#undef warn
/* int warn  (char const* msg, ...); */
/* int info  (char const* msg, ...); */
int debug (char const* msg, ...);

#endif // TB_LOGGER_H
