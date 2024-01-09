#!/usr/bin/env python

import sys, argparse
from os.path import splitext, basename, join
from PIL import Image

def main():
    
    # Set up parser and handle arguments
    parser = argparse.ArgumentParser()
    parser.add_argument ("infile", help="The PNG file(s) to convert", type=argparse.FileType('r'), nargs='*', default=['-'])
    parser.add_argument ("outdir", help="output dir", default=['-'])
    args = parser.parse_args()
    
    # Do the work
    for f in args.infile:
        if f == '-':
            sys.exit()
        png2bmp(f.name, args.outdir)
        
def png2bmp(input, outdir):
    output = join(outdir, splitext(basename(input))[0] + ".bmp")
    img = Image.open(input)
    img = img.convert('L')

    img.save(output)
    
    
# Only run if launched from commandline
if __name__ == '__main__': main()