#!/bin/bash

# Script to install wxWidgets in Linux

mkdir temp

cd temp

git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git

cd wxWidgets

./configure

sudo make install

cd ..
