#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

pushd "$SCRIPT_DIR"

echo "" > ../img-cpp/images.h

for filename in ../img-bmp/*.bmp; do
    python bmp2hex.py "../img-bmp/$filename" >> ../img-cpp/images.h
done

popd