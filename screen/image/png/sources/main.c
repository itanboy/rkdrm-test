#include "drm-core.h"
#include <png.h>

#define PNG_FILE 	0X03
#define JPEG_FILE 	0X02
#define BMP_FILE 	0X01 

uint32_t color_table[6] = {RED,GREEN,BLUE,BLACK,WHITE,BLACK_BLUE};

struct png_file{
	char filename[50];
	FILE *fp;				//文件符
	int bpp;				//bits per pixel
	int size;				//图像大小
	int channels; 			//通道
	int rowsize;			//每行占用的字节大小

	int width;				//图像宽度
	int height;				//图像高度
	unsigned char *buffer;	//解压后的图像buffer			
	
	//png 结构体
	png_structp png_ptr;	
	png_infop info_ptr;
};

void free_png(struct png_file *pfd)
{
	free(pfd->buffer);
}

//判断文件是否是png文件
int judge_png(struct png_file *pfd)
{
	int ret;
	char file_head[8]; 
	//读取文件的八个字节
	if (fread(file_head, 1, 8, pfd->fp) != 8) 
		return -1;
	//根据8个字节判断是否为png文件
	ret = png_sig_cmp(file_head, 0, 8); 
	if(ret < 0){
		printf("not a png file\n");
		return ret;
	}

	return PNG_FILE;
}


int decode_png(struct png_file *pfd)
{
	int ret;
	int i, j;
	uint32_t word;
	int iPos = 0;
	png_bytepp pucPngData; 

	//分配和初始化两个libpng相关的结构体
	pfd->png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
	pfd->info_ptr= png_create_info_struct(pfd->png_ptr);
	
	//设置错误返回点
	setjmp(png_jmpbuf(pfd->png_ptr));
	//fseek(fp, 0, SEEK_SET);
	rewind(pfd->fp); 
	//指定文件
	png_init_io(pfd->png_ptr, pfd->fp);

	//获取PNG图像的信息
	png_read_png(pfd->png_ptr, pfd->info_ptr, PNG_TRANSFORM_EXPAND, 0);

	//channels 4-32bits/3-24bits/...
	pfd->channels = png_get_channels(pfd->png_ptr, pfd->info_ptr); 
	pfd->width 	 = png_get_image_width(pfd->png_ptr, pfd->info_ptr);
	pfd->height  = png_get_image_height(pfd->png_ptr, pfd->info_ptr);
	pfd->bpp  = png_get_bit_depth(pfd->png_ptr, pfd->info_ptr) * pfd->channels;
	pfd->rowsize = png_get_rowbytes(pfd->png_ptr, pfd->info_ptr);
	printf("channels = %d\n",pfd->channels);
	printf("bpp = %d\n",pfd->bpp);

	//获取图像的总大小，为图形缓冲区申请空间
	pfd->size= pfd->width * pfd->height*3; 
	pfd->buffer = (unsigned char*)malloc(pfd->size);
	if (NULL == pfd->buffer) {
		printf("malloc rgba faile ...\n");
		png_destroy_read_struct(&pfd->png_ptr, &pfd->info_ptr, 0);
		fclose(pfd->fp);
		return -1;
	}

	//按行一次性获得图像
	pucPngData = png_get_rows(pfd->png_ptr, pfd->info_ptr); 

	//存放buffer区
	for (i = 0; i < pfd->height; i ++) {
		for (j = 0; j < pfd->width*3; j += 3) {
			pfd->buffer[iPos++] = pucPngData[i][j+2];
			pfd->buffer[iPos++] = pucPngData[i][j+1];
			pfd->buffer[iPos++] = pucPngData[i][j+0];
		}
	}
	png_destroy_read_struct(&pfd->png_ptr, &pfd->info_ptr, 0);
	fclose(pfd->fp);
}


int main(int argc, char **argv)
{
	int i,j;
	int ret;
	uint32_t word;
	struct png_file pfd;

	if(argc <2 ){
		printf("Wrong use !!!!\n");
		printf("Usage: %s [xxx.png]\n",argv[0]);
		goto fail1;
	}

	ret = drm_init();	
	if(ret < 0 ){
		printf("drm_init fail\n");
		goto fail1;
	}

		//打开文件
	pfd.fp= fopen(argv[1], "rb");
	if (pfd.fp== NULL) {
		printf("can not open file");
		return -1;
	}

	memcpy(pfd.filename,argv[1],sizeof(argv[1]));

	ret = judge_png(&pfd);
	if(ret<0)
		goto fail2;

	ret = decode_png(&pfd);
	if(ret<0){
		printf("%d",ret);
		goto fail2;
	}

		//显示图像
	for(i=0;i<720*1280;i++){
		word = 0;
		word =  ((word | pfd.buffer[i*3+2])<<16) | 
				((word | pfd.buffer[i*3+1])<<8) | 
				((word | pfd.buffer[i*3]));	
		buf.vaddr[i] = word ;
	}
		
	getchar();
	free_png(&pfd);
	drm_exit();	
	return 0;

fail2:
	drm_exit();
fail1:	
	printf("Proglem run fail,please check !\n");
	return -1;
}