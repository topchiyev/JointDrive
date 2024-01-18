import sys
from PIL import Image

path = "/Users/topchiyev/3D Print/JointDrive/Firmware/JointDrive/other/img-generation/img-png/intro-screen.png"

#print(path)

img = Image.open(path)
bmp = img.load()
size = img.size
pixels = []

for y in range(size[1]):
    for x in range(size[0]):
        rgb = bmp[x, y];
        isDark = ((rgb[0] + rgb[1] + rgb[2]) / 3) < (255/2)
        b1 = "0x00"
        if isDark:
            b1 = "0x01"
        pixels.append(b1)

output = "[" + str(size[0]) +","+ str(size[1]) + "] {" + ", ".join(pixels) + "};";
print(output)