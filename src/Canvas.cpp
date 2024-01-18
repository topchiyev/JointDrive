#include "Canvas.h"
#include <Adafruit_PCD8544.h>

#define PIN_RST 2
#define PIN_CE 3
#define PIN_DC 4
#define PIN_DIN 5
#define PIN_CLK 6

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_CLK, PIN_DIN, PIN_DC, PIN_CE, PIN_RST);

Canvas::Canvas()
{
    display.begin();
    display.setContrast(60);
    display.clearDisplay();
    display.display();
}

void Canvas::Clear()
{
    display.clearDisplay();
}

void Canvas::AddRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color fill)
{
    display.fillRect(x1, y1, x2-x1, y2-y1, fill);
}

void Canvas::AddImage(uint16_t x, uint16_t y, const Image * image)
{
    display.drawBitmap(x, y, image->img, image->width, image->height, C_BLACK);
}

void Canvas::AddText(uint16_t x, uint16_t y, uint16_t width, String text, Color fill, FontSize size, Align align)
{
    int16_t x2 = 0;
    int16_t y2= 0;
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
}

void Canvas::Draw()
{
    display.display();
}