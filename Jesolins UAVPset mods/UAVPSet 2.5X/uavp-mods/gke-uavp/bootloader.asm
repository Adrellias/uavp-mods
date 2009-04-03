        title   "BootLoader.asm"
;*************************************************************************
; BootLoader
; mit PIC16F876
;*************************************************************************

; BOARD_3_0 or BOARD_3_1 must be defined by commadn line!
        LIST C=200,R=dec

        errorlevel -302         ;avoid "ensure page bits are set" warning
        errorlevel -219         ;avoid "invalid RAM location" warning

; config is already defined in main routine!
;        __config  3F72h
        

; Version
; 1.0 .... Erste Version

        include "p16f876.inc"
		include "general.asm"

; RAM locations
Flags	equ		70h
_IsNull	equ		0		;set if reset vect is stored
Data1	equ		71h
AdrLo	equ		72h
AdrHi	equ		73h
Reset	equ		74h		;6 bytes!
ByteCnt	equ		7ah
ChkSum	equ		7bh
LEDs	equ		7ch

DatBuf	equ		0A0h	;80 byte data buffer

#ifdef NADA
; This section not needed here!
		org		0h
		movlw	high TextTab	;NICHT Bootstart!
		movwf	PCLATH
		goto	BootStart
#endif

_B38400	equ		25

		org		1F00h

		global	BootStart

BootStart
; init RS232
		bsf		STATUS,RP0
		DII
;		clrf	OPTION_REG
		movlw	10100100b	; set RC6 as output (TxD), LEDs
		movwf	TRISC
		clrf	TRISB		;all output
		movlw	_B38400
		movwf	SPBRG
		movlw	00100100b	;async mode, BRGH = 1
		movwf	TXSTA
		clrf	STATUS		;clears RP0 & IRP also!

		movlw	10010000b	;receive async on
		movwf	RCSTA
#ifdef BOARD_3_0
		movlw	10000000b
		movwf	PORTB		;SERVOS OFF, only LISL_CS=1
#endif
#ifdef BOARD_3_1
 		clrf	PORTB		;SERVOS OFF!
#endif

;		clrf	Reset+2		;=nop
;		clrf	Reset+3
;		clrf	Reset+4		;=nop
;		clrf	Reset+5

		movlw	Txt_Hello
		call	SendString

		clrf	Flags
MainLoop
; Read a complete string line
		clrf	STATUS
		movlw	DatBuf
		movwf	FSR

#ifdef BOARD_3_0
		movlw	00000001b
		movwf	PORTC		;all outputs off, only yellow LED on
#endif
#ifdef BOARD_3_1
		movlw	00100000b	;yellow LED on, green off
		call	BootLeds
#endif

; wait for a char to receive via RS232
ML_1
		btfss	PIR1,RCIF
		goto	ML_1
; a byte has been read
		movf	RCREG,w

		btfsc	RCSTA,OERR
		goto	ML_err
		btfsc	RCSTA,FERR
		goto	ML_err

; check for : 0-9 A-F CR LF
		movwf	INDF		;store char in buffer
		sublw	':'
		beq		ML_3		;colon is ok

		swapf	INDF,w
		andlw	0x0f		;check for 0x30..0x4F
		addlw	-3
		beq		ML_3
		addlw	-1
		beq		ML_3

		movlw	0x0a
		subwf	INDF,w		;check for LF
		beq		ML_2		;ready!
		
		addlw	0x0a-0x0d	;check for CR
		bne		ML_err
ML_3
		incf	FSR,f
		goto	ML_1		;go receive next char

ML_2
; the complete line inclusive LF is in DatBuf
		movlw	DatBuf	
		movwf	FSR			;scan data buffer

		movf	INDF,w
		sublw	':'			;first char must be a colon
		bne		ML_err

		incf	FSR,f
		clrf	ChkSum

		call	Asc2Bin
		movwf	ByteCnt		;number of bytes to prog
		clc
		rrf		ByteCnt,f	;number of words to prog

		sublw	0x20		;maximum 32 bytes on line
		bcc		ML_err		;count too big!

		call	Asc2Bin
		movwf	AdrHi		;address high byte
		call	Asc2Bin
		movwf	AdrLo		;address low byte

		call	Asc2Bin		;get record type
		addlw	0
		beq		ML_6		;a data record
		addlw	-1
		beq		ML_ready	;finished record
; else: wrong record type!

; disable, then re-enable serial port to clear errors
ML_err		
		bcf		RCSTA,CREN
		movlw	Txt_Err
		call	SendString
		bsf		RCSTA,CREN
		goto	MainLoop
	
ML_6
		clrf	STATUS
#ifdef BOARD_3_0
		bsf		PORTC,1		;green LED on
#endif
#ifdef BOARD_3_1
		movlw	00101000b	;yellow LED on, green on
		call	BootLeds	;exit is always C=0, Z=1 due to loop end
		clc
#endif
		bsf		STATUS,RP1
		rrf		AdrHi,w		;must divide addr by 2 to get word addr
		movwf	EEADRH
		rrf		AdrLo,w
		movwf	EEADR
; do not program words with addresses >= 1F00
		movlw	0x3E		;=(high BootStart)*2
		subwf	AdrHi,w
		bcs		ML_nxrec	;ignore this record	
 
; Only ROM memory records can make it here!
; data prog loop
ML_7	
		call	Asc2Bin		;get lo byte
		movwf	EEDATA
		call	Asc2Bin		;get hi byte
		movwf	EEDATH		

; check if address is 0
; program the reset vector to point to the bootloader
; reprogram reset vector only after successful program
		movf	EEADRH,f
		bne		ML_notres

		movf	EEADR,f
		bne		ML_Vect2

; YES, remember correct reset sequence
		movwf	Reset		;EEDATH is already in WREG!
		movf	EEDATA,w
		movwf	Reset+1

		bsf		Flags,_IsNull
; program my own start sequence, in case programming fails
		movlw	0x1f
		movwf	EEDATA
		movlw	0x30		;=movlw high BootStart
		call	ProgWordH

		clrf	EEDATH
		movlw	0x8A		;=movwf PCLATH
		movwf	EEDATA
		call	ProgWord

		movlw	low BootStart
		movwf	EEDATA
		movlw	0x28 + 0x07 ;= +(high BootStart & 0x07)
		call	ProgWordH

		movlw	2			;backup 2 bytes
		subwf	EEADR,f
		goto	ML_notres2

; do not program address 0x0001 and 0x0002
; only remember the words in RAM
; this is to avoid overprogramming my own start sequence

ML_Vect2
#ifdef NADA	;not necessary here!
		decf	EEADR,w
		bne		ML_Vect3
; YES, remember correct reset vector
		movf	EEDATH,w
		movwf	Reset+2
		movf	EEDATA,w
		movwf	Reset+3
		goto	ML_notres4

ML_Vect3
		addlw	-1
		bne		ML_notres
; YES, remember correct reset vector
		movf	EEDATH,w
		movwf	Reset+4
		movf	EEDATA,w
		movwf	Reset+5
	
ML_notres4
		incf	EEADR,f		;addr 0 to 2, no need to check EEADRH!
		goto	ML_notres2
#endif 	; NADA	not necessary here!

ML_notres
		call	ProgWord
; proceed to next address

ML_notres2
		decfsz	ByteCnt,f
		goto	ML_7		;do next word		

; now check if checksum was correct
		call	Asc2Bin		;checksum of file
		movf	ChkSum,w	;computed checksum
		movlw	Txt_CSum		;assume error
		skipnz				;if sum = 0 -> OK!
ML_nxrec
		movlw	Txt_OK		;muss so bleiben!!!
		call	SendString
		goto	MainLoop

				
; end-record has been read. Re-program the reset vector
ML_ready
		call	SendString_OK

		btfss	Flags,_IsNull
		goto	ML_Done

		bsf		STATUS,RP1
		clrf	EEADR		;set address to the reset vector 0x0000
		clrf	EEADRH	

		movf	Reset+1,w
		movwf	EEDATA
		movf	Reset+0,w
		call	ProgWordH

		movlw	0xFF
		movwf	EEDATA
		movlw	0x3F
		call	ProgWordH

		call	ProgWord

ML_Done
		movlw	Txt_Done
		call	SendString
		goto	$			;wait for power down
		
; program a word to the currently selected flash address
; and increments address afterwards
; bank 2 MUST be selected before entrance
; bank 2 is selected after return
ProgWordH
		movwf	EEDATH
ProgWord
		bsf		STATUS,RP0	;bank 3
		bsf		EECON1,EEPGD	;program flash
		bsf		EECON1,WREN		;enable flash writes
		movlw	0x55		;the magic sequence...
		movwf	EECON2
		movlw	0xAA
		movwf	EECON2
		bsf		EECON1,WR	;start write
		nop
		nop
		bcf		STATUS,RP0	;bank 2
		incf	EEADR,f		;increment address
		skipnz
		incf	EEADRH,f
		return

; converts a character [0..9,A..F] at INDF to a nibble 0x00-0x0F
; advances FSR by 1 char!
; does not disturb bank selection
Asc2Nib
		movlw	'0'
		subwf	INDF,w	;0x00..ox16
		btfsc	INDF,6
		addlw	-7
		incf	FSR,f
		return

; convert 2 ascii chars (FSR) to a binary byte
; advances FSR by 2 chars!
; does not disturb bank selection
Asc2Bin
		call	Asc2Nib
		movwf	Data1
		swapf	Data1,f

		call	Asc2Nib
		iorwf	Data1,w
		addwf	ChkSum,f
		return		
		
; send a string of the table, starting at offset W
SendString_OK
		movlw	Txt_OK
SendString
		clrf	STATUS
		movwf	Data1
SS_1
		movf	Data1,w
		call	TextTab
		addlw	0
		bne		SS_2				;end of text?
		movlw	0x0d
		call	SendByte
		movlw	0x0a
		goto	SendByte		;=call and return!
;		return
SS_2
		call	SendByte
		incf	Data1,f
		goto	SS_1

SendByte
		btfss	PIR1,TXIF	;wait until shift register is empty
		goto	SendByte
		movwf	TXREG		;send char
		return

#ifdef BOARD_3_1
; Bank 0 must be active!
BootLeds
		movwf	LEDs
		movlw	8
BL_Lp1
		rlf		LEDs,f
		bcf		PORTC,4		;clear SDA
		skipnc
		bsf		PORTC,4		;set SDA
		bsf		PORTC,3		;set SCLK
		bcf		PORTC,3		;clr SCLK
		addlw	-1
		bne		BL_Lp1
		bsf		PORTC,1		;RCLK on
		bcf		PORTC,1		;RCLK off
;		return
; fall through with w=0 to save code		
#endif


TextTab
		addwf	PCL,f
_Hello
		dt		"Boot 1.1\0"
_Err
		dt		"ERR\0"
_CSum
		dt		"CSUM\0"
_OK
		dt		"OK\0"
_Done
		dt		"SUCCESS!\0"

Txt_Hello	equ	_Hello-TextTab-1
Txt_Err		equ	_Err-TextTab-1
Txt_CSum	equ	_CSum-TextTab-1
Txt_OK		equ	_OK-TextTab-1
Txt_Done	equ	_Done-TextTab-1

		END
