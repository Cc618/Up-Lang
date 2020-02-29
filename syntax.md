# Up Syntax

This file explains precisely Up's syntax

## Variables

Declare a variable :

```
# Default init
$int a

# Init
int b = 42

# Auto type
$c = 42
```

## Types

Up types are not C types :

| Up Type | Description / Equivalent |
| ------- | ------------------------ |
| int | int32 |
| num | float32 |
| bool | uint8 |
| str | const char* (WIP) |
| nil | void |

## Literals

Here are some literals :

```
$imInt = 42
$imNum = 42.
$imStr = 'wow'
$imBool = yes
$imBool2 = no
```

## Operations

Operations are close to C operations :

```
a++

b -= -1

c /= 42
```

## Control Flow

Conditions are specific to this language :

```
# If
a < b ?
    fun()

# Else if
or a == b ?
    fun()

# Else
or
    fun()
```

Loops are tiny :

```
while yes
    fun()

for i to 42
    fun()

for i=40 to 42
    fun()
```

## Modules

To import modules :

```
use module
```

libc is used to have c headers :

```
use libc
```

## C Compatibility

To declare extern C functions :

```
cdef num cosf(num)
```

To add C sections in global scope :
```
%{
    // C Code
    int myNumber = 128;
%}
```

## LibUp (_WIP_)

LibUp is the up standard library.
LibUp is written in C and Up, the up source is libup/libup.up
and the C source is in libup/src
