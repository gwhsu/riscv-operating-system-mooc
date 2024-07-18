# Add
# Format:
#	ADD RD, RS1, RS2
# Description:
#	The contents of RS1 is added to the contents of RS2 and the result is 
#	placed in RD.
	.text
	.global _start

_start:
	li x5, 1	# b = 1
	li x6, 2	# c = 2
	li x7, 3	# e = 3
	add x4, x5, x6	# a = b + c
	sub x3, x4, x7	# d = a - e

stop:
	j stop	# Infinite loop to stop execution

	.end