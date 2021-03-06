/**--------------------------------------------------------------------------------
 *
 * pgreshape -- Embed the new column according to the desired position in any table
 * in the postgresql database.
 *
 * Copyright (c) 2019, Rafael Garcia Sagastume
 *
 *---------------------------------------------------------------------------------
 */
#ifndef ACL_H
#define ACL_H
#define OBTable 0
#define OBSequence 1

#include "common.h"

typedef struct Acl
{
	char *grant;
	char *privileges;
	struct Acl *next;
} Acl;

typedef struct List
{
	Acl	*root;
	Acl	*leaf;
} List;

char * formatAcl(char *s, char *c);
void dumpAcl(FILE *fout, int obj, char *sch, char *name, char *privs, char *grant, char *cols);
void dumpGrant(FILE *fout, int obj, char *sch, char *name, char *acla, char *cols);
Acl *splitAcl(char *a);
List *shapeACL(char *acl);
void freeAclItem(Acl *item);
void freeAclList(List *acl_list);

#endif
