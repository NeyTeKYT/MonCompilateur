.data
FormatString1:	.string "%llu"	# used by printf to display 64-bit unsigned integers
FormatString2:	.string "%lf"	# used by printf to display 64-bit floating point numbers
FormatString3:	.string "%c"	# used by printf to display a 8-bit single character
TrueString:	.string "TRUE"	# used by printf to display the boolean value TRUE
FalseString:	.string "FALSE"	# used by printf to display the boolean value FALSE
x:	.double 0.0
y:	.double 0.0
	.align 8
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	subq $8,%rsp			# allocate 8 bytes on stack's top
	movl	$0, (%rsp)	# Conversion of 12.5 (32 bit high part)
	movl	$1076428800, 4(%rsp)	# Conversion of 12.5 (32 bit low part)
	pop x
	subq $8,%rsp			# allocate 8 bytes on stack's top
	movl	$0, (%rsp)	# Conversion of 0.25 (32 bit high part)
	movl	$1070596096, 4(%rsp)	# Conversion of 0.25 (32 bit low part)
	pop y
	push x
	push y
	fldl 8(%rsp)	
	fldl (%rsp)	
	addq $16, %rsp	
	faddp %st(0), %st(1)	# op1 + op2
	subq $8, %rsp	
	fstpl (%rsp)	
	pop x
	push x
	movsd (%rsp), %xmm0	# Charger le double à afficher depuis le sommet de la pile
	subq $16, %rsp		# Allouer de l'espace (alignement)
	movsd %xmm0, 8(%rsp)	# Sauvegarder le double pour l'appel printf
	movq $FormatString2, %rdi	# "%f\n"
	movq $1, %rax		# Indique à printf qu'on utilise les registres xmm
	call printf@PLT
	addq $24, %rsp		# Réajuster la pile après appel
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
