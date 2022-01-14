#ifndef __BITMAP_IMAGE_H
#define __BITMAP_IMAGE_H

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        size_t height;
        size_t width;
        const uint8_t *buffer;

    } bitmap_image;

#ifdef __cplusplus
}
#endif

#endif
