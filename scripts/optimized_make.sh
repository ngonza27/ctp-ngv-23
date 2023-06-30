#!/bin/bash

cd src/
make
strip --remove-section=.comment --remove-section=.note main