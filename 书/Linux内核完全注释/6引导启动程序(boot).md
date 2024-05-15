Linux的启动代码由汇编编写，为了编写能运行在实模式下（刚开机时）的汇编代码，只能用8086汇编器，因为那时的GNU汇编器只能写32位CPU代码指令，所以Linux启动代码分两种汇编器汇编。
当PC上电，CPU自动进入实模式。
CPU上电RESET后，几乎所有寄存器都被置0，除了CS寄存器被置FFFF，左移4位于IP相加得到FFFF0，所以CPU上电执行的第一条指令地址是FFFF0，这个地址位于外置ROM中（BIOS），这个地址上装着一条JMP指令，来跳到BIOS中的相应硬件检测和初始化函数等。在BIOS最后阶段，会执行一段代码，把磁盘的MBR（主引导记录，第一个扇区，512字节）读入0x7c00处，并跳转到7c00处开始执行。MBR里存的内容就是`boot/bootsect.S`，当它被执行时会把自己移动到0x90000处（576K），并把`boot/setup.S`(2KB)，读入内存0x90200处，而内核其他部分，被读入0x10000(64KB)处。
![[Pasted image 20240505192138.png]]
由于system模块本身不超过0x80000字节，所以移动到0x10000处不会把bootsect.S和setup.s覆盖。setup.s中会把system模块移到物理内存起始位置，
![[Pasted image 20240505210817.png]]
可见，最终结果就是system部分直接被放置在了0物理地址处。这样系统就加载完成了。
setup.s中，是最主要的设置部分，包括要把system移动到0地址，设置一些硬件，启动保护模式，并跳入system模块中的head.s中。
# bootsect.S
主要工作：
* 把自己移动到0x90000
* 把setup.s从磁盘读入到0x90200
* 利用0x13bios中断，读取当前引导盘的参数，识别引导盘类型，把设备号存到root_dev(引导块的508地址处)
* 在屏幕显示“Loading system...”字符串
* 把system读入到0x10000
* 长跳转到setup.s(90200)


![[Pasted image 20240505214547.png]]
``` c
!
! SYS_SIZE is the number of clicks (16 bytes) to be loaded.
! 0x3000 is 0x30000 bytes = 196kB, more than enough for current
! versions of linux
!
SYSSIZE = 0x3000
!
!	bootsect.s		(C) 1991 Linus Torvalds
!
! bootsect.s is loaded at 0x7c00 by the bios-startup routines, and moves
! iself out of the way to address 0x90000, and jumps there.
!
! It then loads 'setup' directly after itself (0x90200), and the system
! at 0x10000, using BIOS interrupts. 
!
! NOTE! currently system is at most 8*65536 bytes long. This should be no
! problem, even in the future. I want to keep it simple. This 512 kB
! kernel size should be enough, especially as this doesn't contain the
! buffer cache as in minix
!
! The loader has been made as simple as possible, and continuos
! read errors will result in a unbreakable loop. Reboot by hand. It
! loads pretty fast by getting whole sectors at a time whenever possible.

.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

SETUPLEN = 4				! nr of setup-sectors
BOOTSEG  = 0x07c0			! original address of boot-sector
INITSEG  = 0x9000			! we move boot here - out of the way
SETUPSEG = 0x9020			! setup starts here
SYSSEG   = 0x1000			! system loaded at 0x10000 (65536).
ENDSEG   = SYSSEG + SYSSIZE		! where to stop loading

! ROOT_DEV:	0x000 - same type of floppy as boot.
!		0x301 - first partition on first drive etc
ROOT_DEV = 0x306

entry _start   
_start:   //当前的bootsect复制到90000处,并跳到90000运行
	mov	ax,#BOOTSEG
	mov	ds,ax
	mov	ax,#INITSEG
	mov	es,ax
	mov	cx,#256
	sub	si,si
	sub	di,di
	rep
	movw
	jmpi	go,INITSEG
go:	mov	ax,cs
	mov	ds,ax
	mov	es,ax
! put stack at 0x9ff00.
	mov	ss,ax
	mov	sp,#0xFF00		! arbitrary value >>512
	//到现在，所有段都被指向了9000，栈被设置到了9FF00
! load the setup-sectors directly after the bootblock.
! Note that 'es' is already set up.

load_setup: //开始读入setup.s,将4个扇区读入es:bx，如果出错，CF会被置高，重读
	mov	dx,#0x0000		! drive 0, head 0
	mov	cx,#0x0002		! sector 2, track 0
	mov	bx,#0x0200		! address = 512, in INITSEG
	mov	ax,#0x0200+SETUPLEN	! service 2, nr of sectors
	int	0x13			! read it
	jnc	ok_load_setup		! ok - continue
	mov	dx,#0x0000
	mov	ax,#0x0000		! reset the diskette
	int	0x13
	j	load_setup

ok_load_setup: //已经读取完成，现在获取一下硬盘参数，主要是每磁道扇区数

! Get disk drive parameters, specifically nr of sectors/track

	mov	dl,#0x00
	mov	ax,#0x0800		! AH=8 is get drive parameters
	int	0x13
	mov	ch,#0x00
	seg cs//表示下一条语句的操作数在cs段寄存器所指段中
	mov	sectors,cx
	mov	ax,#INITSEG
	mov	es,ax//0x13会改变es值，再给他改回来

! Print some inane message

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh //bh置零
	int	0x10  
	
	mov	cx,#24
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg1
	mov	ax,#0x1301		! write string, move cursor
	int	0x10  //字符显示完成，开始读入system

! ok, we've written the message, now
! we want to load the system (at 0x10000)

	mov	ax,#SYSSEG
	mov	es,ax		! segment of 0x010000
	call	read_it
	call	kill_motor

! After that we check which root-device to use. If the device is
! defined (!= 0), nothing is done and the given device is used.
! Otherwise, either /dev/PS0 (2,28) or /dev/at0 (2,8), depending
! on the number of sectors that the BIOS reports currently.

	seg cs
	mov	ax,root_dev
	cmp	ax,#0
	jne	root_defined
	seg cs
	mov	bx,sectors
	mov	ax,#0x0208		! /dev/ps0 - 1.2Mb
	cmp	bx,#15
	je	root_defined
	mov	ax,#0x021c		! /dev/PS0 - 1.44Mb
	cmp	bx,#18
	je	root_defined
undef_root:
	jmp undef_root
root_defined:
	seg cs
	mov	root_dev,ax//把根设备准备好，保存在root_dev里

! after that (everyting loaded), we jump to
! the setup-routine loaded directly after
! the bootblock:

	jmpi	0,SETUPSEG

! This routine loads the system at address 0x10000, making sure
! no 64kB boundaries are crossed. We try to load it as fast as
! possible, loading whole tracks whenever we can.
!
! in:	es - starting address segment (normally 0x1000)
!
sread:	.word 1+SETUPLEN	! sectors read of current track
head:	.word 0			! current head
track:	.word 0			! current track

read_it:
	mov ax,es
	test ax,#0x0fff  //类似AND指令，如果结果为0，则表明es段地址与64K对齐，ZF=1
die:	jne die			! es must be at 64kB boundary
	xor bx,bx		! bx is starting address within segment
rp_read:
	mov ax,es
	cmp ax,#ENDSEG		! have we loaded all yet?
	jb ok1_read        //还没读到结尾，就跳转过去继续读
	ret
ok1_read:
	seg cs
	mov ax,sectors
	sub ax,sread
	mov cx,ax
	shl cx,#9
	add cx,bx
	jnc ok2_read
	je ok2_read
	xor ax,ax
	sub ax,bx
	shr ax,#9
ok2_read:
	call read_track
	mov cx,ax
	add ax,sread
	seg cs
	cmp ax,sectors
	jne ok3_read
	mov ax,#1
	sub ax,head
	jne ok4_read
	inc track
ok4_read:
	mov head,ax
	xor ax,ax
ok3_read:
	mov sread,ax
	shl cx,#9
	add bx,cx
	jnc rp_read
	mov ax,es
	add ax,#0x1000
	mov es,ax
	xor bx,bx
	jmp rp_read

read_track:
	push ax
	push bx
	push cx
	push dx
	mov dx,track
	mov cx,sread
	inc cx
	mov ch,dl
	mov dx,head
	mov dh,dl
	mov dl,#0
	and dx,#0x0100
	mov ah,#2
	int 0x13
	jc bad_rt
	pop dx
	pop cx
	pop bx
	pop ax
	ret
bad_rt:	mov ax,#0
	mov dx,#0
	int 0x13
	pop dx
	pop cx
	pop bx
	pop ax
	jmp read_track

!/*
! * This procedure turns off the floppy drive motor, so
! * that we enter the kernel in a known state, and
! * don't have to worry about it later.
! */
kill_motor:
	push dx
	mov dx,#0x3f2
	mov al,#0
	outb
	pop dx
	ret

sectors:
	.word 0

msg1:
	.byte 13,10
	.ascii "Loading system ..."
	.byte 13,10,13,10

.org 508
root_dev:
	.word ROOT_DEV
boot_flag:
	.word 0xAA55

.text
endtext:
.data
enddata:
.bss
endbss:
```
# SETUP.S
setup.s主要作用是读取硬件信息，并保存在硬盘里，供操作系统使用，具体作用：
* 读取并保留硬件信息
* 把system从0x10000移动到0x00000
* 加载中断描述符表寄存器(idtr)和全局描述符表寄存器(gdtr)
* 开启A20地址线
* 设置中断控制芯片8259A，将硬件中断号设置为0x20-0x2f
* 设置CR0，进入32位保护模式
* 跳转到head.s运行

![[Pasted image 20240515234331.png]]
setup.s前面部分是在利用bios中断获取硬件信息，并存入0x90000开始的地方。
开始把system从0x10000移动到0x00000：
```c
	cli			! no interrupts allowed !

! first we move the system to it's rightful place

	mov	ax,#0x0000
	cld			! 'direction'=0, movs moves forward
do_move:
	mov	es,ax		! destination segment
	add	ax,#0x1000
	cmp	ax,#0x9000
	jz	end_move
	mov	ds,ax		! source segment
	sub	di,di
	sub	si,si
	mov 	cx,#0x8000
	rep
	movsw
	jmp	do_move
```
移动完之后，开始设置gdt