#include "Canvas.h"
#include <Adafruit_PCD8544.h>

#define PIN_RST PB5
#define PIN_CE PA15
#define PIN_DC PB9
#define PIN_DIN PB8
#define PIN_CLK PD6

// WIDTH =  84
// HEIGHT = 48

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_CLK, PIN_DIN, PIN_DC, PIN_CE, PIN_RST);

void Canvas::Begin()
{
    display.begin();
    display.setContrast(60);
    display.setBias(0x04);
    display.clearDisplay();
    display.display();
}

void Canvas::Clear()
{
    display.clearDisplay();
}

void Canvas::AddRect(uint16_t x1, uint16_t y1, uint16_t width, uint16_t heigth, Color fill)
{
    display.fillRect(x1, y1, width, heigth, fill);
}

void Canvas::AddImage(uint16_t x, uint16_t y, const Image * image)
{
    for (uint16_t yy = 0; yy < image->height; yy++)
    {
        for (uint16_t xx = 0; xx < image->width; xx++)
        {
            uint8_t px = image->img[(yy*image->width) + xx];
            display.drawPixel(x+xx, y+yy, px);
        }
    }
}

void Canvas::AddText(uint16_t x, uint16_t y, uint16_t width, String text, Color fill, FontSize size, Align align)
{
    int16_t x2 = 0;
    int16_t y2 = 0;
    uint16_t width2 = 0;
    uint16_t height2 = 0;

    display.setTextColor(fill);
    display.setTextSize(size);
    display.getTextBounds(text, x, y, &x2, &y2, &width2, &height2);

    if (align == A_RIGHT)
    {
        x2 = x + width - width2;
    }
    else if (align == A_CENTER)
    {
        x2 = x + (width - width2) / 2;
    }
    else
    {
        x2 = x;
    }

    display.setCursor(x2, y2);
    display.print(text);
}

void Canvas::Draw()
{
    display.display();
}