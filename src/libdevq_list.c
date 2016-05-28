#include <sys/types.h>
#include <sys/tree.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "libdevq.h"

RB_PROTOTYPE(devq_list, devq_list_entry, link, devq_list_entry_cmpr);

struct devq_list_entry {
	RB_ENTRY(devq_list_entry) link;
	char *value;
	char *name;
};


void devq_list_entry_free(struct devq_list_entry *ent) {

	free(ent);
}

void devq_list_init(struct devq_list *list) {

	RB_INIT(list);
}

void devq_list_free(struct devq_list *list) {
	struct devq_list_entry *ent1, *ent2;

	RB_FOREACH_SAFE(ent1, devq_list, list, ent2) {
		RB_REMOVE(devq_list, list, ent1);
		devq_list_entry_free(ent1);
	}

	RB_INIT(list);
}


int devq_list_insert(struct devq_list *list, char const *name, char const *value) {
	struct devq_list_entry *ent, *old_ent;
	size_t namelen, valuelen;
	char *ent_name, *ent_value;

	namelen = strlen(name);
	valuelen = value == NULL ? 0 : strlen(value) + 1;
	
	ent = malloc(sizeof(struct devq_list_entry));

	if(!ent) {
		errno = ENOMEM;
		return -1;
	}

	ent_name = malloc(sizeof(char) * namelen);
	ent_value = malloc(sizeof(char) * valuelen);

	if(ent_name == NULL || ent_value == NULL) {
		errno = ENOMEM;
		return -1;
	}

	strcpy(ent_name, name);
	strcpy(ent_value, value);

	ent->name = ent_name;

	if(valuelen != 0) {
		ent->value = ent_value;
	}
	else {
		ent->value = NULL;
	}
		
	RB_INSERT(devq_list, list, ent);
	return 0;
}

struct devq_list_entry *devq_list_entry_get_first(struct devq_list *list) {

	return RB_MIN(devq_list, list);
}

struct devq_list_entry *devq_list_entry_get_next(struct devq_list_entry *ent) {

	return RB_NEXT(devq_list,, ent);
}

const char *devq_list_entry_get_name(struct devq_list_entry *ent){

	return ent->name;
}


const char *devq_list_entry_get_value(struct devq_list_entry *ent){

	return ent->value;
}


static int devq_list_entry_cmpr (struct devq_list_entry *ent1, struct devq_list_entry *ent2) {

	return strcmp(ent1->name, ent2->name);
}

RB_GENERATE(devq_list, devq_list_entry, link, devq_list_entry_cmpr);

