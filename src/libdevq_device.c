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
	char *name;
	char *vendor_id;
	char *device_id;
	char *sysctl_node;
	devq_device_t type;
};

static struct devq_device *devq_device_new_common(char *path) {
	
	// Path should be sane

	struct devq_device *new_dev;
	char *tmp_str;

	new_dev = malloc(sizeof(struct devq_device));
	if (new_dev == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	//TODO define a marco perhaps
	tmp_str = malloc(sizeof(char) * strlen(path));
	if (tmp_str != NULL) {
		errno = ENOMEM;
		return NULL;
	}
	strcpy(tmp_str, path);

	new_dev->dev_path = tmp_str;

	//TODO finish this
	

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

	if(!S_ISCHR(st.st_mode)) {
		errno = EBADF;
		return NULL;
	}

	// Use st_inode and st_dev to determine the path?
	// Walk through all of /dev/
	// TODO find devpath from fd
	//

	// Walk only /dev/*/ for da/ada no follow link
	//
	char tmp_path[256];
	int found;
	size_t tmp_path_len;

	found = 0;

	for (int i = 0; i < DICT_ENTRIES; i++) {
		dir = opendir(walk_dict[i]);
		if (dir == NULL) 
			return NULL;

		struct dirent *dirent;

		while((dirent = readdir(dir)) != NULL) {
			struct stat tmp_st;

			if(dirent->d_name[0] == '0' 
				|| dirent->d_type == DT_DIR)
				continue;

			tmp_path_len = strlen(walk_dict[i]);
			strcpy(tmp_path, walk_dict[i]);
			tmp_path[tmp_path_len++] = '/';
			tmp_path[tmp_path_len] = '\0';

			strcpy(tmp_path + tmp_path_len, dirent->d_name);
			tmp_path_len += dirent->d_namlen;
			tmp_path[tmp_path_len] = '\0';

			ret = stat(tmp_path, &tmp_st);
			if (ret != 0)
				continue;

			if (st.st_dev  == tmp_st.st_dev &&
				st.st_ino  == tmp_st.st_ino) {
				found = 1;
				break;
			}
		}
		closedir(dir);
		if(found)
			break;

	}

	if(!found) {
		errno = EBADF;
		return NULL;
	}

	printf("%s\n", tmp_path);
	//found
	return devq_device_new_common(tmp_path);



	return NULL;
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



char *devq_device_get_vendor_id(struct devq_device *device) {
	char *ret;
	size_t len = strlen(device->vendor_id);

	ret = malloc(sizeof(char) * len);
	if (ret == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	strcpy(ret, device->vendor_id);

	return ret;

}

int devq_device_free(struct devq_device *device) {
	free(device->dev_path);
	free(device->vendor_id);
	free(device);
	return 0;
}
