#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "cgi.h"
#include "config.h"

char* config_canonical_name = NULL;
char* config_instance_url = NULL;
char* config_host_url = NULL;
unsigned config_host_url_insecure = 0;
unsigned config_library_flags = 0;
char* config_url_prefix = NULL;
unsigned config_experimental_lookup = 0;
unsigned config_test_page = 0;
// Log
char* config_log_level = NULL;
uint8_t config_log_level_bitmask = LOG_ERROR;
char* config_log_output = NULL;
uint8_t config_log_type_bitmask = LOG_CONSOLE;

int debug(char const* msg, ...);

unsigned
uintify(char* str)
{
	// Do int check
	char* err;
	long val = strtol(str, &err, 10);
	if (*err == '\0' && *str != '\0')
		return labs(val);
	
	str[0] = tolower(str[0]);
	return str[0] == 't';
}

int
parse_config(FILE* cfg)
{
	char buf[256];
	if (!cfg)
	{
		perror("[CONFIG] Couldn't load config");
		return 2;		
	}
	while (fgets(buf, 256, cfg))
	{
		if (buf[0] == '!' || buf[0] == ' ')
		{
			debug("Comment: %s", buf+1);
			continue;
		}
		if (buf[0] == '\r' || buf[0] == '\n')
			continue;
		
		char* eq = strchr(buf, '=');
		if (!eq) goto bad;
		
		*eq = '\0';
		// Remove any precending spaces
		if (eq[-1] == ' ')
		{
			char* sp = strchr(buf, ' ');
			*sp = '\0';
		}
		++eq;
		if (*eq == '\0') goto bad;
		while (*eq == ' ') ++eq;
		char* key = buf;
		char* value = eq;
		
		char* end = strchr(value, '\n');
		char* end2 = strchr(value, '\r');
		if (end) *end = '\0';
		if (end2) *end2 = '\0';
		
		
		/* So what's funny, and possibly a compiler being too strict.
		 *  I was having a second value to say if it was a uint.
		 * This caused -Wint-conversion with -Wconditional-type-mismatch
		 *  because of how it kept misinterpreting the type-unsafetyness of things
		 *  even when I used it in completely safe ways.
		 * This works too.
		 */
#		define KV_CFG_S(cfgkey) \
			if (strcmp(key, #cfgkey) == 0) { \
				config_ ## cfgkey = strdup(value); \
				debug("Config: config_%s = %s",#cfgkey, config_ ## cfgkey); \
			}
	
#		define KV_CFG_U(cfgkey) \
			if (strcmp(key, #cfgkey) == 0) { \
				config_ ## cfgkey = uintify(value); \
				debug("Config: config_%s = %d",#cfgkey, config_ ## cfgkey); \
			}
		
		KV_CFG_S(canonical_name);
		KV_CFG_S(instance_url);
		KV_CFG_S(host_url);
		KV_CFG_U(host_url_insecure);
		KV_CFG_U(library_flags);
		KV_CFG_S(url_prefix);
		KV_CFG_U(experimental_lookup);
		KV_CFG_U(test_page);
		// Log
		KV_CFG_S(log_level);
		KV_CFG_S(log_output);
#		undef KV_CFG_U
#		undef KV_CFG_S
		
	} // while(...)
	
	// Post config
	config_log_level_bitmask = log_level_from_str(config_log_level);
	// Not really... but can be done
	config_log_type_bitmask = log_type_from_str(config_log_output);
	
	fclose(cfg);
	debug("got %s\n", config_instance_url);
	return 0;
bad:
	fclose(cfg);
	fputs("[CONFIG] Failed to parse config.", stderr);
	return 1;
}
