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

#include <sys/tree.h>


RB_HEAD(devq_list, devq_list_entry);

struct devq_devices;
struct devq_devices_list;
struct devq_monitor;
struct devq_event;


typedef enum {
	DEVQ_ATTACHED = 1U,
	DEVQ_DETACHED,
	DEVQ_NOTICE,
	DEVQ_UNKNOWN
} devq_event_t;

typedef enum {
	DEVQ_DEVICE_UNKNOWN = 1U,
	DEVQ_DEVICE_KEYBOARD,
	DEVQ_DEVICE_MOUSE,
	DEVQ_DEVICE_JOYSTICK,
	DEVQ_DEVICE_TOUCHPAD,
	DEVQ_DEVICE_TOUCHSCREEN
} devq_device_t;

typedef enum {
	DEVQ_CLASS_UNKNOWN = 1U,
	DEVQ_CLASS_INPUT
} devq_class_t;



struct devq_device *devq_device_new_from_fd(int fd);
struct devq_device *devq_device_new_from_full_path(char *full_path);
struct devq_device *devq_device_new_from_dev_path(char *dev_path);
char *devq_device_get_dev_path(struct devq_device *dev);
void devq_device_free(struct devq_device *dev);

void devq_list_init(struct devq_list *list);
int devq_list_insert(struct devq_list *list, const char *name, const char*value);
void devq_list_free(struct devq_list *list);
struct devq_list_entry *devq_list_entry_get_first(struct devq_list *list);
struct devq_list_entry *devq_list_entry_get_next(struct devq_list_entry *ent);
const char *devq_list_entry_get_name(struct devq_list_entry *ent);
const char *devq_list_entry_get_value(struct devq_list_entry *ent);
#endif /* _HAVE_LIBDEVQ_ */
