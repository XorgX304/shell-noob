; this demo is from https://gist.githubusercontent.com/Morgawr/9852347/raw/37fafdbfab6684d4fb374c826a2c1bc628cbe2b0/netcat-shellcode.asm
; :set ft=masm - handy for intel highlighting in Vim

BITS 32

global _start 

section .text

_start:


jmp short       forward

back:
pop             esi               ; grab the last line
xor             eax, eax          ; I believe eax is now NULL

; store stuff
mov byte        [esi + 9], al     ; terminate /bin/bash
mov byte        [esi + 12], al    ; terminate -c
mov byte        [esi + 15], al    ; terminate /bin/shi -i ... 

; move stuff
mov long        [esi + 16], esi   ; address of /bin/bash in AAAA
lea             ebx, [esi + 10]    ; get address of -c  
mov long        [esi + 20], ebx   ; store address of -c in BBBB 
lea             ebx, [esi + 13]   ; get address of /bin/bash -i ... 
mov long        [esi + 24], ebx   ; store address of /bin/bash -i ... in CCCC
mov long        [esi + 28], eax   ; put NULL in DDDD

; running stuff
mov byte        al, 0x0b          ; pass the execve syscall number as argument
mov             ebx, esi          
lea             ecx, [esi + 16]   ; /bin/bash -c /bin/bash ....
lea             edx, [esi + 28]   ; NULL
int             0x80              ; Run the execve syscall

forward:
call            back
db "/bin/bash#-c#ps#AAAABBBBCCCCDDDD"
