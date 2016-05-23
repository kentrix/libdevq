#ifndef _HAVE_LIBDEVQ_
#define _HAVE_LIBDEVQ_

#define DICT_ENTRIES 2

struct devq_devices;
struct devq_devices_list;
struct devq_monitor;
struct devq_event;

const char walk_dict[DICT_ENTRIES][128] = {
	"/dev", "/dev/dri" }; //TODO Move this to header?

typedef enum {
	DEVQ_DEVICE_BLOCK = 1U,
	DEVQ_DEVICE_DRM
} devq_device_t;

struct devq_device *devq_device_new_from_fd(int fd);

#endif /* _HAVE_LIBDEVQ_ */
