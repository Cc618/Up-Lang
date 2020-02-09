# Up-Lang

Up is a programming language with Python like syntax, static typing and is transpiled in C.

## Up syntax

```
# This is an inline comment

# Create a variable with type int
int a = 42

# Create a variable with automatically deducted type
$b = 38.2

# Some operations
a++
a += 8

# Condition
a > 0 ?
    # Function call
    exit()

# Loops
for i to 618:
    log(i)
for i=3 to 8:
    ...
for i=10 to 1:
    ..

# yes means true
while yes:
    ...

# Same as while yes
loop:
    ...

# Functions
int MyFunc(int a, int b=6)
    ...
    ret 42
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

### Parsing

We use bison and flex to parse, flex is used to tokenize the text and
bison to analyze semantics. The class *Scanner* handle this process.

### Compiling

To compile, OOP is used :
- Instruction : The smaller component in Compiler
- Variable : Has a type and an identifier
- Function : Gathers a sequence of instructions
- ISerializable : Interface which provide the function serialize that returns the object in text
- Compiler : Gathers main data such as the program in text format and also the scanner
