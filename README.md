# Virtual Processor

Hello, GitHub! Here is my Virtual Processor!

It's my implementation of assembler and processor.

This repository contains the assembler that processes asm code in byte code that processor works with and the processor that executes instructions.

Virtual processor is based on ints.


## Commands list

0. HLT(doesn't have an arg) - stops the program.
1. IN(doesn't have an arg) - gets a number from console and pushes to the stack.
2. OUT(doesn't have an arg) - puts a number to stdout and pops it from stack.
3. PUSH(int, AX/BX/CX/DX, [int], [AX/BX/CX/DX]) - pushes a number to stack/register/RAM - it depends on argument
4. POP(AX/BX/CX/DX, [int], [AX/BX/CX/DX]) - pops a number from stack to register/RAM.
5. ADD(doesn't have an arg) - pops 2 numbers from stack, adds them and pushes the result to stack.
6. SUB(doesn't have an arg) - pops 2 numbers from stack, subtracts the first from the second, and pushes the result to stack.
7. MUL(doesn't have an arg) - pops 2 numbers from stack, multiplies them and pushes the result to stack.
8. DIV(doesn't have an arg) - pops 2 numbers from stack, divides the second by the first and pushes the result to stack.  
9. POW(int >= 0) - pops a number from stack, raises it in arg's power and pushes the result to stack.
10. SQRT(doesn't have an arg) - pops a number from stack, finds the square root and pushes the result to stack. But processor is based on ints, so SQRT = (int)(real sqrt). You can use MUL to get a fixed-point. For example:
```
PUSH 34
SQRT
OUT
```
    You will have 5, but
```
PUSH 34
MUL 10000
SQRT
OUT
```
    You will have 583, and your accuracy is 100, so the answer is 5.83.
11. JMP(:labelname) - jumps on label :labelname.
12. JA(:labelname) - jumps on label :labelname if the second popped number is above the first popped number.
13. JAE(:labelname) - jumps on label :labelname if the second popped number is above or equal the first popped number.
14. JB(:labelname) - jumps on label :labelname if the second popped number is below the first popped number
15. JBE(:labelname) - jumps on label :labelname if the second popped number is below or equal the first popped number
16. JE(:labelname) - jumps on label :labelname if the second popped number is equal the first popped number
17. JNE(:labelname) - jumps on label :labelname if the second popped number is not equal the first popped number
18. JSUN(:labelname) - jumps on label :labelname only on Sundays.
19. DRAW(1/2/3/4) - draw VRAM
    1. console output bin - "." - if 0 in VRAM; "#" - if !0 in VRAM.
    2. console output char - draw ASCII code in console.
    3. graphic output in raylib window only black and white.
    4. graphic output in raylib window rgb.
20. CALL(:labelname) - call function :labelname.
21. RET(doesn't have an arg) - returns to function's call.
22. FACT(int >= 0) - pops a number from stack, counts factorial and pushes result.
23. FIB(int >= 0) - pops a number from stack, counts fibonacci and pushes result.
   

## How to work with Virtual Processor?(for windows)

1. At first, compile assembler.cpp with:
    *g++ assembler.cpp -o assembler.exe*
2. Start assembler.exe with:
    *./assembler.exe asmfilename.asm*
3. Compile processor.cpp with:
    *g++ processor.cpp -o processor.exe -I"raylib_path" -L"raylib_path" -lraylib -lopengl32 -lgdi32 -lwinmm*
You must have raylib to work with processor.
raylib_path is path to src in raylib folder.
4. Start processor.exe with:
    *./processor.exe*

## Features:

1. Handmade fixed-point accuracy - choose witch one is more comfortable for you.
2. Turing fullness with recursions.
3. Internal work only with binary files - maximum productivity.
4. Strings comparison only with hashes, say no to strcmps - maximum productivity.
5. Strhandler.hpp and stack.hpp written by me.
6. Maximum protection for stack.
7. Error processing, that returns error in main and stops the program softly.
8. Checking invalid operations, f.e. zero division.
9. A lot of options to upgrade virtual processor.
10. Wait for updates!



<details>
<summary>Assembler includes:</summary>

<stdio.h>\
<string.h>\
<assert.h>\
<config.hpp>\
<strhandler.hpp>

</details>
<details>
<summary>Processor includes:</summary>

<stdio.h>\
<assert.h>
<time.h>\
<sys/stat.h>\
<math.h>\
<config.hpp>\
<stack.hpp>\
<raylib.h>

</details>