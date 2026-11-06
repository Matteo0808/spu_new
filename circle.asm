;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; CENTER FINDING
PUSH 1280
POP [0]
PUSH 960
POP [1]

PUSH [0]
PUSH 2
DIV
POP [2]

PUSH [1]
PUSH 2
DIV
POP [3]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;; VRAM_PTR
PUSH 64
POP [4]
;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;; RADIUS
PUSH 100
PUSH 100
MUL
POP [5]
;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; CIRCLE_INEQUALITY

;;;;;;;;;;;;;;;;;;;;;;;;; Y_COORDINATE_INIT
PUSH 0
POP AX
;;;;;;;;;;;;;;;;;;;;;;;;;

:CYCLE_HEIGHT_START;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PUSH AX
PUSH 960

JA :CYCLE_HEIGHT_BREAK

;;;;;;;;;;;;;;;;;;;;;;;;; X_COORDINATE_INIT
PUSH 0
POP BX
;;;;;;;;;;;;;;;;;;;;;;;;;

:CYCLE_WIDTH_START;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PUSH BX
PUSH 1280

JA :CYCLE_WIDTH_BREAK

;;;;;;;;;;;;;;;;;;;;;;;;; COUNT_DISTANCE
PUSH AX
PUSH [3]
SUB
POW 2

PUSH BX
PUSH [2]
SUB
POW 2


ADD
POP CX; DISTANCE
;;;;;;;;;;;;;;;;;;;;;;;;;

PUSH CX; DISTANCE
PUSH [5]; RADIUS

JA :DIST_NBE_RADIUS

;;;;;;; DRAW_WHITE
PUSH [4]; VRAM_PTR
PUSH AX
PUSH [0]
MUL
ADD
PUSH BX
ADD
POP DX

PUSH 67
POP [DX]
;;;;;;;;;

:DIST_NBE_RADIUS

;;;;;;;;;;;;; INCR_X
PUSH BX
PUSH 1
ADD
POP BX
;;;;;;;;;;;;;

JMP :CYCLE_WIDTH_START
:CYCLE_WIDTH_BREAK;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;; INCR_Y
PUSH AX
PUSH 1
ADD
POP AX
;;;;;;;;;;;;;;

JMP :CYCLE_HEIGHT_START
:CYCLE_HEIGHT_BREAK;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DRAW 3

HLT