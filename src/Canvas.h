#ifndef CANVAS_H
#define CANVAS_H 1

#include "string.h"
#include "ssd1306.h"
#include "Color.h"
#include "Align.h"
#include "FontSize.h"
#include "FontStyle.h"
#include "Image.h"

class Canvas
{
    public:
        explicit Canvas();

        void Clear();
        void AddRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color fill);
        void AddImage(uint16_t x, uint16_t y, const Image *img);
        void AddText(uint16_t x, uint16_t y, uint16_t width, String text, Color fill,
            FontSize size = X1, FontStyle style = REGULAR, Align align = LEFT);
};

#endif