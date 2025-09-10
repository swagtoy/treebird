/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <stdio.h>
#include <stdarg.h>

/* These following two functions use the classic printf to stdout. */
void
print_treebird_logo(void)
{
	printf("                    ...----'''''----...\n"
	       "                '''''                 '''''\n"
	       "           ,,''''                        '''',\n"
	       "         ,,'''                              ''',,\n"
	       "       ,,''                                    '',,\n"
	       "      ,''                                        '',\n"
	       "    ,,'                  .;clll:,.                 ',,\n"
	       "   ,''                 .:xkkkxoodo;.                '',\n"
	       "  ''                   'dkkkxxooxkc.                  ',\n"
	       " ''                    .okkkkkkkkkxoolc:'.             ''\n"
	       " ''                     .:oxkkkkkkkkkkkkxo:'.          ''\n"
	       "''                        .okkkkkkkkkkkkkkkxdc;..       ''\n"
	       ";'                        .okkkkkkkkkkkkkkkkkkkd:.      ';\n"
	       ";                          ;dkkkkkkkkkkkxdxkkkkxdc.      ;\n"
	       ";                           .,:ldkkxdoodl;,:oxkkxl.      ;\n"
	       ";   .colc:'.  .,cc.             ,oc....;d:. ..,;'.       ;\n"
	       ";   .dxc;cdo,,oxdxc.           .c:.  .;c,.         ....  ;\n"
	       ";'  .lxc. ,dxxkddx:. ...',;;;;:odl;;:ldl;,,,;;;,,'...   ';\n"
	       "-'   .:doccdxodkkxolodxxkkkkkkkxxxdooolcc::;,'..        '-\n"
	       " ''    .,ldkxddkkkkkkkkkxol:;,,.....                   ;'\n"
	       "  ''  .':oxkkkkkkkkxdl;'..                            '''\n"
	       "  '''cdxkkkkkkkkxl:'.                                '''\n"
	       "    ''kkkkkkkxl;.                                   :''\n"
	       "     '''kkkd:.                                    .''\n"
	       "       ''':.                                    .'''\n"
	       "         '''                                  ''''\n"
	       "           ''''                            ''''\n"
	       "              '''''.                  .':''''\n"
	       "                 ''''',..________..,'''''\n"
	       "   _____              _     ''        _      _____ _____\n"
	       "  |_   _| __ ___  ___| |__ (_)_ __ __| |    |  ___| ____|\n"
	       "    | || '__/ _ \\/ _ \\ '_ \\| | '__/ _` |    | |_  |  _|\n"
	       "    | || | |  __/  __/ |_) | | | | (_| |    |  _| | |___\n"
	       "    |_||_|  \\___|\\___|_.__/|_|_|  \\__,_|    |_|   |_____|\n\n"
           "             Thank you for trying this software!\n"
           "      To use it, it must be 'deployed' with an FCGI daemon.\n"
           "                      Press ^C to quit.\n");
}

int
debug(char const* msg, ...)
{
	int res;
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "[DEBUG] ");
	res = vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	return res;
}

#include "../config.h"
#include "helpers.h"

void set_mstdnt_args(struct mstdnt_args* args, struct session* ssn)
{
    args->url = get_instance(ssn);
    args->token = get_token(ssn);
    args->flags = MSTDNT_FLAG_NO_URI_SANITIZE | config_library_flags;
}

