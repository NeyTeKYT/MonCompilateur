.data
FormatString1:	.string "%llu"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%lf"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE"	# used by printf to display the boolean value FALSE
i:	.quad 0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $0
	pop i
While0:
	push i
	push $10
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	jb Vrai2	# If below
	push $0		# False
	jmp Suite2
Vrai2:	push $0xFFFFFFFFFFFFFFFF		# True
Suite2:
	pop %rax	# Get the result of expression
	cmpq $0, %rax
	je EndWhile0	# if FALSE, jump out of the loop0
	push i
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop i
	jmp While0
EndWhile0:
	push i
	pop %rsi	# Valeur entière à afficher
	movq $FormatString1, %rdi	# "%llu\n"
	movl $0, %eax
	call printf@PLT
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
