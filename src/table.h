#ifndef TABLE_H
#define TABLE_H

#include "common.h"
#include "acl.h"

PGTable * getTable(PGconn *c, PGROption *opts);
void getPrimaryKeys(PGconn *c, PGTable *t);
int getAttnumOffset(PGTable *t, PGROption *opts);
void getTableAttributes(PGconn *c, PGTable *t);
void getTableIndexes(PGconn *c, PGTable *t);
void getTableUnique(PGconn *c, PGTable *t);
void getTableForeignKey(PGconn *c, PGTable *t);
void dumpDropIndex(FILE *fout, PGTable *t);
void dumpDropUnique(FILE *fout, PGTable *t);
void dumpDropForeignKey(FILE *fout, PGTable *t);
void dumpCreateTempTableBackup(FILE *fout, PGTable *t);
void dumpDropTableColumn(FILE *fout, PGTable *t, PGROption *opts);
void dumpNewColumn(FILE *fout, PGTable *t, PGROption *opts);
void dumpColumnTable(FILE *fout, PGTable *t, PGROption *opts);
void dumpCreateForeignKey(FILE *fout, PGTable *t);
void dumpCreateIndex(FILE *fout, PGTable *t);
void dumpCreateUnique(FILE *fout, PGTable *t);
int existsColumn(PGTable *t, char *column);
void dumpSetNotNullColumnTable(FILE *fout, PGTable *t, PGROption *opts);
void dumpSetCommentColumnTable(FILE *fout, PGTable *t, PGROption *opts);
void dumpAclColumnTable (FILE *fout, PGTable *t, PGROption *opts);
void dumpOptionsColumnTable (FILE *fout, PGTable *t, PGROption *opts);
void getTableCheckConstraint(PGconn *c, PGTable *t);
void dumpDropCheckConstraint(FILE *fout, PGTable *t);
void dumpCreateCheckConstraint(FILE *fout, PGTable *t);
int existsTable(PGconn *c, char *schema, char *table);
void getExcludeConstraint(PGconn *c, PGTable *t);
void dumpDropExcludeConstraint(FILE *fout, PGTable *t);
void dumpCreateExcludeConstraint(FILE *fout, PGTable *t);
void getTableSecurityLabels(PGconn *c, PGTable *t);
void dumpTableSecurityLabels(FILE *fout, PGTable *t);
void getColumnSecurityLabels(PGconn *c, PGTable *t, PGROption *opts);
void dumpColumnSecurityLabels(FILE *fout, PGTable *t);

#endif