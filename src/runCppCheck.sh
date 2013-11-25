#!/bin/bash

cppcheck -v --enable=all --force $(for i in `find . -type d`; do echo -n "-I $i "; done) *
