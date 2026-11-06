;;;;;;;;;;;;;;;;;;;;;;;;;;;; MAIN
IN

POP AX

CALL :FIBONACCI

OUT

HLT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:FIBONACCI

PUSH AX
OUT

PUSH AX
PUSH 0

JE :FIBONACCI_ZERO

PUSH AX
PUSH 1

JE :FIBONACCI_ONE

PUSH AX
PUSH 1
SUB

PUSH AX
PUSH 2
SUB

POP AX
CALL :FIBONACCI

POP BX
POP AX

PUSH BX
CALL :FIBONACCI

ADD

RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:FIBONACCI_ZERO

PUSH 0

RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:FIBONACCI_ONE

PUSH 1

RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;