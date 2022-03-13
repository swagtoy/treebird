/*
 * This is the config file for treebird. Any changes you make here requires
 * a recompile, but treebird shouldn't take long to compile ;)
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
 * Example: "treebird"
 */

static char* const config_canonical_name = "treebird";

/*
 * String: config_instance_url
 *
 * The instances URL which all API calls will be sent to via mastodont-c.
 * This MUST include a slash at the end, and the protocol (like https://) at the
 * beginning
 *
 * Example: "https://cum.desupost.soy/
 */
static char* const config_instance_url = "https://desuposter.club/";

/*
 * String: config_url_prefix
 *
 * The prefix for all urls.
 * For most cases, when you are proxying the CGI paths to root, this will be left blank.
 *
 * Example: "/treebird.cgi"
 */
static char* const config_url_prefix = "/treebird.cgi";

/*
 * Bool: experimental_lookup
 *
 * Uses experimental acct lookup (@user@domain)
 * This allows for the new lookup feature, which can get
 * information for local and federated accounts regardless if signed in.
 *
 * This feature is new and was introduced in the develop branch in 2022.
 * Keep this disabled if running an older pleroma build, or because you just
 * don't want it.
 */
static const int config_experimental_lookup = TRUE;

#endif // CONFIG_H
