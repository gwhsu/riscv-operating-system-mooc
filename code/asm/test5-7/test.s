    .text
    .global _start

_start:

    li x4, 0    # len = 0
    la x5, array_hello  # pArr = array

while1:
    add x7, x5, x4  # x6 = array[len]
    lb x6, 0(x7)     
    addi x4, x4, 1  # len++
    bne x6, x0, while1 # if (array[len] != '\0') --> while1 


    li x3, 0x23333333   # The judgment has come out

stop:
    j stop  # endless loop


array_hello: # Variable s of structure S
    .string "hello,world\0"
    .end
