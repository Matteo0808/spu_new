;;;;;;;;;;;;;;;;;;;;;;;;;;;;; MAIN
IN
POP AX
CALL :FACTORIAL
OUT
HLT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:FACTORIAL

PUSH AX
PUSH 0
JE :FACTORIAL_ZERO

PUSH AX
PUSH 1
JE :FACTORIAL_ONE

PUSH AX
POP BX
PUSH BX
PUSH AX
PUSH 1
SUB
POP AX
CALL :FACTORIAL
MUL

RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:FACTORIAL_ZERO

PUSH 1
RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
:FACTORIAL_ONE

PUSH 1
RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;