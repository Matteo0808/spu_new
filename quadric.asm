;;;;;;;;;;;;;;;;;;;;; INPUT
IN
POP AX
IN
POP BX
IN
POP CX
;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;; if(a != 0)
PUSH AX
PUSH 0

JE :A_E_ZERO

CALL :A_NE_ZERO

HLT
;;;;;;;;;;;;;;;;;;;;;
:A_E_ZERO



;;;;;;;;;;;;;;;;;;;;; if(b != 0)
PUSH BX
PUSH 0

JE :B_E_ZERO

CALL :B_NE_ZERO

HLT
;;;;;;;;;;;;;;;;;;;;;
:B_E_ZERO



;;;;;;;;;;;;;;;;;;;;; if(c != 0)
PUSH CX
PUSH 0

JE :C_E_ZERO

CALL :C_NE_ZERO

HLT
;;;;;;;;;;;;;;;;;;;;;
:C_E_ZERO



;;;;;;;;;;;;;;;;;;;;; if(a,b,c != 0)
PUSH 8
OUT;                                                                                            INFINITY_NUMBER_OF_ROOTS
HLT
;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;
:A_NE_ZERO

CALL :COUNT_DISCRIMINANT

PUSH DX
PUSH 0

JB :CASE_D_B_ZERO

CALL :COUNT_FIRST_ROOT

CALL :COUNT_SECOND_ROOT

PUSH 2
OUT;                                                                                                 TWO_ROOTS
OUT;                                                                                                 FIRST_ROOT
OUT;                                                                                                 SECOND_ROOT

RET
;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;
:COUNT_DISCRIMINANT

PUSH BX
PUSH BX
MUL

PUSH AX
PUSH CX
MUL
PUSH 4
MUL
SUB
PUSH 10000
MUL
POP DX

RET
;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;
:CASE_D_B_ZERO

PUSH 0
OUT;                                                                                             NO_ROOTS_ZERO_D

HLT
;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;
:COUNT_FIRST_ROOT
PUSH 0
PUSH BX
SUB
PUSH 100
MUL

PUSH DX
SQRT
ADD

PUSH AX
PUSH 2
MUL
DIV

RET
;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;
:COUNT_SECOND_ROOT
PUSH 0
PUSH BX
SUB
PUSH 100
MUL

PUSH DX
SQRT
SUB

PUSH AX
PUSH 2
MUL
DIV

RET
;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;; (a = 0, b != 0, c ? 0) 
:B_NE_ZERO

PUSH 0
PUSH CX
SUB
PUSH BX
DIV
PUSH 1
OUT;                                                                                             ONE_ROOT
OUT;                                                                                             ROOT_1

RET
;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;; (a = 0, b = 0, c != 0)
:C_NE_ZERO
PUSH 0
OUT;                                                                                            NO_ROOTS

RET
;;;;;;;;;;;;;;;;;;;;;