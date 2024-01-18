#ifndef IMAGE_H
#define IMAGE_H 1

#include <stdint.h>

struct Image
{
    uint16_t  width;
    uint16_t  height;
    uint8_t * img;
};

#endif