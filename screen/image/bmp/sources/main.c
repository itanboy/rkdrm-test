#include "drm-core.h"

uint32_t color_table[6] = {RED,GREEN,BLUE,BLACK,WHITE,BLACK_BLUE};


struct bmpfile {
	int fd;
	int real;
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
};

int get_bmp_file(char *filename,struct bmpfile *bf)
{
	int fd_bmp,offset = 0;
	int count;
	int i;
	int word = 0;
	char bmp_head[14];
	int size;
	char *ptr;

	fd_bmp = open(filename,O_RDWR);
	if(fd_bmp <1){
		printf("open file fail\n");
		return -1;
	}

	count = read(fd_bmp,bmp_head,14);
	if(count != 14){
		printf("read file fail\n");
		return -1;
	}

	if(bmp_head[0] == 0x42 &&  bmp_head[1] == 0x4d){
		bf->real = 1;
		bf->fd = fd_bmp;
		offset += 2;
		memcpy(&bf->file_size,bmp_head+offset,4);
		offset += 8;		//中间有四个字节的空白区域
		memcpy(&bf->bmp_offset,bmp_head+offset,4);
		ptr = malloc(bf->bmp_offset);
		offset = 0;

		count = read(fd_bmp,ptr,bf->bmp_offset-14);
		if(count != bf->bmp_offset-14){
			printf("read file fail\n");
		return -1;
		}
		memcpy(&bf->biSize,ptr+offset,4);
		offset += 4;
		memcpy(&bf->biWidth,ptr+offset,4);
		offset += 4;
		memcpy(&bf->biHeight,ptr+offset,4);
		offset += 4;
		memcpy(&bf->biPlanes,ptr+offset,2);
		offset += 4;
		memcpy(&bf->biBitCount,ptr+offset,2);
		offset += 4;
		memcpy(&bf->biCompression,ptr+offset,4);
		offset += 4;
		memcpy(&bf->biSizeImage,ptr+offset,4);
		offset += 4;
		memcpy(&bf->biXPelsPerMeter,ptr+offset,4);
		offset += 4;
		memcpy(&bf->biYPelsPerMeter,ptr+offset,4);
		offset += 4;
		memcpy(&bf->biClrUsed,ptr+offset,4);
		offset += 4;
		memcpy(&bf->biClrImportant,ptr+offset,4);

	}

	else{
		bf->real = 0;
		return -1;
	}
	return	fd_bmp;
}



int main(int argc, char **argv)
{
	int i,j;
	int fd_bmp;
	struct bmpfile cbf;
	drm_init();

	fd_bmp = get_bmp_file("file/cat.bmp",&cbf);
	if(fd_bmp < 0){
		printf("something wrong in bmp file %d\n",fd_bmp);
		return -1;
	}
	printf("file_size = %d\n",cbf.file_size);
	printf("bmp_offset = %d\n",cbf.bmp_offset);
	printf("biSize = %d\n",cbf.biSize);
	printf("biWidth = %d\n",cbf.biWidth);
	printf("biHeight = %d\n",cbf.biHeight);
	printf("biPlanes = %d\n",cbf.biPlanes);
	printf("biBitCount = %d\n",cbf.biBitCount);
	printf("biCompression = %d\n",cbf.biCompression);
	printf("biSizeImage = %d\n",cbf.biSizeImage);
	printf("biXPelsPerMeter = %d\n",cbf.biXPelsPerMeter);
	printf("biYPelsPerMeter = %d\n",cbf.biYPelsPerMeter);
	printf("biClrUsed = %d\n",cbf.biClrUsed);
	printf("biClrImportant = %d\n",cbf.biClrImportant);

	drm_exit();	

	return 0;
}