.data
FormatString1:	.string "%llu"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%lf"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE"	# used by printf to display the boolean value FALSE
a:	.quad 0
b:	.quad 0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $3
	pop a
	push a
	pop %rsi	# Valeur entière à afficher
	movq $FormatString1, %rdi	# "%llu\n"
	movl $0, %eax
	call printf@PLT
	push a
	push $2
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
	push $10
	pop b
	jmp Next1	# Do not execute the else statement
Else1:
	push $20
	pop b
Next1:
	push b
	pop %rsi	# Valeur entière à afficher
	movq $FormatString1, %rdi	# "%llu\n"
	movl $0, %eax
	call printf@PLT
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
