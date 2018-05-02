#!/bin/bash

for r in */image_*.png; do
    f="${r%.*}.csv"
    ../spherical_harmonics/src/script/image2sphcoef --image $r --csv $f
done