#include "Canvas.h"

Canvas::Canvas()
{
    ssd1306_128x64_i2c_init();
}

void Canvas::Clear()
{
    ssd1306_clearScreen();
}

void Canvas::AddRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color fill)
{
    ssd1306_setColor(fill);
    ssd1306_drawRect(x1, y1, x2, y2);
}

void Canvas::AddImage(uint16_t x, uint16_t y, const Image * image)
{
    ssd1306_drawBitmap(x, y, image->width, image->height, image->img);
}

void Canvas::AddText(uint16_t x, uint16_t y, uint16_t width, const char *text, Color fill, FontSize size, FontStyle style, Align align)
{
    EFontStyle estyle = (EFontStyle)style;
    uint16_t x2 = x;
    uint16_t width2 = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        uint16_t unicode = ssd1306_unicode16FromUtf8(text[i]);
        SCharInfo charInfo;
        ssd1306_getCharBitmap(unicode, &charInfo);
        width2 += charInfo.width;
    }

    if (align == RIGHT)
    {
        x2 = x + width - width2;
    }
    else if (align == CENTER)
    {
        x2 = x + (width - width2) / 2;
    }

    ssd1306_setColor(fill);
    ssd1306_printFixedN(x2, y, text, estyle, size);
}