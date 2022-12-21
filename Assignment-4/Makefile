# RUN THIS ON THE HOST!

all: nested-sharedMem pru-copy 

nested-sharedMem:
	make --directory=as4-linux

pru-copy:
	mkdir -p $(HOME)/cmpt433/public/pru/
	cp -r * $(HOME)/cmpt433/public/pru/
	@echo "DO NOT MODIFY THIS FOLDER: COPY ONLY" > $(HOME)/cmpt433/public/pru/COPY_ONLY___DO_NOT_MODIFY
	@echo ""
	@echo "You must build the PRU code on the target, then install it:"
	@echo "(bbg)$$ cd /mount/remote/pru/<your-folder>/"
	@echo "(bbg)$$ make"
	@echo "(bbg)$$ sudo make install_PRU0"
