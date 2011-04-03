; boot loader for SantaOS
; written by Santa Zhang, 2011
;
; start the cpu, switch to 32-bit protected mode
;
; the bios loads this code from the first sector of the hard disk
; into memory at 0x7c00, and starts executing in real mode,
; with %cs = 0 %ip=7c00

%macro Descriptor 3
  dw      %2 & 0ffffh
  dw      %1 & 0ffffh
  db      (%1 >> 16) & 0ffh
  dw      ((%2 >> 8) & 0f00h) | (%3 & 0f0ffh)
  db      (%1 >> 24) & 0ffh
%endmacro

; boot loader start, 16-bit mode
start:
  org     07c00h

  .code16     ; 16-bit mode
  cli         ; disable interrupts
  cld         ; string operations increment

  ; setup data segment registers (ds, es, ss)
  mov     ax, cs
  mov     ds, ax  ; data segment
  mov     es, ax  ; extra segment
  mov     ss, ax  ; stack segment
  call    cls16
  call    hello16
  call    hello16direct
  jmp     $

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
  mov     bp, ax    ; es:bp = offset of string
  mov     ah, 13h   ; write strings function
  mov     bh, 0     ; page 0
  mov     bl, 0ch   ; color
  mov     cx, 27    ; string length
  mov     dh, 0     ; row 0
  mov     dl, 0     ; col 0
  int     10h

  mov     ah, 2     ; set cursor position
  mov     bh, 0     ; page 0
  mov     dh, 1     ; row 1
  mov     dl, 0     ; col 0
  int     10h
  ret

; greetings using direct vmem io, in 16-bit mode
hello16direct:
  ; TODO
  ret

greeting16:
  db  "Greetings from 16-bit mode!"


