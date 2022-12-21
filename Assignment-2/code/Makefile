OUTFILE = light_sampler

OUTDIR = $(HOME)/cmpt433/public/myApps

CC_C = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: noworky lightSampler

noworky: noworky.c
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow noworky.c -o noworky
	cp noworky $(HOME)/cmpt433/public/myApps/

lightSampler: light_sampler.o joystick.o LED_matrix.o sampler.o common.o analysis.o LED_output.o user_button.o shutdown.o
	$(CC_C) $(CFLAGS) light_sampler.o joystick.o LED_matrix.o sampler.o common.o analysis.o LED_output.o user_button.o shutdown.o -o $(OUTFILE) -pthread
	cp $(OUTFILE) $(HOME)/cmpt433/public/myApps/

light_sampler.o: light_sampler.c
	$(CC_C) $(CFLAGS) -c light_sampler.c

joystick.o: joystick.c
	$(CC_C) $(CFLAGS) -c joystick.c

LED_matrix.o: LED_matrix.c
	$(CC_C) $(CFLAGS) -c LED_matrix.c

sampler.o: sampler.c
	$(CC_C) $(CFLAGS) -c sampler.c

common.o: common.c
	$(CC_C) $(CFLAGS) -c common.c

analysis.o: analysis.c
	$(CC_C) $(CFLAGS) -c analysis.c

LED_output.o : LED_output.c
	$(CC_C) $(CFLAGS) -c LED_output.c

user_button.o : user_button.c
	$(CC_C) $(CFLAGS) -c user_button.c

shutdown.o: shutdown.c
	$(CC_C) $(CFLAGS) -c shutdown.c

clean:
	rm -f *.o $(OUTFILE)