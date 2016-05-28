#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libdevq.h"
struct devq_enumerate {
	char *filter;
	struct devq_list dev_list;
};

struct devq_enumerate *devq_enumerate_new(const char *filter) {

	struct devq_enumerate *en;
	char *en_filt;

	en = malloc(sizeof(struct devq_enumerate));

	if(en == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	if(filter == NULL) {
		en->filter = NULL;
		goto out;
	}

	en_filt = malloc(strlen(filter));
	

	if(en_filt == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	strcpy(en_filt, filter);
	en->filter = en_filt;

out:
	DBG("enumerate new:%p. Filter(%s)\n",en, filter == NULL ? "NULL" : filter);
	devq_list_init(&en->dev_list);

	return en;
}

void devq_enumerate_free(struct devq_enumerate *en) {

	DBG("enumberate free: %p", en);

	free(en->filter);
	free(en);
}


