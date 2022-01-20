#!/bin/bash

gcc -fdiagnostics-color=always -g moodtracker.c -o moodtracker `pkg-config --cflags --libs gtk+-2.0`