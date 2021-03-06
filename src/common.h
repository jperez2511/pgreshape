/**--------------------------------------------------------------------------------
 *
 * pgreshape -- Embed the new column according to the desired position in any table
 * in the postgresql database.
 *
 * Copyright (c) 2019, Rafael Garcia Sagastume
 *
 *---------------------------------------------------------------------------------
 */

#ifndef COMMON_H
#define COMMON_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <c.h>

typedef struct PGSecLabel
{
	char	*provider;
	char	*label;
} PGSecLabel;

typedef struct PGForeignKey
{
	char *conname;
	char *condef;
} PGForeignKey;

typedef struct PGExcludeKey
{
	char *conname;
	char *condef;
} PGExcludeKey;

typedef struct PGCheckConstraint
{
	char *conname;
	char *condef;
} PGCheckConstraint;

typedef struct PGUnique
{
	char contype;
	char *tablespace;
	char *schema;
	char *table;
	char *unique_name;
	char *columns;
	char *comment;
} PGUnique;

typedef struct PGIndex
{
	char contype;
	char *tablespace;
	char *schema;
	char *relname;
	char *indexdef;
	char *comment;
} PGIndex;

typedef struct PGSequence
{
	int oid;
	char *nspname;
	char *relname;
	char *comment;
	char *relowner;
	char *relacl;
	char *start_value;
	char *increment;
	char *minvalue;
	char *maxvalue;
	char *cycle_option;
	char *typname;
	/* security labels */
	int nseclabels;
	PGSecLabel *seclabels;
} PGSequence;

typedef struct PGAttribute
{
	int			attnum;
	char		*attname;
	bool		attnotnull;
	char		*atttypname;
	char		*attdefexpr;
	char		*attcollation;
	int			attstattarget;
	char		*attstorage;
	bool		defstorage;
	char		*attoptions;
	char		*comment;
	char		*acl;

	/* security labels */
	int nseclabels;
	PGSecLabel *seclabels;
} PGAttribute;

typedef struct PGView
{
	int oid;
	char *schema;
	char *view;
	char relkind;
	char *view_definition;
	char *relowner;
	char *comment;
	char *acl;
	char *reloptions;
	char *checkoption;
	/* security labels */
	int nseclabels;
	PGSecLabel *seclabels;
} PGView;

typedef struct PGTable
{
	int oid;
	char *schema;
	char *table;
	int nattributes;
	PGAttribute *attributes;
	int nindexes;
	PGIndex *indexes;
	int nunique;
	PGUnique *unique;
	int nforeignkey;
	PGForeignKey *foreignkeys;
	int nviews;
	PGView *views;
	char *primary_keys;
	char *primary_keys_aa;
	char *primary_keys_nn;
	int ncheck;
	PGCheckConstraint *checks;
	int nsequence;
	PGSequence *sequence;
	int nexcludec;
	PGExcludeKey *excludec;

	/* security labels */
	int nseclabels;
	PGSecLabel *seclabels;
} PGTable;

typedef struct PGROption {

	char *schema;
	char *table;
	char *offset;
	char *column;
	char *type;

} PGROption;

char * formatObjectIdentifier(char *s);

#endif