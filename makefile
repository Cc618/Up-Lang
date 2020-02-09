
.PHONY: all src test clean

all: src

src:
	mkdir -p bin
	cd src && make

# Runs the compiler on the test/main.up file
test: src
	bin/up test/main.up

clean:
	cd src && make clean
	rm -rf bin
