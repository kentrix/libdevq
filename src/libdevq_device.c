#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <dirent.h>
//TODO clean up

#if defined(HAVE_LIBPROCSTAT_H)
# include <sys/param.h>
# include <sys/queue.h>
# include <sys/socket.h>
# include <kvm.h>
# include <libprocstat.h>
#endif

#include "libdevq.h"


struct devq_device {
	char *dev_path;
};

static struct devq_device *devq_device_new_common(char *path) {
	
	// Path should be sane
	DBG("new_common from %s\n", path);

	struct devq_device *new_dev;
	char *tmp_str;

	new_dev = malloc(sizeof(struct devq_device));
	if (new_dev == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	tmp_str = malloc(sizeof(char) * strlen(path));
	if (tmp_str != NULL) {
		errno = ENOMEM;
		return NULL;
	}

	strcpy(tmp_str, path);

	new_dev->dev_path = tmp_str;

	

	return new_dev;
	
}

struct devq_device *devq_device_new_from_full_path(char *full_path) {

	int fd, ret;	
	struct stat st;

	fd = open(full_path, O_NOFOLLOW);
	ret = fstat(fd, &st);
	if(ret != 0) {
		return NULL;
	}

	if(!S_ISCHR(st.st_mode)) {
		errno = EBADF;
		return NULL;
	}

	close(fd);

	return devq_device_new_common(full_path);


}

struct devq_device *devq_device_new_from_fd(int fd) {


#if defined(HAVE_LIBPROCSTAT_H)
	int ret;
	struct procstat *procstat;
	struct kinfo_proc *kip;
	struct filestat_list *head;
	struct filestat *fst;
	unsigned int count;
	size_t len;

	ret = 0;
	head = NULL;

	procstat = procstat_open_sysctl();
	if (procstat == NULL)
		return (-1);

	count = 0;
	kip = procstat_getprocs(procstat, KERN_PROC_PID, getpid(), &count);
	if (kip == NULL || count != 1) {
		ret = -1;
		goto out;
	}

	head = procstat_getfiles(procstat, kip, 0);
	if (head == NULL) {
		ret = -1;
		goto out;
	}

	STAILQ_FOREACH(fst, head, next) {
		if (fst->fs_uflags != 0 ||
				fst->fs_type != PS_FST_TYPE_VNODE ||
				fst->fs_fd != fd)
			continue;

		if (fst->fs_path == NULL) {
			errno = EBADF;
			ret = -1;
			break;
		}

		len = strlen(fst->fs_path);

		return devq_device_new_from_fullpath(fst->fs_path, len);
	}

out:
	if (head != NULL)
		procstat_freefiles(procstat, head);
	if (kip != NULL)
		procstat_freeprocs(procstat, kip);
	procstat_close(procstat);

	return NULL;

#else /* !defined(HAVE_LIBPROCSTAT_H) */
	int ret;
	struct stat st;
	struct dirent *ds;
	DIR *dir;

	// fd sanity check
	ret = fstat(fd, &st);
	if(ret != 0) {
		return NULL;
	}

	if(!S_ISCHR(st.st_mode) || st.st_rdev <= (dev_t)0) {
		errno = EBADF;
		return NULL;
	}


	char tmp_path[128];
	char full_path[128];

	devname_r(st.st_rdev, S_IFCHR, tmp_path, sizeof(tmp_path));

	sprintf(full_path, DEV_ROOT "%s", tmp_path);

	DBG("devstat() -> %s\n", full_path);
	return devq_device_new_common(full_path);

#endif
}



struct devq_device *devq_device_new_from_dev_path(char *dev_path) {
	// /dev/
	char *full_path;
	int ret;
	if((malloc(sizeof(char) * strlen(dev_path) + 5)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	strcpy(full_path, "/dev/");
	strcat(full_path, dev_path);

	return devq_device_new_from_full_path(full_path);

}



char *devq_device_get_dev_path(struct devq_device *device) {
	char *ret;
	size_t len = strlen(device->dev_path);

	ret = malloc(sizeof(char) * len);
	if (ret == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	strcpy(ret, device->dev_path);

	return ret;

}

int devq_device_free(struct devq_device *device) {
	free(device->dev_path);
	free(device);
	return 0;
}
