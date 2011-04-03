; boot loader for SantaOS
; written by Santa Zhang, 2011
;
; start the cpu, switch to 32-bit protected mode
;
; the bios loads this code from the first sector of the hard disk
; into memory at 0x7c00, and starts executing in real mode,
; with %cs = 0 %ip=7c00

DA_32		EQU	4000h
DA_DRW		EQU	92h
DA_DRWA		EQU	93h
DA_C		EQU	98h

; usage: Descriptor Base, Limit, Attr
;        Base:  dd
;        Limit: dd (low 20 bits available)
;        Attr:  dw (lower 4 bits of higher byte are always 0)
%macro Descriptor 3
	dw	%2 & 0FFFFh
	dw	%1 & 0FFFFh
	db	(%1 >> 16) & 0FFh
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)
	db	(%1 >> 24) & 0FFh
%endmacro



org	07c00h
  jmp	start

[SECTION .gdt]
  LABEL_GDT:	        Descriptor  0,        0,                  0
  LABEL_DESC_CODE32:  Descriptor  0,        SegCode32Len - 1,   DA_C + DA_32
  LABEL_DESC_VIDEO:   Descriptor  0B8000h,  0ffffh,             DA_DRW
  LABEL_DESC_DATA:    Descriptor  0,        DataLen - 1,        DA_DRW
  LABEL_DESC_STACK:   Descriptor  0,        TopOfStack,         DA_DRWA + DA_32

  GdtLen		equ	$ - LABEL_GDT
  GdtPtr		dw	GdtLen - 1
            dd	0

  SelectorCode32	equ	LABEL_DESC_CODE32	- LABEL_GDT
  SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
  SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT
  SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT
; END of [SECTION .gdt]


; boot loader start, 16-bit mode
[section .s16]
[bits 16]
start:
  cli         ; disable interrupts
  cld         ; string operations increment

  ; setup data segment registers (ds, es, ss)
  mov     ax, cs
  mov     ds, ax    ; data segment
  mov     es, ax    ; extra segment
  mov     ss, ax    ; stack segment
  mov     sp, 0100h ; init stack
  
  call    cls16
  call    hello16
  call    hello16direct

  ; enable A20
seta20.1:
  in      al, 0x64  ; wait until not busy
  test    al, 0x2
  jnz     seta20.1

  mov     al, 0d1h  ; send 0d1h -> port 0x64
  out     0x64, al

seta20.2:
  in      al, 0x64  ; wait until not busy
  test    al, 0x2
  jnz     seta20.2

  mov     al, 0xdf  ; send 0xdf -> port 0x60
  out     0x60, al

  ; switch from real mode to protected mode, using a bootstrap gdt
  ; and segmetn translation that makes virtual addr equal to physical
  ; addr, so that the effective memory map does not change during the
  ; switch

	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah
	
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_DATA
	mov	word [LABEL_DESC_DATA + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_DATA + 4], al
	mov	byte [LABEL_DESC_DATA + 7], ah
	
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_STACK
	mov	word [LABEL_DESC_STACK + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_STACK + 4], al
	mov	byte [LABEL_DESC_STACK + 7], ah
	
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT
	mov	dword [GdtPtr + 2], eax

	lgdt	[GdtPtr]

  mov     eax, cr0
  or      eax, 1
  mov     cr0, eax

	jmp	dword SelectorCode32:0

; clear screen, in 16-bit mode
cls16:
  mov     ah, 7     ; scroll down function
  mov     al, 0     ; 0 = entire window
  mov     cx, 0     ; (0, 0) as upper left corner
  mov     dx, 184fh ; (24, 79) as lower right corner
  mov     bh, 7     ; normal attribute
  int     10h       ; call bios function

  mov     ah, 2     ; set cursor position
  mov     bh, 0     ; page 0
  mov     dh, 0     ; row 0
  mov     dl, 0     ; col 0
  int     10h
  ret

; greetings using bios function call, in 16-bit mode
hello16:
  mov     ax, greeting16
  push    bp                    ; XXX: find out what is boot time bp
  mov     bp, ax                ; es:bp = offset of string
  mov     ah, 13h               ; write strings function
  mov     bh, 0                 ; page 0
  mov     bl, 0ch               ; color
  mov     cx, greeting16len     ; string length
  mov     dh, 0                 ; row 0
  mov     dl, 0                 ; col 0
  int     10h
  pop     bp
  ret

; greetings using direct vmem io, in 16-bit mode
hello16direct:
  mov     ax, greeting16
  mov     si, ax                  ; source index
  mov     ax, 0b800h              ; vmem
  mov     gs, ax                  ; use gs for output
  mov     di, 80 * 2              ; dest index, start from second row
  mov     cx, greeting16len       ; string length
  mov     ah, 02                  ; text color
hello16direct.1:
  mov     al, [ds:si]
  mov     [gs:di], ax
  inc     si
  add     di, 2
  loop hello16direct.1
  ret
; end of code 16

[SECTION .data16]
greeting16:
  db  "Greetings from 16-bit real mode!"
  greeting16len equ $ - greeting16
; end of data 16

[SECTION .data32]
ALIGN	32
[BITS	32]
LABEL_DATA:
  PMMessage:		db	"Greetings from 32-bit protected mode!"
  OffsetPMMessage		equ	PMMessage - $$
  DataLen			equ	$ - LABEL_DATA
; endof data 32

[SECTION .s32]
[BITS	32]
LABEL_SEG_CODE32:
  mov	ax, SelectorData
  mov	ds, ax
  mov	ax, SelectorVideo
  mov	gs, ax

  mov	ax, SelectorStack
  mov	ss, ax

  mov	esp, TopOfStack

  mov	ah, 09h
  xor	esi, esi
  xor	edi, edi
  mov	esi, OffsetPMMessage
  mov	edi, (80 * 2 + 0) * 2

  mov ecx, DataLen
.1:
  lodsb
  mov	[gs:edi], ax
	add	edi, 2
	loop .1

  jmp	$

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; end of code 32

[SECTION .gs]
ALIGN	32
[BITS	32]
LABEL_STACK:
  times 16 db 0

TopOfStack	equ	$ - LABEL_STACK - 1

