; boot loader for SantaOS
; written by Santa Zhang, 2011
;
; start the cpu, switch to 32-bit protected mode
;
; the bios loads this code from the first sector of the hard disk
; into memory at 0x7c00, and starts executing in real mode,
; with %cs = 0 %ip=7c00

; usage SEG base, limit, attr
;   base:   dd
;   limit:  dd (low 20 bits available)
;   attr:   dw (lower 4 bits of higher byte are always 0)
%macro segdesc 3
  dw      %2 & 0ffffh
  dw      %1 & 0ffffh
  db      (%1 >> 16) & 0ffh
  dw      ((%2 >> 8) & 0f00h) | (%3 & 0f0ffh)
  db      (%1 >> 24) & 0ffh
%endmacro

; boot loader start, 16-bit mode
[section .s16]
[bits 16]
start:
  org     07c00h

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
  ; TODO

  xor     eax, eax
  mov     ax, cs
  shl     eax, 4
  add     eax, segdesc_code32
  mov     word [segdesc_code32 + 2], ax
  shr     eax, 16
  mov     byte [segdesc_code32 + 4], al
  mov     byte [segdesc_code32 + 7], ah

  xor     eax, eax
  mov     ax, ds
  shl     eax, 4
  add     eax, label_gdt
  mov     dword [gdtdesc + 2], eax
 
  lgdt    [gdtdesc]
  mov     eax, cr0
  or      eax, 1
  mov     cr0, eax

  ; FIXME just can't jump into protect mode
  jmp     $
;  jmp     dword selector_code32:0

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
  push    bp        ; XXddX: find out what is boot time bp
  mov     bp, ax    ; es:bp = offset of string
  mov     ah, 13h   ; write strings function
  mov     bh, 0     ; page 0
  mov     bl, 0ch   ; color
  mov     cx, 27    ; string length
  mov     dh, 0     ; row 0
  mov     dl, 0     ; col 0
  int     10h
  pop     bp
  ret

; greetings using direct vmem io, in 16-bit mode
hello16direct:
  mov     ax, greeting16
  mov     si, ax      ; source index
  mov     ax, 0b800h  ; vmem
  mov     gs, ax      ; use gs for output
  mov     di, 80 * 2  ; dest index, start from second row
  mov     cx, 27      ; string length
  mov     ah, 02      ; text color
hello16direct.1:
  mov     al, [ds:si]
  mov     [gs:di], ax
  inc     si
  add     di, 2
  loop hello16direct.1
  ret

greeting16:
  db  "Greetings from 16-bit mode!"

; 32 bit code
[section .s32]
[bits 32]
label_seg_code32:
  mov ax, segdesc_video
  mov gs, ax
  mov edi, (80 * 11) * 2
  mov ah, 0ch
  mov al, 'p'
  mov [gs:edi], ax

  jmp $

; bootstrap gdt
[section .gdt]
label_gdt:
  segdesc   0,    0,          0
segdesc_code32:
  segdesc   0,    0xffffffff, 98h + 4000h ; 0~0xffffffff, code & 32-bit
segdesc_data32:
  segdesc   0,    0xffffffff, 92h         ; 0~0xffffffff, data read/write
segdesc_video:
  segdesc   0b8000h,  0ffffh, 92h         ; data read/write, for output

selector_code32 equ segdesc_code32 - label_gdt

gdtdesc:
  dw      $ - label_gdt - 1   ; gdt limit: sizeof(gdt) - 1
  dd      label_gdt; gdt base: address of gdt

