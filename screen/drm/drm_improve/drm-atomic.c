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
	struct drm_mode_create_dumb create ;	//创建的dumb
 	struct drm_mode_map_dumb map;			//内存映射结构体
};

struct drm_atomic {

	drmModeObjectProperties *props;
	drmModeAtomicReq *req;

	uint32_t blob_id;
	uint32_t property_crtc_id;
	uint32_t property_mode_id;
	uint32_t property_active;
	uint32_t property_fb_id;
	uint32_t property_crtc_x;
	uint32_t property_crtc_y;
	uint32_t property_crtc_w;
	uint32_t property_crtc_h;
	uint32_t property_src_x;
	uint32_t property_src_y;
	uint32_t property_src_w;
	uint32_t property_src_h;
};


drmModeConnector *conn;	//connetor相关的结构体
drmModeRes *res;		//资源
drmModePlaneRes *plane_res;

int fd;					//文件描述符
uint32_t conn_id;
uint32_t crtc_id;
uint32_t plane_id;

#define COLOR1 0XFF0000
#define COLOR2 0X00FF00
#define COLOR3 0X0000FF

struct drm_device buf;
struct drm_atomic da;

static int drm_create_fb(struct drm_device *bo)
{
	/* create a dumb-buffer, the pixel format is XRGB888 */
	bo->create.width = bo->width;
	bo->create.height = bo->height;
	bo->create.bpp = 32;

	/* handle, pitch, size will be returned */
	drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &bo->create);

	/* bind the dumb-buffer to an FB object */
	bo->pitch = bo->create.pitch;
	bo->size = bo->create.size;
	bo->handle = bo->create.handle;
	drmModeAddFB(fd, bo->width, bo->height, 24, 32, bo->pitch,
			   bo->handle, &bo->fb_id);
	
	//每行占用字节数，共占用字节数，MAP_DUMB的句柄
	printf("pitch = %d ,size = %d, handle = %d \n",bo->pitch,bo->size,bo->handle);

	/* map the dumb-buffer to userspace */
	bo->map.handle = bo->create.handle;
	drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &bo->map);

	bo->vaddr = mmap(0, bo->create.size, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, bo->map.offset);

	/* initialize the dumb-buffer with white-color */
	memset(bo->vaddr, 0xff,bo->size);

	return 0;
}

static void drm_destroy_fb(struct drm_device *bo)
{
	struct drm_mode_destroy_dumb destroy = {};

	drmModeRmFB(fd, bo->fb_id);

	munmap(bo->vaddr, bo->size);

	destroy.handle = bo->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
}


static uint32_t get_property_id(int fd, drmModeObjectProperties *props,
				const char *name)
{
	drmModePropertyPtr property;
	uint32_t i, id = 0;

	/* find property according to the name */
	for (i = 0; i < props->count_props; i++) {
		property = drmModeGetProperty(fd, props->props[i]);
		if (!strcmp(property->name, name))
			id = property->prop_id;
		drmModeFreeProperty(property);

		if (id)
			break;
	}

	return id;
}

int drm_init()
{
	
	
	fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);

	res = drmModeGetResources(fd);
	crtc_id = res->crtcs[0];
	conn_id = res->connectors[0];

	drmSetClientCap(fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
	plane_res = drmModeGetPlaneResources(fd);
	plane_id = plane_res->planes[0];

	conn = drmModeGetConnector(fd, conn_id);
	buf.width = conn->modes[0].hdisplay;
	buf.height = conn->modes[0].vdisplay;

	drm_create_fb(&buf);

	drmSetClientCap(fd, DRM_CLIENT_CAP_ATOMIC, 1);

	/* get connector properties */
	da.props = drmModeObjectGetProperties(fd, conn_id,	DRM_MODE_OBJECT_CONNECTOR);
	da.property_crtc_id = get_property_id(fd, da.props, "CRTC_ID");
	drmModeFreeObjectProperties(da.props);

	/* get crtc properties */
	da.props = drmModeObjectGetProperties(fd, crtc_id, DRM_MODE_OBJECT_CRTC);
	da.property_active = get_property_id(fd, da.props, "ACTIVE");
	da.property_mode_id = get_property_id(fd, da.props, "MODE_ID");
	drmModeFreeObjectProperties(da.props);

	/* create blob to store current mode, and retun the blob id */
	drmModeCreatePropertyBlob(fd, &conn->modes[0],
				sizeof(conn->modes[0]), &da.blob_id);

	/* start modeseting */
	da.req = drmModeAtomicAlloc();
	drmModeAtomicAddProperty(da.req, crtc_id, da.property_active, 1);
	drmModeAtomicAddProperty(da.req, crtc_id, da.property_mode_id, da.blob_id);
	drmModeAtomicAddProperty(da.req, conn_id, da.property_crtc_id, crtc_id);
	drmModeAtomicCommit(fd, da.req, DRM_MODE_ATOMIC_ALLOW_MODESET, NULL);
	drmModeAtomicFree(da.req);
}

int drm_exit()
{
	drm_destroy_fb(&buf);
	drmModeFreeConnector(conn);
	drmModeFreePlaneResources(plane_res);
	drmModeFreeResources(res);
	close(fd);
	return 0;
}


int drm_set_plane()
{
	
	/* get plane properties */
	da.props = drmModeObjectGetProperties(fd, plane_id, DRM_MODE_OBJECT_PLANE);
	da.property_crtc_id = get_property_id(fd, da.props, "CRTC_ID");
	da.property_fb_id = get_property_id(fd, da.props, "FB_ID");
	da.property_crtc_x = get_property_id(fd, da.props, "CRTC_X");
	da.property_crtc_y = get_property_id(fd, da.props, "CRTC_Y");
	da.property_crtc_w = get_property_id(fd, da.props, "CRTC_W");
	da.property_crtc_h = get_property_id(fd, da.props, "CRTC_H");
	da.property_src_x = get_property_id(fd, da.props, "SRC_X");
	da.property_src_y = get_property_id(fd, da.props, "SRC_Y");
	da.property_src_w = get_property_id(fd, da.props, "SRC_W");
	da.property_src_h = get_property_id(fd, da.props, "SRC_H");
	drmModeFreeObjectProperties(da.props);

    /* atomic plane update */
	da.req = drmModeAtomicAlloc();
	drmModeAtomicAddProperty(da.req, plane_id, da.property_crtc_id, crtc_id);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_fb_id, buf.fb_id);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_crtc_x, 50);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_crtc_y, 50);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_crtc_w, 320);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_crtc_h, 320);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_src_x, 0);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_src_y, 0);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_src_w, 320 << 16);
	drmModeAtomicAddProperty(da.req, plane_id, da.property_src_h, 320 << 16);
	drmModeAtomicCommit(fd, da.req, 0, NULL);
	drmModeAtomicFree(da.req);

}

int main(int argc, char **argv)
{
	drm_init();
	printf("drmModeAtomicCommit SetCrtc\n");
	getchar();

	drm_set_plane();

	printf("drmModeSetPlane\n");
	getchar();
	drm_exit();	

	return 0;
}
