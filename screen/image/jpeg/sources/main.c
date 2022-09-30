#include "drm-core.h"
#include <jpeglib.h>
#include <jerror.h>

#define PNG_FILE 	0X03
#define JPEG_FILE 	0X02
#define BMP_FILE 	0X01 

uint32_t color_table[6] = {RED,GREEN,BLUE,BLACK,WHITE,BLACK_BLUE};


struct jpeg_file{
	char filename[50];
	FILE* fp;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	int width;
	int height;
	int size;
	int row_size;

	uint8_t *pucbuffer;
	uint8_t *buffer;
};

void free_jpeg(struct jpeg_file *jf)
{
	free(jf->pucbuffer);
	free(jf->buffer);
}

//判断文件是否是jpeg文件
int judge_jpeg(char *filename,struct jpeg_file *pfd)
{
	int ret;
	uint32_t word;
	pfd->fp = fopen(filename, "rb");
	if(pfd->fp == NULL){
		printf("can't open %s\n",filename);
		return -1;
	}
    pfd->cinfo.err = jpeg_std_error(&pfd->jerr);
	//创建一个jpeg_compress_struct结构体
    jpeg_create_decompress(&pfd->cinfo);
	
	//指定jpeg解压的源文件
    jpeg_stdio_src(&pfd->cinfo, pfd->fp);
	
	//解析jpeg文件，解析完成后可获得图像的格式
    ret = jpeg_read_header(&pfd->cinfo, TRUE);
	if(ret < 0){
		printf("file is not jpg ...\n");
		return -1;
	}
	memcpy(pfd->filename,filename,sizeof(filename));
	return JPEG_FILE;
}

int decode_jpeg(char *filename,struct jpeg_file *jf)
{
	int ret;
	uint32_t word;

	jf->cinfo.scale_num = 1;
	jf->cinfo.scale_denom = 1;

	//对cinfo所指定的源文件进行解压，并将解压后的数据存到cinfo结构体的成员变量中。
    jpeg_start_decompress(&jf->cinfo);
 
    jf->row_size = jf->cinfo.output_width * jf->cinfo.output_components;
    jf->width = jf->cinfo.output_width;
    jf->height = jf->cinfo.output_height;

    jf->size = jf->row_size * jf->cinfo.output_height; 
	jf->pucbuffer = malloc(jf->row_size);
    jf->buffer = malloc(jf->size);
    
    printf("size: %d w: %d h: %d row_size: %d \n",
			jf->size,jf->width,jf->height,jf->row_size);

    while (jf->cinfo.output_scanline < jf->cinfo.output_height){
        //可以读取RGB数据到buffer中，参数3能指定读取多少行
		jpeg_read_scanlines(&jf->cinfo, &jf->pucbuffer, 1);
        //复制到内存
        memcpy(jf->buffer + jf->cinfo.output_scanline * jf->row_size, jf->pucbuffer, jf->row_size);
    }
	// 完成解码
	jpeg_finish_decompress(&jf->cinfo);
	//释放结构体
    jpeg_destroy_decompress(&jf->cinfo);

	return 0;
}

int main(int argc, char **argv)
{
	int i,j;
	uint32_t word;
	int ret;
	struct jpeg_file jf;

	if(argc <2 ){
		printf("Wrong use !!!!\n");
		printf("Usage: %s [xxx.jpg / xxx.jpeg]\n",argv[0]);
		goto fail1;
	}

	ret = drm_init();	
	if(ret < 0 ){
		printf("drm_init fail\n");
		return -1;
	}
	ret =judge_jpeg(argv[1],&jf);
	if(ret < 0 ){
		goto fail2;
	}

	ret = decode_jpeg(argv[1],&jf);
	if(ret < 0 ){
		printf("decode_jpeg wrong\n");
		goto fail2;
	}

	for(i=0;i<720*1280;i++){
		word = 0;
		word =  ((word | jf.buffer[i*3])<<16) | 
				((word | jf.buffer[i*3+1])<<8) | 
				((word | jf.buffer[i*3+2]));	
		buf.vaddr[i] = word ;
	}
	sleep(3);
	
    fclose(jf.fp);
	free_jpeg(&jf);
	drm_exit();	
	return 0;

fail2:
	fclose(jf.fp);
	free_jpeg(&jf);
	drm_exit();
fail1:	
	printf("Proglem run fail,please check !\n");
	return -1;
}