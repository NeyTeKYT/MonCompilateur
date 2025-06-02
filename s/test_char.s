.data
FormatString1:	.string "%llu"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%lf"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE"	# used by printf to display the boolean value FALSE
c:	.byte 0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	movq $0, %rax
	movb $'A',%al
	push %rax	# push a 64-bit version of 'A'
	pop %rax
	movb %al,c
	push c
	pop %rsi			# Récupère le caractère (8 bits dans %sil)
	movq $FormatString3, %rdi	# "%c\n"
	movl $0, %eax
	call printf@PLT
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
