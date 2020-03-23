#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>

#include "dispmanx.h"

using namespace std;

DISPMANX_DISPLAY_HANDLE_T g_display = 0;

//-------------------------------------------------
//dispmanx display manipulation
//-------------------------------------------------

void dispmanx_init()
{
	if( g_display==0 )
	{
		bcm_host_init();
		g_display = vc_dispmanx_display_open( 0 );
	}
}

void dispmanx_close()
{
	vc_dispmanx_display_close(g_display);
}

DISPMANX_UPDATE_HANDLE_T dispmanx_start_update( int Priority )
{
	return vc_dispmanx_update_start( Priority );
}

void dispmanx_sync( DISPMANX_UPDATE_HANDLE_T Update )
{
	vc_dispmanx_update_submit_sync( Update );
}

//-------------------------------------------------
//dispmanx elements manipulation
//-------------------------------------------------

//init
void dispmanx_element_init(struct DISPMANX_ELEMENT* Element)
{
    memset((void*)Element,0,sizeof(struct DISPMANX_ELEMENT));
}

//delete
void dispmanx_element_delete(struct DISPMANX_ELEMENT* Element)
{
	DISPMANX_UPDATE_HANDLE_T update;
	if( Element->vc_element_ || Element->res_)
	{
		update = dispmanx_start_update( 10 );
		if( Element->vc_element_ )
			vc_dispmanx_element_remove( update, Element->vc_element_ );
		if( Element->res_ )
			vc_dispmanx_resource_delete( Element->res_ );
		dispmanx_sync( update );
		memset((void*)Element,0,sizeof(struct DISPMANX_ELEMENT));
	}
}

void dispmanx_element_write(struct DISPMANX_ELEMENT* Element, char* Pixels)
{
	VC_IMAGE_TYPE_T type = Element->type_;
	DISPMANX_UPDATE_HANDLE_T update = dispmanx_start_update( 10 );

	VC_RECT_T rect;
	uint32_t pitch = 0;

	pitch = ALIGN_UP( Element->src_width_*4, 32);
	vc_dispmanx_rect_set(&rect, 0, 0, Element->src_width_, Element->src_height_);

	vc_dispmanx_resource_write_data(
		Element->res_,
		type,
		pitch,
		Pixels,
		&rect );

	dispmanx_sync( update );
}

//create
struct DISPMANX_ELEMENT dispmanx_element_create(
    VC_IMAGE_TYPE_T type,
    int SrcW, int SrcH, int OutX, int OutY, int OutW, int OutH, int OutLayer )
{
	uint32_t img_ptr; //unused
	VC_RECT_T src_rect;
	VC_RECT_T dst_rect;
	DISPMANX_UPDATE_HANDLE_T update;
	VC_DISPMANX_ALPHA_T alpha;
	alpha.flags =
		(DISPMANX_FLAGS_ALPHA_T)(DISPMANX_FLAGS_ALPHA_FROM_SOURCE | DISPMANX_FLAGS_ALPHA_MIX);
	alpha.opacity = 255;
	alpha.mask = 0;

	struct DISPMANX_ELEMENT el;
	el.dst_x_ = OutX;
	el.dst_y_ = OutY;
	el.dst_width_ = OutW;
	el.dst_height_ = OutH;
	el.src_width_ = SrcW;
	el.src_height_ = SrcH;
	el.dst_layer_ = OutLayer;
	el.type_ = type;
	el.res_ = vc_dispmanx_resource_create(type, SrcW, SrcH,  &img_ptr );

	update = dispmanx_start_update( 10 );
	vc_dispmanx_rect_set( &src_rect, 0, 0, SrcW << 16, SrcH << 16 );
	uint32_t OutW_ = ALIGN_UP( OutW, 32 );
	vc_dispmanx_rect_set( &dst_rect, OutX, OutY, OutW_, OutH );

	el.vc_element_ = vc_dispmanx_element_add(
		update,
		g_display,
		OutLayer,
		&dst_rect, el.res_, &src_rect, DISPMANX_PROTECTION_NONE, &alpha, NULL, DISPMANX_NO_ROTATE );

	dispmanx_sync( update );
	return el;
}

void dispmanx_element_move( DISPMANX_UPDATE_HANDLE_T update, struct DISPMANX_ELEMENT* Element, int32_t NewX, int32_t NewY )
{
	//DISPMANX_UPDATE_HANDLE_T update;
	VC_RECT_T src_rect;
	VC_RECT_T dst_rect;
	Element->dst_x_ = NewX;
	Element->dst_y_ = NewY;
	if( Element->vc_element_ )
	{
		vc_dispmanx_rect_set( &src_rect, 0, 0, (Element->src_width_<< 16), (Element->src_height_ << 16));
		vc_dispmanx_rect_set( &dst_rect,
		Element->dst_x_, Element->dst_y_, Element->dst_width_, Element->dst_height_);
		//update = dispmanx_start_update(10);
		vc_dispmanx_element_change_attributes( update,
			Element->vc_element_, 0, //(1<<2),
			Element->dst_layer_,
			0, &dst_rect, &src_rect, 0, DISPMANX_NO_ROTATE);
		//dispmanx_sync( update );
	}
}
