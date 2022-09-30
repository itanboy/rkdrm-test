#include "drm-core.h"
#include <png.h>

uint32_t color_table[6] = {RED,GREEN,BLUE,BLACK,WHITE,BLACK_BLUE};



struct png_file{

	FILE *fp;
	int Bpp;
	int rgb_size;
	
	int width;
	int height;
	unsigned char *buffer;
	
	png_structp pngstr;
	png_infop pnginfo;
	unsigned char *pucRawData;
	int channels;
};


int decode_jpeg(char *filename,struct png_file *pfd)
{
	int ret;
	char file_head[8]; 
	int i, j;
	uint32_t word;
	int iPos = 0;
	png_bytepp pucPngData; 

	pfd->fp= fopen(filename, "rb");
	if (pfd->fp== NULL) {
		return -1;
	}

	if (fread(file_head, 1, 8, pfd->fp) != 8) 
		return -1;
	
	ret = png_sig_cmp(file_head, 0, 8); 
	if(ret < 0){
		printf("%s not a png file\n",filename);
		return ret;
	}
	//分配和初始化两个libpng相关的结构体
	pfd->pngstr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
	pfd->pnginfo= png_create_info_struct(pfd->pngstr);
	
	//设置错误返回点
	setjmp(png_jmpbuf(pfd->pngstr));
	//fseek(fp, 0, SEEK_SET);
	rewind(pfd->fp); 
	//指定文件
	png_init_io(pfd->pngstr, pfd->fp);

	//获取PNG图像的信息
	png_read_png(pfd->pngstr, pfd->pnginfo, PNG_TRANSFORM_EXPAND, 0); 
	//channels 4-32bits/3-24bits/...
	pfd->channels = png_get_channels(pfd->pngstr, pfd->pnginfo); 
	pfd->width 	 = png_get_image_width(pfd->pngstr, pfd->pnginfo);
	pfd->height  = png_get_image_height(pfd->pngstr, pfd->pnginfo);
	pfd->pixel_depth  =png_get_bit_depth(pfd->pngstr, pfd->pnginfo);
	//逐行读取数据
	pucPngData = png_get_rows(pfd->pngstr, pfd->pnginfo); 
	printf("channels = %d\n",pfd->channels);
	

	pfd->rgb_size= pfd->width * pfd->height*3; 
	pfd->buffer = (unsigned char*)malloc(pfd->rgb_size);
	if (NULL == pfd->buffer) {
		printf("malloc rgba faile ...\n");
		png_destroy_read_struct(&pfd->pngstr, &pfd->pnginfo, 0);
		fclose(pfd->fp);
		return -1;
	}

	for (i = 0; i < pfd->height; i ++) {
		for (j = 0; j < pfd->width*3; j += 3) {
			pfd->buffer[iPos++] = pucPngData[i][j+2];
			pfd->buffer[iPos++] = pucPngData[i][j+1];
			pfd->buffer[iPos++] = pucPngData[i][j+0];
		}
	}
	
	for(i=0;i<720*1280;i++){
		word = 0;
		word =  ((word | pfd->buffer[i*3+2])<<16) | 
				((word | pfd->buffer[i*3+1])<<8) | 
				((word | pfd->buffer[i*3]));	
		buf.vaddr[i] = word ;
	}
	png_destroy_read_struct(&pfd->pngstr, &pfd->pnginfo, 0);
	fclose(pfd->fp);
}


int main(int argc, char **argv)
{
	int i,j;
	int ret;
	struct png_file pf;

	// if(argc <2 ){
	// 	printf("Wrong use !!!!\n");
	// 	printf("Usage: %s [xxx.jpg / xxx.jpeg]\n",argv[0]);
	// 	goto fail1;
	// }

	ret = drm_init();	
	if(ret < 0 ){
		printf("drm_init fail\n");
		goto fail1;
	}

	ret = decode_jpeg("file/png/cat.png",&pf);
	printf("%d",ret);

	getchar();
	drm_exit();	
	return 0;

fail2:
	drm_exit();
fail1:	
	printf("Proglem run fail,please check !\n");
	return -1;
}