#!/bin/bash
for f in `ls -d ../examples/[0-9]*|xargs`; do 
    nm=`echo bgfx-$f|sed 's~../examples/[0-9][0-9]-~~'`;
    echo $nm ...
    pro="${nm}.pro"
    echo "# Generated `date`" > $pro
    echo "" >> $pro
    c=`ls $f/*.c*|xargs`
    echo "SOURCES += $c" >> $pro
    h=`ls $f/*.h*|xargs`
    echo "HEADERS += $h" >> $pro
    echo "" >> $pro
    echo "include(\"common-demo.pri\")" >> $pro
    echo "include(\"common.pri\")" >> $pro
    echo "include(\"QtBgfx/QtBgfx.pri\")" >> $pro
done
