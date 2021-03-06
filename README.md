# Up-Lang

Up is a programming language with Python like syntax, static typing and is transpiled in C.

## Up syntax

More details in [this file](syntax.md)

```
# This is an inline comment

# Import up modules (import the file ./mymodule.up)
use mymodule

# Wrapper to import libc c headers
use libc

# Create a variable with type int
int a = 42

# Create a variable with automatically deduced type (num here)
$b = 38.2

# Some operations
a++
a += 8

# Condition
a > 0 ?
    # Function call
    exit()

# Else if
or a == b ?
    fun()

# Else
or
    fun()

# Loops
for i to 618
    log(i)
for i=3 to 8
    ...
for i=10 to 1
    ..

# yes means true
while yes
    ...

# Functions
int myFunc(int a, int b)
    ...
    ret 42

# C compatibility
# cdef declares an extern C function
cdef num cosf(num)

%{
    // This code is in the global scope
    int myNb = 128;
%}

# Objects
# To declare objects, see syntax.md
$o = MyObject(618)
o.add(48)
```

## Depedencies

Up targets linux platforms, some programs are required to compile Up :

- make
- gcc
- g++
- flex
- bison

## Mechanism

There are two processes : *Parsing* and *compiling*.
Parsing serves to understand program semantics, it creates components.
Compiling is used to gather all those components in one string, the program.

### Parsing

We use bison and flex to parse, flex is used to tokenize the text and
bison to analyze semantics. The class *Scanner* handle this process.

### Compiling

To compile multiple components are used :
- Variable : Has a type and an identifier

## Usage

To compile a file :

```sh
# Display to stdout the C code
up <entry.up>
# Transpile to C
up <entry.up> <out.c>
# Compile to Bin
up <entry.up> <out>
```

To run the test file located at test/main.up :

```sh
make test
```

## Example

Here is a small example :

```python
# To have functions from stdio.h, stdlib.h and math.h
use libc

# $ means auto type
$a = 42

# num stands for floating point
num b = -618.32

# Booleans are set with yes or no
bool c = yes
$yesOrNo = no
$x = 'I am a string'

# Declare variable without initialization
$nil c

# Create function
nil func()
    a = b
    b = 3.14

# Operations
b = 8
a += --b

# Functions
printf('Result : %f\n', cosf(3.14))
```

Result :

```c
// Code generated by the Up compiler

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

void func() {
    a = b;
    b = 3.14f;
}

int main() {
    int a = 42;
    float b = -618.32f;
    uint8 c = true;
    uint8 yesOrNo = false;
    const char* x = "I am a string";
    void c;
    b = 8;
    a += --b;
    printf("Result : %f\n", cosf(3.14f));
}
```
