#include "drm-core.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

int fd_hzk16;
struct stat hzk_stat;
unsigned char *hzkmem;

uint32_t color_table[6] = {RED,GREEN,BLUE,BLACK,WHITE,BLACK_BLUE};

void show_pixel(uint32_t x , uint32_t y , uint32_t color)
{
	if(x > buf.width || y > buf.height){
		printf("wrong set\n");
	}

	buf.vaddr[ y*buf.width + x] = color;
}


void show_8x16(uint32_t x , uint32_t y , uint32_t color, unsigned char num)
{
	int i,j;
	unsigned char dot;
	for(i = 0 ; i<16 ; i++){
		dot = fontdata_8x16[num*16+i];
		for(j=0;j<8;j++){
			if(dot & 0x80)
				show_pixel(x+j,y+i,color);
			else
				show_pixel(x+j,y+i,BLACK_BLUE);
			dot = dot << 1;
		}
	}
}

void show_string(uint32_t color)
{
	int i,j;
	int row=64;
	int x_offset = (buf.width - 64*8)/2;
	int y_offset = (buf.height - 16*4)/2;
	for(j=0;j<4;j++){
		for(i=0;i<64;i++){
			show_8x16(i*8+x_offset,16*j+y_offset,color,i+j*64);
		}
	}
}

int utf_8_to_unicode_word(uint8_t *utf_8,uint8_t *uicode)
{
	int len;
	//1位
	if(utf_8[0]<0x80){
		uicode[0] = 0;
		uicode[1] = utf_8[0];
		len = 1;
		return len;
	}
	//2位
	else if(utf_8[0] > 0xc0 & utf_8[0] <0xe0){
		uicode[1] = (utf_8[1]&0x3f) | ((utf_8[0]<< 6)& 0xc0 );
		uicode[0] = ((utf_8[0]>>2) & 0x07) ;
		return 2;
	}
	//3位
	else if(utf_8[0] > 0xe0 & utf_8[0]<0xf0){
		uicode[1] = (utf_8[2]&0x3f) | ((utf_8[1] << 6)& 0xc0);
		uicode[0] = ((utf_8[1]>>2)&0x0f) | ((utf_8[0] <<4)& 0xf0) ;
		return 2;
	}

	else
		printf("over 2 bytes\n");

}


int utf_8_to_unicode_string(uint8_t *utf_8,uint8_t *uicode)
{
	int len = 0;
	int utf_8_size = strlen(utf_8);
	int utf_8_len = 0;
	int uicode_len = 0;

	printf("size=%d\n",utf_8_size);

	while(utf_8_size > 0){
		//一位
		if(utf_8[utf_8_len] < 0x80){
			uicode[uicode_len] = 0;
			uicode[uicode_len+1] = utf_8[utf_8_len];
			len ++;
			utf_8_len ++;
			utf_8_size--;
			uicode_len =uicode_len + 2;
			continue;
		}
		//2位
		else if(utf_8[utf_8_len] > 0xc0 & utf_8[utf_8_len] <0xe0){
			uicode[uicode_len+1] = (utf_8[utf_8_len+1]&0x3f) | ((utf_8[utf_8_len]<< 6)& 0xc0 );
			uicode[uicode_len] = ((utf_8[utf_8_len]>>2) & 0x07) ;
			len ++;
			utf_8_len +=2;
			utf_8_size -=2;
			uicode_len +=2;
			continue;
		}
		//3位
		else if(utf_8[utf_8_len] > 0xe0 & utf_8[utf_8_len]<0xf0){
			uicode[uicode_len+1] = (utf_8[utf_8_len+2]&0x3f) | ((utf_8[utf_8_len+1] << 6)& 0xc0);
			uicode[uicode_len] = ((utf_8[utf_8_len+1]>>2)&0x0f) | ((utf_8[utf_8_len] <<4)& 0xf0) ;
			len ++;
			utf_8_len +=3;
			uicode_len +=2;
			utf_8_size -=3;
			continue;
		}


	}
	printf("out\n");
	return len;
	

}


void show_chinese(int x, int y, unsigned char *str)
{
	unsigned int area  = str[0] - 0xA1;
	unsigned int where = str[1] - 0xA1;
	unsigned char *dots = hzkmem + (area * 94 + where)*32;
	unsigned char byte;

	int i, j, b;
	for (i = 0; i < 16; i++){
		for (j = 0; j < 2; j++){
			byte = dots[i*2 + j];
			for (b = 7; b >=0; b--){
				if (byte & (1<<b))
					show_pixel(x+j*8+7-b, y+i, WHITE);
				else
					show_pixel(x+j*8+7-b, y+i, BLACK_BLUE); 
			}
		}
	}
}

void draw_bitmap( FT_Bitmap* bitmap,FT_Int x,FT_Int y)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;

	//printf("x = %d, y = %d\n", x, y);

	for ( j = y, q = 0; j < y_max; j++, q++ )
	{
		for ( i = x, p = 0; i < x_max; i++, p++ )
		{
			if ( i < 0      || j < 0       ||
				i >= buf.width || j >= buf.height )
			continue;

			//image[j][i] |= bitmap->buffer[q * bitmap->width + p];
			show_pixel(i, j, bitmap->buffer[q * bitmap->width + p]);
		}
	}
}

int main(int argc, char **argv)
{
	int i,j,count;
	FT_Library	  library;
	FT_Face 	  face;
	int error;
	FT_UInt     charIdx;
    FT_Vector     pen;
	FT_GlyphSlot  slot;
	int font_size = 720;

	wchar_t *chinese_str = L"火";

	// wchar_t *zhong = unicode[2];

	unsigned char str2[] = "中";
	unsigned char str3[] = "野火科技";
	unsigned char str[2] = {0xd6,0xd0}; 
	unsigned short str4 = 0x4e2d;
	uint8_t unicode[2];
	

	// memset(unicode,0,sizeof(unicode));
	drm_init();
	// show_string(WHITE);

	// // show_chinese(0,0,str);
	utf_8_to_unicode_word(str2,unicode);
	printf("%x,%x\n",unicode[0],unicode[1]);
	// utf_8_to_unicode_string(str3,unicode);

	/* 显示矢量字体 */
	error = FT_Init_FreeType( &library );			   /* initialize library */
	/* error handling omitted */
	
	error = FT_New_Face( library, "file/simsun.ttc", 0, &face ); /* create face object */
	/* error handling omitted */	
	slot = face->glyph;

	FT_Set_Pixel_Sizes(face, font_size, 0);

	charIdx = FT_Get_Char_Index(face,str4);
	FT_Load_Glyph(face,charIdx, FT_LOAD_DEFAULT); 

	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

	// error = FT_Load_Char( face, chinese_str[0], FT_LOAD_RENDER );

	draw_bitmap( &slot->bitmap,0,0);
	if (error){
		printf("FT_Load_Char error\n");
		return -1;
	}
	getchar();
	drm_exit();	

	return 0;
}