#ifndef CONFIG_H
#define CONFIG_H
#include <stdio.h>
#include "logger.h"

extern char* config_canonical_name;
extern char* config_instance_url;
extern char* config_host_url;
extern unsigned config_host_url_insecure;
extern unsigned config_library_flags;
extern char* config_url_prefix;
extern unsigned config_experimental_lookup;
extern unsigned config_test_page;
// Log
extern char* config_log_level;
extern uint8_t config_log_level_bitmask;
extern char* config_log_output;
extern uint8_t config_log_type_bitmask;


/** Returns 1 on parse error, dumped to some log of sorts */
int
parse_config(FILE* cfg);

#endif // CONFIG_H
