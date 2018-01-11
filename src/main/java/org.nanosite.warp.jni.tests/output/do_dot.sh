#!/bin/bash

for i in ./*_dot.txt
do
    echo $i
    dot -Tpng -o${i%%_dot.txt}.png $i
done
