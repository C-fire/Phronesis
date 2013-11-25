#!/bin/bash

f=$1
sed 's/_[0-9]*_/_/g' -i $f;
sed 's/_None//g' -i $f;
sed 's/"[0-9]*_/"/g' -i $f;
