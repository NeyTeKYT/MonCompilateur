.data
FormatString1:	.string "%llu"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%lf"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE"	# used by printf to display the boolean value FALSE
i:	.quad 0
j:	.quad 0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
For0:
	push $0
	pop i
	push $5
	pop %rsi	# La borne TO
	pop %rax	# La valeur initiale du compteur pour aller à TO
ForInit0:
ForTest0:
	cmpq %rsi, %rax	
	ja EndFor0
	push j
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop j
	addq $1, %rax	# i++
	jmp ForTest0
EndFor0:
	push i
	pop %rsi	# Valeur entière à afficher
	movq $FormatString1, %rdi	# "%llu\n"
	movl $0, %eax
	call printf@PLT
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
