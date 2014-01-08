// RUN: llvm-mc -triple i386-unknown-unknown --show-encoding %s | FileCheck %s

	.code16

	movl $0x12345678, %ebx
// CHECK: movl
// CHECK: encoding: [0x66,0xbb,0x78,0x56,0x34,0x12]
	pause
// CHECK: pause
// CHECK: encoding: [0xf3,0x90]
	sfence
// CHECK: sfence
// CHECK: encoding: [0x0f,0xae,0xf8]
	lfence
// CHECK: lfence
// CHECK: encoding: [0x0f,0xae,0xe8]
	mfence
	stgi
// CHECK: stgi
// CHECK: encoding: [0x0f,0x01,0xdc]
	clgi
// CHECK: clgi
// CHECK: encoding: [0x0f,0x01,0xdd]

	rdtscp
// CHECK: rdtscp
// CHECK:  encoding: [0x0f,0x01,0xf9]


// CHECK: movl	%eax, 16(%ebp)          # encoding: [0x67,0x66,0x89,0x45,0x10]
	movl	%eax, 16(%ebp)
// CHECK: movl	%eax, -16(%ebp)          # encoding: [0x67,0x66,0x89,0x45,0xf0]
	movl	%eax, -16(%ebp)

// CHECK: testb	%bl, %cl                # encoding: [0x84,0xcb]
        testb %bl, %cl

// CHECK: cmpl	%eax, %ebx              # encoding: [0x66,0x39,0xc3]
        cmpl %eax, %ebx

// CHECK: addw	%ax, %ax                # encoding: [0x01,0xc0]
        addw %ax, %ax

// CHECK: shrl	%eax                    # encoding: [0x66,0xd1,0xe8]
        shrl $1, %eax

// CHECK: shll	%eax                    # encoding: [0x66,0xd1,0xe0]
        sall $1, %eax
// CHECK: shll	%eax                    # encoding: [0x66,0xd1,0xe0]
        sal $1, %eax

into
// CHECK: into
// CHECK:  encoding: [0xce]
int3
// CHECK: int3
// CHECK:  encoding: [0xcc]
int $4
// CHECK: int $4
// CHECK:  encoding: [0xcd,0x04]
int $255
// CHECK: int $255
// CHECK:  encoding: [0xcd,0xff]

// CHECK: pushfl	# encoding: [0x66,0x9c]
        pushfl
// CHECK: popfl	        # encoding: [0x66,0x9d]
        popfl

// CHECK: cmoval	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x47,0xd0]
        	cmoval	%eax,%edx

// CHECK: cmovael	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x43,0xd0]
        	cmovael	%eax,%edx

// CHECK: cmovbel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x46,0xd0]
        	cmovbel	%eax,%edx

// CHECK: cmovbl	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x42,0xd0]
        	cmovbl	%eax,%edx

// CHECK: cmovbw %bx, %bx
cmovnae	%bx,%bx


// CHECK: cmovbel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x46,0xd0]
        	cmovbel	%eax,%edx

// CHECK: cmovbl	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x42,0xd0]
        	cmovcl	%eax,%edx

// CHECK: cmovel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x44,0xd0]
        	cmovel	%eax,%edx

// CHECK: cmovgl	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4f,0xd0]
        	cmovgl	%eax,%edx

// CHECK: cmovgel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4d,0xd0]
        	cmovgel	%eax,%edx

// CHECK: cmovll	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4c,0xd0]
        	cmovll	%eax,%edx

// CHECK: cmovlel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4e,0xd0]
        	cmovlel	%eax,%edx

// CHECK: cmovbel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x46,0xd0]
        	cmovnal	%eax,%edx

// CHECK: cmovnel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x45,0xd0]
        	cmovnel	%eax,%edx

// CHECK: cmovael	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x43,0xd0]
        	cmovnbl	%eax,%edx

// CHECK: cmoval	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x47,0xd0]
        	cmovnbel	%eax,%edx

// CHECK: cmovael	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x43,0xd0]
        	cmovncl	%eax,%edx

// CHECK: cmovnel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x45,0xd0]
        	cmovnel	%eax,%edx

// CHECK: cmovlel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4e,0xd0]
        	cmovngl	%eax,%edx

// CHECK: cmovgel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4d,0xd0]
        	cmovnl	%eax,%edx

// CHECK: cmovnel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x45,0xd0]
        	cmovnel	%eax,%edx

// CHECK: cmovlel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4e,0xd0]
        	cmovngl	%eax,%edx

// CHECK: cmovll	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4c,0xd0]
        	cmovngel	%eax,%edx

// CHECK: cmovgel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4d,0xd0]
        	cmovnll	%eax,%edx

// CHECK: cmovgl	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4f,0xd0]
        	cmovnlel	%eax,%edx

// CHECK: cmovnol	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x41,0xd0]
        	cmovnol	%eax,%edx

// CHECK: cmovnpl	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4b,0xd0]
        	cmovnpl	%eax,%edx

// CHECK: cmovnsl	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x49,0xd0]
        	cmovnsl	%eax,%edx

// CHECK: cmovnel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x45,0xd0]
        	cmovnzl	%eax,%edx

// CHECK: cmovol	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x40,0xd0]
        	cmovol	%eax,%edx

// CHECK: cmovpl	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x4a,0xd0]
        	cmovpl	%eax,%edx

// CHECK: cmovsl	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x48,0xd0]
        	cmovsl	%eax,%edx

// CHECK: cmovel	%eax, %edx
// CHECK:  encoding: [0x66,0x0f,0x44,0xd0]
        	cmovzl	%eax,%edx

// CHECK: fmul	%st(0)
// CHECK:  encoding: [0xd8,0xc8]
        fmul %st(0), %st

// CHECK: fadd	%st(0)
// CHECK:  encoding: [0xd8,0xc0]
        fadd %st(0), %st

// CHECK: fsub	%st(0)
// CHECK:  encoding: [0xd8,0xe0]
        fsub %st(0), %st

// CHECK: fsubr	%st(0)
// CHECK:  encoding: [0xd8,0xe8]
        fsubr %st(0), %st

// CHECK: fdivr	%st(0)
// CHECK:  encoding: [0xd8,0xf8]
        fdivr %st(0), %st

// CHECK: fdiv	%st(0)
// CHECK:  encoding: [0xd8,0xf0]
        fdiv %st(0), %st

// CHECK: movl	%cs, %eax
// CHECK:  encoding: [0x66,0x8c,0xc8]
        movl %cs, %eax

// CHECK: movw	%cs, %ax
// CHECK:  encoding: [0x8c,0xc8]
        movw %cs, %ax

// CHECK: movl	%cs, (%eax)
// CHECK:  encoding: [0x67,0x66,0x8c,0x08]
        movl %cs, (%eax)

// CHECK: movw	%cs, (%eax)
// CHECK:  encoding: [0x67,0x8c,0x08]
        movw %cs, (%eax)

// CHECK: movl	%eax, %cs
// CHECK:  encoding: [0x66,0x8e,0xc8]
        movl %eax, %cs

// CHECK: movl	(%eax), %cs
// CHECK:  encoding: [0x67,0x66,0x8e,0x08]
        movl (%eax), %cs

// CHECK: movw	(%eax), %cs
// CHECK:  encoding: [0x67,0x8e,0x08]
        movw (%eax), %cs

// CHECK: movl	%cr0, %eax
// CHECK:  encoding: [0x0f,0x20,0xc0]
        movl %cr0,%eax

// CHECK: movl	%cr1, %eax
// CHECK:  encoding: [0x0f,0x20,0xc8]
        movl %cr1,%eax

// CHECK: movl	%cr2, %eax
// CHECK:  encoding: [0x0f,0x20,0xd0]
        movl %cr2,%eax

// CHECK: movl	%cr3, %eax
// CHECK:  encoding: [0x0f,0x20,0xd8]
        movl %cr3,%eax

// CHECK: movl	%cr4, %eax
// CHECK:  encoding: [0x0f,0x20,0xe0]
        movl %cr4,%eax

// CHECK: movl	%dr0, %eax
// CHECK:  encoding: [0x0f,0x21,0xc0]
        movl %dr0,%eax

// CHECK: movl	%dr1, %eax
// CHECK:  encoding: [0x0f,0x21,0xc8]
        movl %dr1,%eax

// CHECK: movl	%dr1, %eax
// CHECK:  encoding: [0x0f,0x21,0xc8]
        movl %dr1,%eax

// CHECK: movl	%dr2, %eax
// CHECK:  encoding: [0x0f,0x21,0xd0]
        movl %dr2,%eax

// CHECK: movl	%dr3, %eax
// CHECK:  encoding: [0x0f,0x21,0xd8]
        movl %dr3,%eax

// CHECK: movl	%dr4, %eax
// CHECK:  encoding: [0x0f,0x21,0xe0]
        movl %dr4,%eax

// CHECK: movl	%dr5, %eax
// CHECK:  encoding: [0x0f,0x21,0xe8]
        movl %dr5,%eax

// CHECK: movl	%dr6, %eax
// CHECK:  encoding: [0x0f,0x21,0xf0]
        movl %dr6,%eax

// CHECK: movl	%dr7, %eax
// CHECK:  encoding: [0x0f,0x21,0xf8]
        movl %dr7,%eax

// CHECK: wait
// CHECK:  encoding: [0x9b]
	fwait

// CHECK: pushal
// CHECK:  encoding: [0x66,0x60]
        	pushal

// CHECK: popal
// CHECK:  encoding: [0x66,0x61]
        	popal

// CHECK: jmpl *8(%eax)
// CHECK:   encoding: [0x67,0x66,0xff,0x60,0x08]
        jmpl	*8(%eax)

// CHECK: lcalll $2, $4660
// CHECK:   encoding: [0x66,0x9a,0x34,0x12,0x00,0x00,0x02,0x00]
lcalll $0x2, $0x1234


sysret
// CHECK: sysretl
// CHECK: encoding: [0x0f,0x07]
sysretl
// CHECK: sysretl
// CHECK: encoding: [0x0f,0x07]

testl	%ecx, -24(%ebp)
// CHECK: testl	-24(%ebp), %ecx
testl	-24(%ebp), %ecx
// CHECK: testl	-24(%ebp), %ecx


pushw %cs
// CHECK: pushw	%cs
// CHECK: encoding: [0x0e]
pushw %ds
// CHECK: pushw	%ds
// CHECK: encoding: [0x1e]
pushw %ss
// CHECK: pushw	%ss
// CHECK: encoding: [0x16]
pushw %es
// CHECK: pushw	%es
// CHECK: encoding: [0x06]
pushw %fs
// CHECK: pushw	%fs
// CHECK: encoding: [0x0f,0xa0]
pushw %gs
// CHECK: pushw	%gs
// CHECK: encoding: [0x0f,0xa8]

pushl %cs
// CHECK: pushl	%cs
// CHECK: encoding: [0x66,0x0e]
pushl %ds
// CHECK: pushl	%ds
// CHECK: encoding: [0x66,0x1e]
pushl %ss
// CHECK: pushl	%ss
// CHECK: encoding: [0x66,0x16]
pushl %es
// CHECK: pushl	%es
// CHECK: encoding: [0x66,0x06]
pushl %fs
// CHECK: pushl	%fs
// CHECK: encoding: [0x66,0x0f,0xa0]
pushl %gs
// CHECK: pushl	%gs
// CHECK: encoding: [0x66,0x0f,0xa8]

popl %ss
// CHECK: popl	%ss
// CHECK: encoding: [0x66,0x17]
popl %ds
// CHECK: popl	%ds
// CHECK: encoding: [0x66,0x1f]
popl %es
// CHECK: popl	%es
// CHECK: encoding: [0x66,0x07]

pushfd
// CHECK: pushfl
popfd
// CHECK: popfl
pushfl
// CHECK: pushfl
popfl
// CHECK: popfl


	setc	%bl
	setnae	%bl
	setnb	%bl
	setnc	%bl
	setna	%bl
	setnbe	%bl
	setpe	%bl
	setpo	%bl
	setnge	%bl
	setnl	%bl
	setng	%bl
	setnle	%bl

        setneb  %cl // CHECK: setne %cl
	setcb	%bl // CHECK: setb %bl
	setnaeb	%bl // CHECK: setb %bl


// CHECK: lcalll	$31438, $31438
// CHECK: lcalll	$31438, $31438
// CHECK: ljmpl	$31438, $31438
// CHECK: ljmpl	$31438, $31438

calll	$0x7ace,$0x7ace
lcalll	$0x7ace,$0x7ace
jmpl	$0x7ace,$0x7ace
ljmpl	$0x7ace,$0x7ace

// CHECK: calll a
 calll a

// CHECK:	incb	%al # encoding: [0xfe,0xc0]
	incb %al

// CHECK:	incw	%ax # encoding: [0x40]
	incw %ax

// CHECK:	incl	%eax # encoding: [0x66,0x40]
	incl %eax

// CHECK:	decb	%al # encoding: [0xfe,0xc8]
	decb %al

// CHECK:	decw	%ax # encoding: [0x48]
	decw %ax

// CHECK:	decl	%eax # encoding: [0x66,0x48]
	decl %eax

// CHECK: pshufw $14, %mm4, %mm0 # encoding: [0x0f,0x70,0xc4,0x0e]
pshufw $14, %mm4, %mm0

// CHECK: pshufw $90, %mm4, %mm0 # encoding: [0x0f,0x70,0xc4,0x5a]
pshufw $90, %mm4, %mm0

// CHECK: aaa
// CHECK:  encoding: [0x37]
        	aaa

// CHECK: aad	$1
// CHECK:  encoding: [0xd5,0x01]
        	aad	$1

// CHECK: aad
// CHECK:  encoding: [0xd5,0x0a]
        	aad	$0xA

// CHECK: aad
// CHECK:  encoding: [0xd5,0x0a]
        	aad

// CHECK: aam	$2
// CHECK:  encoding: [0xd4,0x02]
        	aam	$2

// CHECK: aam
// CHECK:  encoding: [0xd4,0x0a]
        	aam	$0xA

// CHECK: aam
// CHECK:  encoding: [0xd4,0x0a]
        	aam

// CHECK: aas
// CHECK:  encoding: [0x3f]
        	aas

// CHECK: daa
// CHECK:  encoding: [0x27]
        	daa

// CHECK: das
// CHECK:  encoding: [0x2f]
        	das

// CHECK: bound	2(%eax), %bx
// CHECK:  encoding: [0x67,0x62,0x58,0x02]
        	bound	2(%eax),%bx

// CHECK: bound	4(%ebx), %ecx
// CHECK:  encoding: [0x67,0x66,0x62,0x4b,0x04]
        	bound	4(%ebx),%ecx

// CHECK: arpl	%bx, %bx
// CHECK:  encoding: [0x63,0xdb]
        	arpl	%bx,%bx

// CHECK: arpl	%bx, 6(%ecx)
// CHECK:  encoding: [0x67,0x63,0x59,0x06]
        	arpl	%bx,6(%ecx)

// CHECK: fcompi	%st(2)
// CHECK:  encoding: [0xdf,0xf2]
        	fcompi	%st(2), %st

// CHECK: fcompi	%st(2)
// CHECK:  encoding: [0xdf,0xf2]
        	fcompi	%st(2)

// CHECK: fcompi
// CHECK:  encoding: [0xdf,0xf1]
        	fcompi

// CHECK: fucompi	%st(2)
// CHECK:  encoding: [0xdf,0xea]
        	fucompi	%st(2),%st

// CHECK: fucompi	%st(2)
// CHECK:  encoding: [0xdf,0xea]
        	fucompi	%st(2)

// CHECK: fucompi
// CHECK:  encoding: [0xdf,0xe9]
        	fucompi

// CHECK: wait
// CHECK:  encoding: [0x9b]
        	fclex

// CHECK: fnclex
// CHECK:  encoding: [0xdb,0xe2]
        	fnclex

// CHECK: ud2
// CHECK:  encoding: [0x0f,0x0b]
        	ud2

// CHECK: ud2
// CHECK:  encoding: [0x0f,0x0b]
        	ud2a

// CHECK: ud2b
// CHECK:  encoding: [0x0f,0xb9]
        	ud2b

// CHECK: loope 0
// CHECK: encoding: [0xe1,A]
	loopz 0

// CHECK: loopne 0
// CHECK: encoding: [0xe0,A]
	loopnz 0

// CHECK: outsb # encoding: [0x6e]
// CHECK: outsb
// CHECK: outsb
	outsb
	outsb	%ds:(%si), %dx
	outsb	(%si), %dx

// CHECK: outsw # encoding: [0x6f]
// CHECK: outsw
// CHECK: outsw
	outsw
	outsw	%ds:(%si), %dx
	outsw	(%si), %dx

// CHECK: outsl # encoding: [0x66,0x6f]
// CHECK: outsl
	outsl
	outsl	%ds:(%si), %dx
	outsl	(%si), %dx

// CHECK: insb # encoding: [0x6c]
// CHECK: insb
	insb
	insb	%dx, %es:(%di)

// CHECK: insw # encoding: [0x6d]
// CHECK: insw
	insw
	insw	%dx, %es:(%di)

// CHECK: insl # encoding: [0x66,0x6d]
// CHECK: insl
	insl
	insl	%dx, %es:(%di)

// CHECK: movsb # encoding: [0xa4]
// CHECK: movsb
// CHECK: movsb
	movsb
	movsb	%ds:(%si), %es:(%di)
	movsb	(%si), %es:(%di)

// CHECK: movsw # encoding: [0xa5]
// CHECK: movsw
// CHECK: movsw
	movsw
	movsw	%ds:(%si), %es:(%di)
	movsw	(%si), %es:(%di)

// CHECK: movsl # encoding: [0x66,0xa5]
// CHECK: movsl
// CHECK: movsl
	movsl
	movsl	%ds:(%si), %es:(%di)
	movsl	(%si), %es:(%di)

// CHECK: lodsb # encoding: [0xac]
// CHECK: lodsb
// CHECK: lodsb
// CHECK: lodsb
// CHECK: lodsb
	lodsb
	lodsb	%ds:(%si), %al
	lodsb	(%si), %al
	lods	%ds:(%si), %al
	lods	(%si), %al

// CHECK: lodsw # encoding: [0xad]
// CHECK: lodsw
// CHECK: lodsw
// CHECK: lodsw
// CHECK: lodsw
	lodsw
	lodsw	%ds:(%si), %ax
	lodsw	(%si), %ax
	lods	%ds:(%si), %ax
	lods	(%si), %ax

// CHECK: lodsl # encoding: [0x66,0xad]
// CHECK: lodsl
// CHECK: lodsl
// CHECK: lodsl
// CHECK: lodsl
	lodsl
	lodsl	%ds:(%si), %eax
	lodsl	(%si), %eax
	lods	%ds:(%si), %eax
	lods	(%si), %eax

// CHECK: stosb # encoding: [0xaa]
// CHECK: stosb
// CHECK: stosb
	stosb
	stosb	%al, %es:(%di)
	stos	%al, %es:(%di)

// CHECK: stosw # encoding: [0xab]
// CHECK: stosw
// CHECK: stosw
	stosw
	stosw	%ax, %es:(%di)
	stos	%ax, %es:(%di)

// CHECK: stosl # encoding: [0x66,0xab]
// CHECK: stosl
// CHECK: stosl
	stosl
	stosl	%eax, %es:(%di)
	stos	%eax, %es:(%di)

// CHECK: strw
// CHECK: encoding: [0x0f,0x00,0xc8]
	str %ax

// CHECK: strl
// CHECK: encoding: [0x66,0x0f,0x00,0xc8]
	str %eax


// CHECK: fsubp
// CHECK: encoding: [0xde,0xe1]
fsubp %st,%st(1)

// CHECK: fsubp	%st(2)
// CHECK: encoding: [0xde,0xe2]
fsubp   %st, %st(2)

// CHECK: xchgl %eax, %eax
// CHECK: encoding: [0x66,0x90]
xchgl %eax, %eax

// CHECK: xchgw %ax, %ax
// CHECK: encoding: [0x90]
xchgw %ax, %ax

// CHECK: xchgl %ecx, %eax
// CHECK: encoding: [0x66,0x91]
xchgl %ecx, %eax

// CHECK: xchgl %ecx, %eax
// CHECK: encoding: [0x66,0x91]
xchgl %eax, %ecx