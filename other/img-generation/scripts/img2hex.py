#!/usr/bin/env python

import os
from PIL import Image

rootDir = os.path.dirname(os.path.realpath(__file__))

inputDir = os.path.join(rootDir, "..", "img-src")
outputDir = os.path.join(rootDir, "..", "..", "..", "src", "img")

files = os.listdir(inputDir)

for inputFile in files:
    if not inputFile.endswith(".bmp") and not inputFile.endswith(".png") and not inputFile.endswith(".jpg"):
        continue;
    
    inputPath = os.path.join(inputDir, inputFile)

    outputFile = os.path.splitext(os.path.basename(inputPath))[0]
    outputPath = os.path.join(outputDir, "img-"+outputFile+".h")

    img = Image.open(inputPath)
    bmp = img.load()
    size = img.size

    imageName = "IMG_" + (outputFile.upper().replace("-", "_"))

    output = ""\
        "#include <Arduino.h>"\
        "\n#include \"Image.h\""\
        "\n"\
        "\nconst Image " + imageName +" = "\
        +"\n{"\
        +"\n\t"+str(size[0])+", "+str(size[1])+", PROGMEM new uint8_t["+str(size[0]*size[1])+"]"\
        +"\n\t{\n\t\t"\

    for y in range(size[1]):
        for x in range(size[0]):
            rgb = bmp[x, y];
            isDark = ((rgb[0] + rgb[1] + rgb[2]) / 3) < (255/2)
            b1 = "0x00"
            if isDark:
                b1 = "0x01"
            output += b1 + ", "
        output += "\n\t\t"

    output = output[:-5]
    output += "\n\t}"
    output += "\n};\n"

    with open(outputPath, 'w') as f:
        f.write(output)