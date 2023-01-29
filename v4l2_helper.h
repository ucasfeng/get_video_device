#ifndef _v4l2_helper_h_
#define _v4l2_helper_h_

#define MAX_DEVICE 32
#define MAX_FORMAT 32 // per device

typedef struct {
  char name[32]; // format name
  int width;
  int height;
  int fps_d; 
  int fps_n; 
}V4l2Format;

typedef struct {
  char path[32]; // device path e: /dev/video0
  char name[32];
  int num; // count of format.
  V4l2Format format[MAX_FORMAT];
}V4l2Device;

typedef struct {
  int num; // count of devices.
  V4l2Device device[MAX_DEVICE];
}V4l2Camera;


void v4l2_helper_get(V4l2Camera*);
void v4l2_helper_print(V4l2Camera*);

#endif
