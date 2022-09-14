#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

struct drm_device {
	uint32_t width;			//显示器的宽的像素点数量
	uint32_t height;		//显示器的高的像素点数量
	uint32_t pitch;			//每行占据的字节数
	uint32_t handle;		//drm_mode_create_dumb的返回句柄
	uint32_t size;			//显示器占据的总字节数
	uint32_t *vaddr;		//mmap的首地址
	uint32_t fb_id;			//创建的framebuffer的id号
	drmModeConnector *conn;	//图层的宽度
	drmModeRes *res;		//图层的宽度
	int fd;					//图层的宽度
	struct drm_mode_create_dumb create ;	//图层的宽度
 	struct drm_mode_map_dumb map;			//图层的宽度
};

#define COLOR1 0XFF0000
#define COLOR2 0X00FF00
#define COLOR3 0X0000FF

struct drm_device buf;


static int drm_create_fb()
{
	/* create a dumb-buffer, the pixel format is XRGB888 */
	buf.create.width = buf.width;
	buf.create.height = buf.height;
	buf.create.bpp = 32;

	/* handle, pitch, size will be returned */
	drmIoctl(buf.fd, DRM_IOCTL_MODE_CREATE_DUMB, &buf.create);

	/* bind the dumb-buffer to an FB object */
	buf.pitch = buf.create.pitch;
	buf.size = buf.create.size;
	buf.handle = buf.create.handle;
	drmModeAddFB(buf.fd, buf.width, buf.height, 24, 32, buf.pitch,
			   buf.handle, &buf.fb_id);
	
	//每行占用字节数，共占用字节数，MAP_DUMB的句柄
	printf("pitch = %d ,size = %d, handle = %d \n",buf.pitch,buf.size,buf.handle);

	/* map the dumb-buffer to userspace */
	buf.map.handle = buf.create.handle;
	drmIoctl(buf.fd, DRM_IOCTL_MODE_MAP_DUMB, &buf.map);

	buf.vaddr = mmap(0, buf.create.size, PROT_READ | PROT_WRITE,
			MAP_SHARED, buf.fd, buf.map.offset);

	/* initialize the dumb-buffer with white-color */
	memset(buf.vaddr, 0xff,buf.size);

	return 0;
}

static void drm_destroy_fb()
{
	struct drm_mode_destroy_dumb destroy = {};

	drmModeRmFB(buf.fd, buf.fb_id);
	
	munmap(buf.vaddr, buf.size);
	
	destroy.handle = buf.handle;

	drmIoctl(buf.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
}

int drm_init()
{
	uint32_t conn_id;
	uint32_t crtc_id;

	//打开drm设备，设备会随设备树的更改而改变,多个设备时，请留一下每个屏幕设备对应的drm设备
	buf.fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if(buf.fd < 0){
        printf("wrong\n");
        return 0;
    }

	//获取drm的信息
	buf.res = drmModeGetResources(buf.fd);
	crtc_id = buf.res->crtcs[0];
	conn_id = buf.res->connectors[0];
	//打印CRTCS,以及conneter的id
	printf("crtc = %d , conneter = %d\n",crtc_id,conn_id);

	buf.conn = drmModeGetConnector(buf.fd, conn_id);
	buf.width = buf.conn->modes[0].hdisplay;
	buf.height = buf.conn->modes[0].vdisplay;

	//打印屏幕分辨率
	printf("width = %d , height = %d\n",buf.width,buf.height);

	//创建framebuffer层
	drm_create_fb();

	//设置CRTCS
	drmModeSetCrtc(buf.fd, crtc_id, buf.fb_id,
			0, 0, &conn_id, 1, &buf.conn->modes[0]);

	return 0;
}

int drm_exit()
{
	drm_destroy_fb();

	drmModeFreeConnector(buf.conn);

	drmModeFreeResources(buf.res);

	close(buf.fd);
}
int main(int argc, char **argv)
{
	int i;
	drm_init();

	//清屏设置颜色
	for(i=0;i<buf.width*buf.height;i++)
		buf.vaddr[i] = 0x123456;

	sleep(2);
	drm_exit();

	exit(0);
}
