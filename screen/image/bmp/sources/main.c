#include "drm-core.h"

uint32_t color_table[6] = {RED,GREEN,BLUE,BLACK,WHITE,BLACK_BLUE};


struct bmpfile {
	FILE* fp;
	int fd;
	int file_size;		//文件中图像的大小-4Bytes
	int bmp_offset;		//图像偏移量-4Bytes
	int biSize;			//位图信息头大小-4Bytes
	int biWidth;		//图像宽度-4Bytes
	int biHeight;		//图像高度-4Bytes
	short biPlanes;		//颜色平面书-2Bytes
	short biBitCount;	//每Bit占用位数-2Bytes
	int biCompression;	//数据压缩类型-4Bytes
	int biSizeImage;	//图像数据大小-4Bytes
	int biXPelsPerMeter;//像素/米-4Bytes
	int biYPelsPerMeter;//像素/米-4Bytes
	int biClrUsed;		//调色板索引数-4Bytes
	int biClrImportant;	//索引数目-4Bytes
	unsigned char *mem_buf;		//内存映射，整个文件
	unsigned char *bmp_buf;		//将bmp文件的格式改成图像rgb格式
};

int judge_bmp(char *filename,struct bmpfile *pfd)
{
	int offset = 0;
	int count;
	char file_head[14];

	pfd->fp = fopen(filename,"rb");
	if(pfd->fp == NULL){
		printf("open file fail\n");
		return -1;
	}
	count = fread(file_head,1,14,pfd->fp);
	if(count != 14){
		printf("read file fail\n");
		return -1;
	}

	if(file_head[0] == 0x42 &&  file_head[1] == 0x4d){
		//解析参数
		offset = 2;
		memcpy(&pfd->file_size,file_head+offset,4);
		offset += 8;		//中间有四个字节的空白区域
		memcpy(&pfd->bmp_offset,file_head+offset,4);
		fclose(pfd->fp);
		return 1;
	}
	else{
		fclose(pfd->fp);
		return -1;
	}
		
}

int get_bmp_file(char *filename,struct bmpfile *bf)
{
	
	int i;
	int word = 0;
	char bmp_head[14];
	int size;
	int offset = 14;

	bf->fd = open(filename,O_RDWR);
	if(bf->fd < 0){
		printf("can not openfile\n");
		return -1;
	}

	bf->mem_buf = (unsigned char *)mmap(NULL , bf->file_size, 
						PROT_READ | PROT_WRITE, MAP_SHARED, 
						bf->fd, 0);
	memcpy(&bf->biSize,bf->mem_buf+offset,4);
	offset += 4;
	memcpy(&bf->biWidth,bf->mem_buf+offset,4);
	offset += 4;
	memcpy(&bf->biHeight,bf->mem_buf+offset,4);
	offset += 4;
	memcpy(&bf->biPlanes,bf->mem_buf+offset,2);
	offset += 2;
	memcpy(&bf->biBitCount,bf->mem_buf+offset,2);
	offset += 2;
	memcpy(&bf->biCompression,bf->mem_buf+offset,4);
	offset += 4;
	memcpy(&bf->biSizeImage,bf->mem_buf+offset,4);
	offset += 4;
	memcpy(&bf->biXPelsPerMeter,bf->mem_buf+offset,4);
	offset += 4;
	memcpy(&bf->biYPelsPerMeter,bf->mem_buf+offset,4);
	offset += 4;
	memcpy(&bf->biClrUsed,bf->mem_buf+offset,4);
	offset += 4;
	memcpy(&bf->biClrImportant,bf->mem_buf+offset,4);

	//分配空间，操作指针需要分配空间
	bf->bmp_buf = malloc(bf->biSizeImage);

	//将bmp文件的格式改为RGB适用的格式
	for(i = 0;i<bf->biHeight;i++)
		memcpy( bf->bmp_buf + i*bf->biWidth*3 , bf->mem_buf +(bf->biHeight-1-i)*bf->biWidth*3+bf->bmp_offset,bf->biWidth * 3);

	return 0;
}


int free_bmp_file(struct bmpfile *bf)
{
	munmap(bf->mem_buf, bf->file_size);
	free(bf->bmp_buf);
}

int show_bmp(struct bmpfile *bf)
{
	int i;
	uint32_t word;
	if(bf->biBitCount == 24){
		//显示图片
		for(i=0;i<720*1280;i++){
			word = 0;
			word = ((word | bf->bmp_buf[i*3+2])<<16) | 
					((word | bf->bmp_buf[i*3+1])<<8) | 
					((word | bf->bmp_buf[i*3]));	
			buf.vaddr[i] = word ;
		}
	}
	else{
		printf("unsupport,you can use 24bpp bmp file to test\n");
		return -1;	

	}
	return 0;	
}

void show_bmp_info(struct bmpfile *bf)
{
	//打印文件的内容
	printf("file_size = %d\n",bf->file_size);
	printf("bmp_offset = %d\n",bf->bmp_offset);
	printf("biSize = %d\n",bf->biSize);
	printf("biWidth = %d\n",bf->biWidth);
	printf("biHeight = %d\n",bf->biHeight);
	printf("biPlanes = %d\n",bf->biPlanes);
	printf("biBitCount = %d\n",bf->biBitCount);
	printf("biCompression = %d\n",bf->biCompression);
	printf("biSizeImage = %d\n",bf->biSizeImage);
	printf("biXPelsPerMeter = %d\n",bf->biXPelsPerMeter);
	printf("biYPelsPerMeter = %d\n",bf->biYPelsPerMeter);
	printf("biClrUsed = %d\n",bf->biClrUsed);
	printf("biClrImportant = %d\n",bf->biClrImportant);
}


int main(int argc, char **argv)
{
	int i,j;
	int fd_bmp;
	int ret;
	struct bmpfile cbf;

	if(argc <2 ){
		printf("Wrong use !!!!\n");
		printf("Usage: %s xxx.bmp\n",argv[0]);
		goto fail1;
	}

	//初始化
	ret = drm_init();
	if(ret < 0){
		printf("drm init fail\n");
		return -1;
	}
	ret = judge_bmp(argv[1],&cbf);
	if(ret < 0){
		printf("something wrong in bmp file %d\n",fd_bmp);
		goto fail2;
	}
	ret = get_bmp_file(argv[1],&cbf);
	if(ret < 0){
		printf("something wrong in bmp file %d\n",fd_bmp);
		goto fail2;
	}

	show_bmp_info(&cbf);
	ret = show_bmp(&cbf);
	if(ret < 0){
		printf("show_bmp wrong!\n");
		goto fail2;
	}

	getchar();

	drm_exit();	
	return 0;

fail2: 
	drm_exit();
	free_bmp_file(&cbf);
fail1:	
	printf("Proglem run fail,please check !\n");
	return -1;
}