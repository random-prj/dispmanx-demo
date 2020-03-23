#ifndef _DISPMANX_H_
#define _DISPMANX_H_

#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <cstring>

//raspberry specific
#include <bcm_host.h>

using namespace std;

struct DISPMANX_ELEMENT {
    DISPMANX_RESOURCE_HANDLE_T  res_;
    DISPMANX_ELEMENT_HANDLE_T   vc_element_;
    VC_IMAGE_TYPE_T type_;
    uint32_t src_width_;
    uint32_t src_height_;
    uint32_t dst_layer_;
    uint32_t dst_width_;
    uint32_t dst_height_;
    int32_t  dst_x_;
    int32_t  dst_y_;
};

void dispmanx_init();
void dispmanx_element_init(struct DISPMANX_ELEMENT* Element);
struct DISPMANX_ELEMENT dispmanx_element_create(
    VC_IMAGE_TYPE_T type,
    int SrcW, int SrcH, int OutX, int OutY, int OutW, int OutH, int OutLayer );
void dispmanx_element_delete(struct DISPMANX_ELEMENT* Element);
void dispmanx_element_write(struct DISPMANX_ELEMENT* Element, char* Pixels);
void dispmanx_element_move( DISPMANX_UPDATE_HANDLE_T update, struct DISPMANX_ELEMENT* Element, int32_t NewX, int32_t NewY );
DISPMANX_UPDATE_HANDLE_T dispmanx_start_update( int Priority );
void dispmanx_sync( DISPMANX_UPDATE_HANDLE_T Update );

#endif //_DISPMANX_H_
