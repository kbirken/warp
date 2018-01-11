#!/bin/bash

for i in ./*.dot
do
    echo $i
    dot -Tpng -o${i%%.dot}.png $i
done
