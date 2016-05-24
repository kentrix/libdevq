#ifndef _HAVE_LIBDEVQ_
#define _HAVE_LIBDEVQ_

#define DEBUG_MODE
#ifdef DEBUG_MODE
# define DBG(fmt, ...) \
	do { \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
	}while(0) 
#else
# define DBG(fmt, ...)
#endif /* DEBUG_MODE */

#define DEV_ROOT "/dev/"



struct devq_devices;
struct devq_devices_list;
struct devq_monitor;
struct devq_event;

typedef enum {
	DEVQ_DEVICE_BLOCK = 1U,
	DEVQ_DEVICE_DRM
} devq_device_t;

struct devq_device *devq_device_new_from_fd(int fd);
struct devq_device *devq_device_new_from_full_path(char *full_path);
struct devq_device *devq_device_new_from_dev_path(char *dev_path);
char *devq_device_get_dev_path(struct devq_device *dev);
void devq_device_free(struct devq_device *dev);

#endif /* _HAVE_LIBDEVQ_ */