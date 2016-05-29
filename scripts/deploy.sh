#!/bin/sh -x

TARGET=i57pc7.ira.uka.de:htdocs/weigl/software/sharpPI/

rst2man.py README.rst sharpPI.1
rst2html.py --stylesheet=scripts/style.css README.rst index.html

scp build/src/sharpPI scripts/style.css index.html sharpPI.1  $TARGET
