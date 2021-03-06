/**--------------------------------------------------------------------------------
 *
 * pgreshape -- Embed the new column according to the desired position in any table
 * in the postgresql database.
 *
 * Copyright (c) 2019, Rafael Garcia Sagastume
 *
 *---------------------------------------------------------------------------------
 */

#include "pgreshape.h"
#include "config.h"
#include "common.h"
#include "table.h"
#include "view.h"
#include "update.h"
#include "trigger.h"
#include "schema.h"
#include "sequence.h"
#include <libpq-fe.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FILE *output;
PGTable *t;

static void help(void);
PGconn* pg_connect(const char * host, const char *user, const char *pass, const char *dbname, const char *port);
static void pg_close(PGconn *conn);
static void getTableObjects(PGconn *c, PGROption *opts);


/*
 * helper to connect to the database
 */
PGconn* pg_connect(const char * host, const char *user, const char *pass, const char *dbname, const char *port) {
	PGconn *conn = NULL;
	ConnStatusType status;
	conn = PQsetdbLogin(host, port, NULL, NULL, dbname, user, pass);
	if((status = PQstatus(conn)) != CONNECTION_OK) {
		printf("%s\n", PQerrorMessage(conn));
		conn = NULL;
	}
	
	return conn;
}


/*
 * helper to close to the database
 */
static void pg_close(PGconn *conn) {
	if(conn) {
		PQfinish(conn);       
	}
}


static void help(void)
{
	printf("%s: Embed the new column according to the desired position in any table.\n", PGR_NAME);
	printf("\n");
	printf("  -c, =FILENAME       configuration file*\n");
	printf("  -s                  the name of the database schema is configured\n");
	printf("  -t                  the name of the database table is configured\n");
	printf("  -offset             it is configured from the column of the database table\n");
	printf("                      that will subsequently have the new column\n");
	printf("  -column             the name of the new column is configured\n");
	printf("  -type               the type of the new column is set\n");
	printf("                      example: \n");
	printf("                          -type boolean\n");
	printf("                          or -type 'numeric (15, 2) default 10.20'\n");
	printf("                          or -type \"text default 'hola'\"\n");
	printf("  -file               argument for the sql file path, this will be priority if specified\n");
	printf("  example:\n");
	printf("  pgreshape -c /opt/reshape.conf -s rrhh -t expediente -offset emisor -column nueva -type 'boolean' -file /tmp/file-other.sql\n");
	printf("\n");
	printf("\n");
	printf("  file.sql            the name of the output file is file.sql in the execution path'\n");
	printf("\n");
	printf("  reshape.conf\n");
	printf("              host = localhost\n");
	printf("              port = 5432\n");
	printf("              dbname = db1\n");
	printf("              user = postgres\n");
	printf("              password = 123\n");
	printf("              file = /tmp/file.sql\n");
	printf("\n");
	printf("  --help              show this help, then exit\n");
	printf("  --version           output version information, then exit\n");
}


static void pgreshape(FILE *fout, PGROption *opts) {
	fprintf(fout, "--\n-- pgreshape %s\n", PGR_VERSION);
	fprintf(fout, "-- Copyright %s.\n", PGR_COPY);
	fprintf(fout, "--");

	fprintf(fout, "\n\nBEGIN;\n");
	fprintf(fout, "\nSET session_replication_role = replica;\n\n");
	dumpDisableTriggerAll(fout, t);

	/*dump drop unique*/
	dumpDropUnique(fout, t);

	/*dump drop index*/
	dumpDropIndex(fout, t);

	/*dump drop foreign keys*/
	dumpDropForeignKey(fout, t);

	/*dump drop exclude constraint*/
	dumpDropExcludeConstraint(fout, t);

	/*dump drop check constraint*/
	dumpDropCheckConstraint(fout, t);

	/*dump drop dependent views*/
	dumpDropDependentView(fout, t);

	/*create table temp for backup data*/
	fprintf(fout, "\n");
	dumpCreateTempTableBackup(fout, t);

	
	if (opts->offset == NULL || strcmp(opts->offset, "") == 0)
	{
		fprintf(fout, "Error -> offset not specified\n");
		exit(EXIT_SUCCESS);
	}

	/*dump drop columns on table*/
	dumpDropTableColumn(fout, t, opts);



	/*
	 *
	 * process for reshape the table with its attributes and dependencies
	 *
	 */

	fprintf(fout, "\n\n");
	
	/*create table security labels*/
	dumpTableSecurityLabels(fout, t);

	/*create sequences*/
	dumpCreateSequences(fout, t);

	/*create new column*/
	dumpNewColumn(fout, t, opts);

	/*recreate the processed columns*/
	dumpColumnTable(fout, t, opts);

	/*retrieve temporary backup information*/
	fprintf(fout, "\n");
	dumpUpdateData(fout, t, opts);

	/*dump to generate not null on column*/
	dumpSetNotNullColumnTable(fout, t, opts);

	/*dump acl on columns*/
	dumpAclColumnTable(fout, t, opts);

	/*dump attoptions on columns*/
	dumpOptionsColumnTable(fout, t, opts);

	/*dump to generate comment on columns*/
	dumpSetCommentColumnTable(fout, t, opts);

	/*create column security labels*/
	dumpColumnSecurityLabels(fout, t);

	/*dump to generate the foreign keys again*/
	fprintf(fout, "\n");
	dumpCreateForeignKey(fout, t);

	/*dump to generate the exclude constraint*/
	fprintf(fout, "\n");
	dumpCreateExcludeConstraint(fout, t);

	/*dump to generate the check constraint again*/
	fprintf(fout, "\n");
	dumpCreateCheckConstraint(fout, t);

	/*dump to generate index*/
	dumpCreateIndex(fout, t);

	/*dump to generate unique constraint*/
	dumpCreateUnique(fout, t);

	/*dump to generate dependent views*/
	dumpCreateCreateView(fout, t);

	fprintf(fout, "\n");
	dumpEnableTriggerAll(fout, t);
	fprintf(fout, "\n\nSET session_replication_role = DEFAULT;\n");

	fprintf(fout, "\n--COMMIT;\n");
	fprintf(fout, "\n--ROLLBACK;\n");
}


/*
 * process to find the dependencies of the table, views, functions, 
 * foreign keys for recessing the new column
 */
static void getTableObjects(PGconn *c, PGROption *opts) {

	t = getTable(c, opts);

	if (t == NULL)
	{
		/* code */
		printf("without results!\n");
		exit(EXIT_SUCCESS);
	}

	/*security labels for table*/
	getTableSecurityLabels(c, t);

	/*bring all the attributes of the table*/
	getTableAttributes(c, t);

	/*buscar etiquetas de seguridad para las columnas*/
	getColumnSecurityLabels(c, t, opts);

	/*Search all sequences referenced to columns*/
	getSequences(c, t, opts);

	/*security labels for sequences*/
	getSequenceSecurityLabels(c, t);

	/*Search all indexes referenced to the table*/
	getTableIndexes(c, t);

	/*Search all unique key referenced to the table*/
	getTableUnique(c, t);

	/*Search all foreign keys referenced to the table*/
	getTableForeignKey(c, t);

	/*Search all exclude constraint referenced to the table*/
	getExcludeConstraint(c, t);

	/*Search all check referenced to the table*/
	getTableCheckConstraint(c, t);

	/*Search all views referenced to the table*/
	getDependentViews(c, t);

	/*get security labels for views*/
	getViewSecurityLabels(c, t);

	printf("OID:[%d] Tabla:[%s]\n", t->oid, t->table);
}


int main(int argc, char const *argv[])
{
	ConfigFile *config = NULL;
	PGROption *opts = NULL;
	PGconn *conn = NULL;

	if (argc > 1)
	{
		if (strcmp(argv[1], "--help") == 0)
		{
			help();
			exit(EXIT_SUCCESS);
		}
		
		if (strcmp(argv[1], "--version") == 0)
		{
			printf(PGR_NAME " " PGR_VERSION "\n"CREATOR"\n"DIR"\n");
			exit(EXIT_SUCCESS);
		}

		if (strcmp(argv[1], "-c") == 0)
		{
			config = loadConfig(argv[2]);
		}
	}


	if(config!=NULL) {

		opts = (PGROption*)malloc(1 * sizeof(PGROption));

		while ((argc > 1)) {

			if (!strcmp(argv[1], "-s")){

				if (argv[2])
					opts->schema = strdup(argv[2]);

			} else if (!strcmp(argv[1], "-t")) {

				if (argv[2])
					opts->table = strdup(argv[2]);

			} else if (!strcmp(argv[1], "-offset")) {

				if (argv[2])
					opts->offset = strdup(argv[2]);

			} else if (!strcmp(argv[1], "-column")) {

				if (argv[2])
					opts->column = strdup(argv[2]);

			} else if (!strcmp(argv[1], "-type")) {

				if (argv[2])
					opts->type = strdup(argv[2]);

			} else if (!strcmp(argv[1], "-file")) {

				if (argv[2])
					config->file = strdup(argv[2]);

			}

			++argv;
			--argc;
		}


		/*
		* database connection is prepared
		*/
		conn = pg_connect(config->host, config->user, config->password, config->dbname, config->port);

		if (config->file != NULL)
		{
			output = fopen(config->file, "w");
			if (!output)
			{
				printf("could not open file \"%s\": %s \n", config->file, strerror(errno));
				exit(EXIT_FAILURE);
			}


			if (existsSchema(conn, opts->schema) == 0)
			{
				printf("schema does not exist : \"%s\"\n", opts->schema);
				exit(EXIT_FAILURE);
			}


			if (existsTable(conn, opts->schema, opts->table) == 0)
			{
				printf("table does not exist : \"%s.%s\"\n", opts->schema, opts->table);
				exit(EXIT_FAILURE);
			}
			

			/*extract the necessary objects for the first phase of the procedure*/
			getTableObjects(conn, opts);

			/*offset cannot be the last column*/
			if (getAttnumOffset(t, opts) == t->nattributes)
			{
				printf("offset cannot be the last column : \"%s\"\n", opts->offset);
				exit(EXIT_FAILURE);
			}

			/*offset must be different from primary keys*/
			if (getAttnumOffset(t, opts) <= 0)
			{
				printf("offset not specified : \"%s\"\n", opts->offset);
				exit(EXIT_FAILURE);
			}


			if (existsColumn(t, opts->column) != 0)
			{
				printf("the new column really exists : \"%s\"\n", opts->column);
				exit(EXIT_FAILURE);
			}

			pgreshape(output, opts);

		} else {
			printf("output file not specified.\n");	
		}

		pg_close(conn);
	} else {
		printf("configuration file not loaded, nothing to do.\n");
	}

	freeConfig(config);
	return 0;
}
