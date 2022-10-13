#include <stdio.h>
#include <sys/types.h>		//open需要的头文件
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>	        //write
#include <sys/types.h>
#include <sys/mman.h>		//mmap  内存映射相关函数库
#include <stdlib.h>	        //malloc free 动态内存申请和释放函数头文件
#include <string.h> 	
#include <linux/fb.h>
#include <sys/ioctl.h>

//32位的颜色ls 
#define Black 	0x00000000
#define White 	0xffFFFFFF
#define Red 	0xffFF0000
#define Green 	0xff00ff00
#define Blue 	0xff0000ff

int fd;
unsigned int *fb_mem  = NULL;	//设置显存的位数为32位
struct fb_var_screeninfo var;
struct fb_fix_screeninfo fix;
struct fb_con2fbmap con2fb;

int main(void)
{
	unsigned int i;
	int a;
	int ret;

	fd = open("/dev/fb0",O_RDWR);			//打开framebuffer设备
	if(fd == -1){
		perror("Open LCD");
		return -1;
	}

	ioctl(fd, FBIOGET_VSCREENINFO, &var);
	ioctl(fd, FBIOGET_FSCREENINFO, &fix);

	var.xres = var.xres/2;
	var.yres = var.yres/2;
	ioctl(fd, FBIOPUT_VSCREENINFO, &var);

	ioctl(fd, FBIOPAN_DISPLAY, &var);

	printf("xres= %d,yres= %d \n",var.xres,var.yres);
	printf("xres_virtual=%d,yres_virtual=%d\n",var.xres_virtual,var.yres_virtual);
	printf("xoffset=%d,yoffset=%d\n",var.xoffset,var.yoffset);
	printf("\n");
	printf("smem_start= %ld,smem_len= %d \n",fix.smem_start,fix.smem_len);
	printf("type=%d,visual=%d\n",fix.type,fix.visual);
	printf("xpanstep=%d,ypanstep=%d\n",fix.xpanstep,fix.ypanstep);
	printf("mmio_len=%d,mmio_start=%ld\n",fix.mmio_len,fix.mmio_start);


    fb_mem = (unsigned int *)mmap(NULL, 720*1280*4, 		//获取显存，映射内存
			PROT_READ |  PROT_WRITE, MAP_SHARED, fd, 0);   
								  
	if(fb_mem == MAP_FAILED){
		perror("Mmap LCD");
		return -1;	
	}
	//设置屏幕，一定要使用这两个命令，才能让屏幕显示东西
	
	// ioctl(fd, FBIOPAN_DISPLAY, &fb_var);		
	memset(fb_mem,0xee,720*1280*4);		//清屏
	
	ioctl(fd, FBIOGET_VSCREENINFO, &var);
	ioctl(fd, FBIOGET_FSCREENINFO, &fix);

	printf("\n");
	printf("\n");
	printf("xres= %d,yres= %d \n",var.xres,var.yres);
	printf("xres_virtual=%d,yres_virtual=%d\n",var.xres_virtual,var.yres_virtual);
	printf("xoffset=%d,yoffset=%d\n",var.xoffset,var.yoffset);
	printf("\n");
	printf("smem_start= %ld,smem_len= %d \n",fix.smem_start,fix.smem_len);
	printf("type=%d,visual=%d\n",fix.type,fix.visual);
	printf("xpanstep=%d,ypanstep=%d\n",fix.xpanstep,fix.ypanstep);
	printf("mmio_len=%d,mmio_start=%ld\n",fix.mmio_len,fix.mmio_start);

	munmap(fb_mem,720*1280*4); //映射后的地址，通过mmap返回的值	
	close(fd); 			//关闭fb0设备文件
	return 0;			
}
