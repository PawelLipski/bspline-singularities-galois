#!/usr/bin/env bash

rm -rf test_meshes
mkdir test_meshes

depths="`seq 1 10`"

for depth in $depths; do
    ./generate -g --edged-4 ${depth} > ./test_meshes/edged-4-depth-${depth}
done

for depth in $depths; do
    ./generate -g --edged-4 ${depth} > ./test_meshes/edged-4-depth-${depth}
done

