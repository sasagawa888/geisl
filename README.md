# GEasy-ISLisp

GEasy-ISLisp(GEISL) is an interpreter and compiler compatible with ISLisp standard.
GEISL is for GPGPU. It runs with CUDA using GPU.
GEISL was written by Kenichi Sasagawa


see [ISLisp](https://en.wikipedia.org/wiki/ISLISP)

# Installation
Change to the git cloned or downloaded Easy-ISLisp directory.

In Linux  type "rm *.o" , "make". And type "sudo make install".

```console
$ rm *.o
$ make
gcc  -O3  -c main.c -o main.o
gcc  -O3  -c function.c -o function.o
nvcc -c extension.c -o extension.o
gcc  -O3  -c syntax.c -o syntax.o
gcc  -O3  -c data.c -o data.o
gcc  -O3  -c gbc.c -o gbc.o
gcc  -O3  -c cell.c -o cell.o
gcc  -O3  -c error.c -o error.o
gcc  -O3  -c bignum.c -o bignum.o
gcc  -O3  -c compute.c -o compute.o
gcc  -O3  -c edit.c -o edit.o
nvcc -c gpgpu.c -o gpgpu.o
nvcc -c kernel.cu -o kernel.o
nvcc  -O3  main.o function.o extension.o syntax.o data.o gbc.o cell.o error.o bignum.o compute.o edit.o gpgpu.o kernel.o -o geisl -lm -ldl -lcublas 
$ sudo make install
[sudo] password for sasagawa:               
install -Dm755 geisl /usr/local/bin/geisl


```

# Invoke

```
geisl 
```

In the Linux version,the REPL is editable. If you do not desire to use the editable REPL, invoke with -r option.

```
geisl -r
```

If you desire to start after reading file, invoke with -l option.
e.g.

```
geisl -l foo.lsp 
```

Other options, see GEISL -h

```
$ geisl -h
List of options:
-c           -- GEISL starts after reading compiler.lsp.
-f           -- GEISL starts after reading formatter.lsp.
-h           -- display help.
-l filename  -- GEISL starts after reading the file.
-r           -- GEISL does not use editable REPL.
-s filename  -- GEISL runs the file with script mode.
-v           -- dislplay version number.
```

# Quit

On REPL type (quit) or Esc+Q

On not editable REPL type (quit) or CTRL+D

# Editable REPL
key-bindings are as follows:

- ctrl+F  or → move right
- ctrl+B  or ← move left 
- ctrl+P  or ↑ recall history older
- ctrl+N  or ↓ recall history newer
- ctrl+A  move to begin of line
- strl+E  move to end of line 
- ctrl+J ctrl+M or return insert end of line
- ctrl+H  or back-space  backspace
- ctrl+D  delete one char
- ctrl+K  kill line from current positon
- ctrl+Y  yank killed line
- Esc Tab completion

# Goal
GEISL aims at easy handling of GPGPU.

# Compiler
EISL has a compiler. it generates GCC code and generates object code.


```
Invoke with -c option
eisl -c

or (import "compiler") in REPL

(compile-file "foo.lsp")

(load "foo.o")

example
~/geisl$ geisl
GEasy-ISLisp Ver0.90
> (import "compiler")
T
> (compile-file "tests/tarai.lsp")
type inference
initialize
pass1
pass2
compiling TARAI 
compiling TARAI* 
compiling FIB 
compiling FIB* 
compiling ACK 
compiling GFIB 
compiling TAK 
compiling LISTN 
compiling TAKL 
compiling CTAK 
compiling CTAK-AUX 
finalize
invoke CC
T
> (load "tests/tarai.o")
T
> (time (tarai 12 6 0))
Elapsed Time(second)=0.025118
<undef>
> 
```

Now, I am adding function using CUDA

see tests/gpu-test.lsp
extended array for float.
```
(defglobal a #2f((1.0 2.0)(3.0 4.0)))
(defglobal b #2f((1.0 2.0)(3.0 4.0)))
(gpu-mult a b)
(gpu-add a b)
```

extended create-array builtin-function.
it can generate array for float. e.g.
```
(create-array '(3000 3000) 'rand 'float)
```

# Functions for debug
- (trace fn1 fn2 ... fn)
- (untrace fn1 fn2 ... fn) or (untrace)
- (backtrace)
- (break)
- (macroexpand-1)

# Extended functions
- (random n) random-integer from 0 to n
- (random-real) random-float-number from 0 to 1
- (gbc) invoke garbage collection.
- (gbc t) display message when invoke GC.
- (gbc nil) not display message when invoke GC.
- (gbc 'copy) change GC method to copying-GC
- (gbc 'm&s) change GC method to mark & sweep
- (heapdump n) display cell dump list from nth address
- (instance n) display instance of nth address
- (defmodule name body) see tests/module.lsp
- (import x) import library. e.g. (import "compiler")
