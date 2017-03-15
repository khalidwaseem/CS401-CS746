#ifndef MY_IOCTL_H
#define MY_IOCTL_H
#include <linux/ioctl.h>

#define MAX_ARRAY_SIZE 10

typedef struct{
	int value;
	int index;
} myioctl_arg_t;

typedef struct{
	int array[MAX_ARRAY_SIZE];
	int size;
}array_arg;

#define MYIOCTL_GET_VALUE _IOR('q', 1, myioctl_arg_t *)
#define MYIOCTL_CLR _IO('q', 2)
#define MYIOCTL_PRINT _IOR('q', 3, array_arg*)
//#define MYIOCTL_PRINT _IO('q', 3)
#define MYIOCTL_SET_VALUE _IOW('q', 4, myioctl_arg_t *)

#endif
