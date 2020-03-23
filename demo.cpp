
#include <iostream>
#include <thread>
#include <chrono>

#include "dispmanx.h"

using namespace std;

char* LoadBitmap ( char *FileName, int *Width, int *Height )
{
	char* buffer = nullptr;
	int imagesize = 0;
	FILE* f = fopen( FileName, "rb");

	if(f == NULL) return NULL;

	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];
	imagesize = width*height *4;
	buffer = (char*)malloc(imagesize);
	if (buffer == NULL)
	{
		fclose(f);
		return 0;
	}
	*Width = width;
	*Height = height;
	cout << "Bitmap " << FileName << " " << width << " " << height << "\n" ;

	int row_padded = (width*3 + 3) & (~3);
	char* data = (char*)malloc(row_padded);

	for(int i = 0; i < height; i++)
	{
		char* dst = buffer+(height-i-1)*width*4;
		fread(data, sizeof(unsigned char), row_padded, f);
		for(int j = 0; j < width; j++ )
		{
			// Convert (B, G, R) to (R, G, B, A)
			char B = data[j*3+0];
			char G = data[j*3+1];
			char R = data[j*3+2];
			if( B==0 && G==255 && R==0 )
			{
				//key-color!!! transparent!!
				dst[j*4+2] = 0;
				dst[j*4+1] = 0;
				dst[j*4+0] = 0;
				dst[j*4+3] = 0;
			}
			else
			{
				//opaque
				dst[j*4+0] = B;
				dst[j*4+1] = G;
				dst[j*4+2] = R;
				dst[j*4+3] = 0xFF;
			}
		}
	}
	free(data);
	fclose(f);
	return buffer;
}

typedef struct OBJ{
	int width_;
	int height_;
	int x_;
	int y_;
	int layer_;
	int speed_;
	char* pixels_;
} OBJ_;

int main(int argc , char *argv[])
{
	cout << "Hello Raspberry DispmanX API!\n";
	dispmanx_init();

	OBJ_ cloud1;
	cloud1.pixels_ = LoadBitmap( (char*)"clouds.bmp", &cloud1.width_, &cloud1.height_ );
	cloud1.layer_ = 100;
	cloud1.x_ = 100;
	cloud1.y_ = 120;
	cloud1.speed_ = 3;

	struct DISPMANX_ELEMENT cloud1_element = dispmanx_element_create(
		VC_IMAGE_ARGB8888, cloud1.width_, cloud1.height_, cloud1.x_, cloud1.y_, cloud1.width_, cloud1.height_, cloud1.layer_ );
	dispmanx_element_write( &cloud1_element, cloud1.pixels_ );

	OBJ_ cloud2;
	cloud2.pixels_ = LoadBitmap( (char*)"clouds.bmp", &cloud2.width_, &cloud2.height_ );
	cloud2.layer_ = 101;
	cloud2.x_ = 10;
	cloud2.y_ = 230;
	cloud2.speed_ = 2;

	struct DISPMANX_ELEMENT cloud2_element = dispmanx_element_create(
		VC_IMAGE_ARGB8888, cloud2.width_, cloud2.height_, cloud2.x_, cloud2.y_, cloud2.width_*1.3, cloud2.height_*1.4, cloud2.layer_ );
	dispmanx_element_write( &cloud2_element, cloud2.pixels_ );

	OBJ_ balls;
	balls.pixels_ = LoadBitmap( (char*)"balls.bmp", &balls.width_, &balls.height_ );
	balls.layer_ = 102;
	balls.x_ = -100;
	balls.y_ = 351;
	balls.speed_ = 5;

	struct DISPMANX_ELEMENT balls_element = dispmanx_element_create(
		VC_IMAGE_ARGB8888, balls.width_, balls.height_, balls.x_, balls.y_, balls.width_, balls.height_, balls.layer_ );
	dispmanx_element_write( &balls_element, balls.pixels_ );

	OBJ_ sun;
	sun.pixels_ = LoadBitmap( (char*)"sun.bmp", &sun.width_, &sun.height_ );
	sun.layer_ = 99;
	sun.x_ = -250;
	sun.y_ = 10;
	sun.speed_ = 1;

	struct DISPMANX_ELEMENT sun_element = dispmanx_element_create(
		VC_IMAGE_ARGB8888, sun.width_, sun.height_, sun.x_, sun.y_, sun.width_, sun.height_, sun.layer_ );
	dispmanx_element_write( &sun_element, sun.pixels_ );

	while(1)
	{
		this_thread::sleep_for( chrono::milliseconds(20) );
		cloud1.x_ += cloud1.speed_;
		if( cloud1.x_>= 1920 )
			cloud1.x_ = 10 - cloud1.width_;

		cloud2.x_ += cloud2.speed_;
		if( cloud2.x_>= 1920 )
			cloud2.x_ = 133 - cloud2.width_;

		balls.x_ += balls.speed_;
		if( balls.x_>= 1920 )
			balls.x_ = 200 - balls.width_;

		sun.x_ += sun.speed_;
		if( sun.x_>= 1920 )
			sun.x_ = 250 - sun.width_;

		DISPMANX_UPDATE_HANDLE_T update = dispmanx_start_update(10);
		dispmanx_element_move( update, &cloud1_element, cloud1.x_, cloud1.y_ );
		dispmanx_element_move( update, &cloud2_element, cloud2.x_, cloud2.y_ );
		dispmanx_element_move( update, &balls_element, balls.x_, balls.y_ );
		dispmanx_element_move( update, &sun_element,   sun.x_,   sun.y_ );
		dispmanx_sync( update );
	}

	cout << "Bye Raspberry DispmanX API!\n";
}

