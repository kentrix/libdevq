#ifndef _LIBDEVQ_H_
#define _LIBDEVQ_H_

#include <sys/types.h>
#include <sys/sysctl.h>

typedef enum {
	DEVQ_UNKNOWN = 1U,
	DEVQ_ATTACHED,
	DEVQ_DETACHED,
	DEVQ_NOTICE,
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
	//More devices classes? or just input
} devq_class_t;

struct devq_event;
struct devq_enumerate;
struct devq_device;
struct devq_monitor;
struct devq_list;
#define devq_list_foreach(entry, device_list) 
	//

struct devq_device *	devq_device_new_from_fd(int fd);
struct devq_device *	devq_device_new_from_devpath(const char *dev_path);
struct devq_device *	devq_device_new_from_system_name(const char *dev_path);
struct devq_device *	devq_device_ref(struct devq_device *devq_device);
struct devq_device *	devq_device_unref(struct devq_device *devq_device);
int			devq_device_get_devpath(char *path,
				    size_t *path_len);
int			devq_device_get_pciid(int *vendor_id, int *device_id);
int			devq_device_get_drvname(char *driver_name,
				    size_t *driver_name_len);
int 			devq_device_get_userdata(struct devq_device *, void *data);
int			devq_device_set_userdata(struct devq_device *, const void *data);
devq_device_t		devq_device_get_type(struct devq_device *);
devq_class_t		devq_device_get_class(struct devq_device *);
const char *		devq_device_get_system_name(struct devq_device *)
const char *		devq_device_get_path(struct devq_device *);
const char *		devq_device_get_product(struct devq_device *);
const char *		devq_device_get_vendor(struct devq_device *);
const char *		devq_device_get_manufacturer(struct devq_device *);
const char *		devq_device_get_product(struct devq_device *);
const char *		devq_device_get_serial(struct devq_device *);
//What more can we get from a device?

struct devq_monitor *	devq_monitor_new(void);
struct devq_monitor *	devq_monitor_ref(struct devq_monitor *devq_monitor);
struct devq_monitor *	devq_monitor_unref(struct devq_monitor *devq_monitor);
int			devq_monitor_get_fd(struct devq_monitor *devq_monitor);
int			devq_monitor_poll(struct devq_monitor *devq_monitor);
int 			devq_monitor_start_receiving(struct devq_monitor *devq_monitor);
struct devq_device * 	devq_montior_receive_device(struct devq_monitor *devq_monitor);
struct devq_event *	devq_monitor_read(struct devq_monitor *devq_monitor);

struct devq_device *	devq_get_device(struct devq_event *);
devq_event_t		devq_get_type(struct devq_event *);
const char *		devq_dump(struct devq_event *);


struct devq_enumerate * devq_enumerate_new();
struct devq_enumerate * devq_enumerate_ref(struct devq_enumerate *devq_enumerate);
struct devq_enumerate * devq_enumerate_undef(struct devq_enumerate *devq_enumerate);
int 			devq_enumerate_add_match_subtree(struct devq_enumerate *, const char *);
int 			devq_enumerate_add_nomatch_subtree(struct devq_enumerate *, const char *);
int			devq_enumerate_scan_devices(struct devq_enumerate *);
struct devq_list *	devq_enumerate_get_device_list(struct devq_enumerate *);


#endif
