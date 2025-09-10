/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "logger.h"

uint8_t
log_level_from_str(char* str)
{
	if (strcmp(str, "warn") == 0) return LOG_WARN | LOG_ERROR;
	if (strcmp(str, "info") == 0) return LOG_INFO | LOG_WARN | LOG_ERROR;
	if (strcmp(str, "debug") == 0) return LOG_DEBUG | LOG_INFO | LOG_WARN | LOG_ERROR;
	return LOG_ERROR;
}

uint8_t
log_type_from_str(char* str)
{
	if (strcmp(str, "console") == 0) return LOG_CONSOLE;
	if (strcmp(str, "syslog") == 0) return LOG_SYSLOG;
	// This works for config.c
	return LOG_FILE;
}

int
debug(char const* msg, ...)
{
	int res;
	va_list ap;
	va_start(ap, msg);
	// TODO pass to function
	printf("[DEBUG] ");
	res = vprintf(msg, ap);
	puts("");
	va_end(ap);
	return res;
}


int
error(char const* msg, ...)
{
	int res;
	va_list ap;
	va_start(ap, msg);
	// TODO pass to function
	printf("[ERROR] ");
	res = vprintf(msg, ap);
	puts("");
	va_end(ap);
	return res;
}
