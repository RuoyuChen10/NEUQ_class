ORG 0000H
LJMP MAIN
ORG 0100H
MAIN:	SETB P0.0
		ACALL DELAY
		CLR P0.0
		SETB P0.1
		ACALL DELAY
		CLR P0.1
		SETB P0.2
		ACALL DELAY
		CLR P0.2
		SETB P0.3
		ACALL DELAY
		CLR P0.3
		SJMP MAIN
DELAY:	MOV R0,#0FH
DEL1:	MOV R1,#03FH
DEL2:	DJNZ R1,DEL2
		DJNZ R0,DEL1
		RET
END
