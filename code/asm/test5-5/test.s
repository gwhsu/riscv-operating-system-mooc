# Add
# Format:
#	ADD RD, RS1, RS2
# Description:
#	The contents of RS1 is added to the contents of RS2 and the result is 
#	placed in RD.
 	.text
    .global _start

_start:
    # Create a new 4 byte array
    la x3, array1       # x3 = array1;

    li x4, 0x11111111
    li x5, 0xffffffff
    sw x4, 0(x3)        # array1[0] = 0x11111111;
    sw x5, 4(x3)        # array1[1] = 0xffffffff;

    lw x6, 0(x3)        # i = array1[0];
    lw x7, 4(x3)        # j = array1[1];

stop:
    j stop  # endless loop

array1:
    .word 0xffffffff # array1[0] = 0
    .word 0xffffffff # array1[1] = 0

    .end
