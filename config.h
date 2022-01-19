/*
 * This is the config file for RatFE. Any changes you make here requires
 * a recompile, but RatFE shouldn't take long to compile ;)
 *
 * You don't need to understand C, I've created simple types to represent
 * what you need to set without any C knowledge.
 */

#ifndef CONFIG_H
#define CONFIG_H
#define FALSE 0
#define TRUE 1

/*
 * String: config_canonical_name
 *
 * The software's recognizable name.
 *
 * 
 * Example: "RatFE"
 */

static const char* config_canonical_name = "RatFE";

/*
 * String: config_instance_url
 *
 * The instances URL which all API calls will be sent to via mastodont-c.
 * This MUST include a slash at the end, and the protocol (like https://) at the
 * beginning
 *
 * Example: https://cum.desupost.soy/
 */
static const char* config_instance_url = "https://cum.desupost.soy/";

#endif // CONFIG_H
