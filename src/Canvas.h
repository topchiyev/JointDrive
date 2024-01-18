#ifndef CANVAS_H
#define CANVAS_H 1

#include <Arduino.h>
#include "Color.h"
#include "Align.h"
#include "FontSize.h"
#include "Image.h"

class Canvas
{
    public:
        explicit Canvas();

        void Clear();
        void AddRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color fill);
        void AddImage(uint16_t x, uint16_t y, const Image *img);
        void AddText(uint16_t x, uint16_t y, uint16_t width, String text, Color fill,
            FontSize size = FS_X1, Align align = A_LEFT);

        void Draw();
};

#endif