/**--------------------------------------------------------------------------------
 *
 * pgreshape -- Embed the new column according to the desired position in any table
 * in the postgresql database.
 *
 *  UNSUPPORTED
 * ~~~~~~~~~~~
 * columns (comment, default, type, security, check, unique, constraint)
 * views
 * functions
 * index
 * constraints (unique, check, not-null, primary, foreign keys, exclusion key)
 * procedure
 * triggers
 * materialized views
 * rules
 * comments
 * acl
 *
 * Copyright (c) 2019, Rafael Garcia Sagastume
 *
 *---------------------------------------------------------------------------------
 */

#include "pgreshape.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void help(void);

static void help(void)
{
	printf("%s: Embed the new column according to the desired position in any table.\n\n", PGR_NAME);
}

int main(int argc, char const *argv[])
{
	ConfigFile *config = NULL;

	if (argc > 1)
	{
		if (strcmp(argv[1], "--help") == 0)
		{
			help();
			exit(EXIT_SUCCESS);
		}
		
		if (strcmp(argv[1], "--version") == 0)
		{
			printf(PGR_NAME " " PGR_VERSION "\n");
			exit(EXIT_SUCCESS);
		}

		if (strcmp(argv[1], "-c") == 0)
		{
			config = loadConfig(argv[2]);
		}
	}


	if(config!=NULL) {

		printf("host: %s\n", config->host);

	} else {
		printf("nothing to do.\n");
	}

	freeConfig(config);
	return 0;
}