    .text
    .global _start

# #define set_struct(s) \
#  s.a = a; \
#  s.b = b;
# #define get_struct(s) \
#  a = s.a; \
#  b = s.b;

.macro get_struct S_s
    lw x3, 0(\S_s)  # a = s.a
    lw x4, 4(\S_s)  # b = s.b
.endm

.macro set_struct S_s
    sw x3, 0(\S_s)   # s.a = a
    sw x4, 4(\S_s)   # s.b = b
.endm



_start:
    li x3, 0x12345678   # a = 0x12345678
    li x4, 0x87654321   # b = 0x87654321
    la x5, struct_S_s

    set_struct x5
    li x3, 0    # a = 0
    li x4, 0    # b = 0

    get_struct x5


stop:
    j stop  # endless loop

struct_S_s: # struct S s = {0}
    .word 0 # s.a = 0
    .word 0 # s.b = 0

    .end
