#ifndef __FONT_H
#define __FONT_H

#include <stdio.h>
#include "bitmap_image.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        size_t line_height;
        const bitmap_image glyphs[];

    } font;

#ifdef __cplusplus
}
#endif

#endif
