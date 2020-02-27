
# TODO : Remove fib

.PHONY: all src test clean fib

all: src

src:
	mkdir -p bin
	cd src && make

# Runs the compiler on the test/main.up file
test: src
	@printf '--- Running up ---\n\n'
	@bin/up test/main.up

clean:
	cd src && make clean
	rm -rf bin

# Compiles the fibonacci example and executes it in the tmp folder
fib: all examples/fibonacci.up
	mkdir -p tmp
	bin/up examples/fibonacci.up > tmp/main.c
	gcc -o tmp/a tmp/main.c
	tmp/a

# TODO : rm
fizz: all examples/fizzbuzz.up
	mkdir -p tmp
	bin/up examples/fizzbuzz.up > tmp/main.c
	gcc -o tmp/a tmp/main.c
	tmp/a
