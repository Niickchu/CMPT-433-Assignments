# Edit this file to compile extra C files into their own programs.
TARGET= as4-morsecode

SOURCES= main.c morsecode.c utilities.c led.c morseApp.c
#SOURCES= main.c audioMixer_template.c led.c utilities.c digits.c joystick.c
#SOURCES= wave_player.c 

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror


# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0 
#      to host  ~/public/asound_lib_BBB/libasound.so
# Copy to just base library:

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB


# -pg for supporting gprof profiling.
#CFLAGS += -pg


all: 
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lpthread


clean:
	rm -f $(OUTDIR)/$(TARGET)
