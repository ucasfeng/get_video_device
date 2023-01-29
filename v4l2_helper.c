#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <ctype.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include "v4l2_helper.h"

void v4l2_helper_get(V4l2Camera* c){
  c->num = 0;
  DIR *dp = 0;
  if(0==(dp=opendir("/dev"))){
    return; 
  }
  
  struct dirent *ep = 0;
  while((ep=readdir(dp))){
    if(memcmp(ep->d_name, "video", 5) || !isdigit(ep->d_name[5])){
      continue;
    }
    if(c->num>=MAX_DEVICE) break;
    
    char rp[PATH_MAX] = "/dev/";
    strcat(rp, ep->d_name);
    struct v4l2_capability vcap;
    int fd = open(rp, O_RDWR);
    if(fd<0) continue;
    int err = ioctl(fd, VIDIOC_QUERYCAP, &vcap);
    if(err) continue;
    if(!(vcap.device_caps & V4L2_CAP_VIDEO_CAPTURE)) continue;

    // a valid video capture device.
    int i = c->num++;
    strcpy(c->device[i].path, rp);
    strcpy(c->device[i].name, vcap.card);
    // formats
    struct v4l2_fmtdesc fmt;
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while(0==(ioctl(fd, VIDIOC_ENUM_FMT, &fmt))){
      // framesize
      struct v4l2_frmsizeenum fs;
      fs.index = 0;
      fs.pixel_format = fmt.pixelformat;
      while(0==(ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fs))){
        if(fs.type!=V4L2_FRMSIZE_TYPE_DISCRETE) continue;
        // interval
        struct v4l2_frmivalenum fi;
        fi.index=0;
        fi.pixel_format = fmt.pixelformat; 
        fi.width = fs.discrete.width;
        fi.height = fs.discrete.height;
        while(0==(ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fi))){
          if(fi.type!=V4L2_FRMIVAL_TYPE_DISCRETE) continue;
          int j = c->device[i].num++;
          strcpy(c->device[i].format[j].name, fmt.description);
          c->device[i].format[j].width = fi.width;
          c->device[i].format[j].height = fi.height;
          c->device[i].format[j].fps_d = fi.discrete.denominator;
          c->device[i].format[j].fps_n = fi.discrete.numerator;
          fi.index++;
        }
        fs.index++;
      }
      fmt.index++; 
    }
    close(fd);
  }
}

void v4l2_helper_print(V4l2Camera* c) {
  int i,j;
  for(i=0;i<c->num;i++){
    printf("device: %s %s\n",c->device[i].name, c->device[i].path);  
    for(j=0;j<c->device[i].num;j++){
      printf("%s %dx%d %.3f\n", c->device[i].format[j].name,
                                c->device[i].format[j].width,
                                c->device[i].format[j].height,
                                1.0 * c->device[i].format[j].fps_d / c->device[i].format[j].fps_n);
    }
  }
}

int main(){
  V4l2Camera c;
  v4l2_helper_get(&c); 
  v4l2_helper_print(&c);
  return 1;
}

