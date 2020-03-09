#!/bin/bash

shopt -s globstar
declare -a args

for i in $1/**/*.h*; do
    args+=(-i)
    args+=($i)
done

hpp2plantuml ${args[@]} -o "$1.puml"
