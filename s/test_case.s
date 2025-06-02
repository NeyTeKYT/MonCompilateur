.data
FormatString1:	.string "%llu"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%lf"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE"	# used by printf to display the boolean value FALSE
x:	.quad 0
y:	.quad 0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $2
	pop x
	push $0
	pop y
Case0:
	push x
	pop %rax	# Get the result of expression
Cas1:
	push $1
	push $10
	pop y
Cas2:
	push $2
	push $20
	pop y
Cas3:
	push $3
	push $4
	push $30
	pop y
	jmp EndCase0
EndCase0:
	push y
	push $20
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	je Vrai3	# If equal
	push $0		# False
	jmp Suite3
Vrai3:	push $0xFFFFFFFFFFFFFFFF		# True
Suite3:
	pop %rax	# Get the result of expression
	cmpq $0, %rax
	je Else1	# if FALSE, jump to Else1
	push x
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop x
	jmp Next1	# Do not execute the else statement
Else1:
	push x
	push $1
	pop %rbx
	pop %rax
	subq	%rbx, %rax	# SUB
	push %rax
	pop x
Next1:
	push x
	pop %rsi	# Valeur entière à afficher
	movq $FormatString1, %rdi	# "%llu\n"
	movl $0, %eax
	call printf@PLT
	push y
	pop %rsi	# Valeur entière à afficher
	movq $FormatString1, %rdi	# "%llu\n"
	movl $0, %eax
	call printf@PLT
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
