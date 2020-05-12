instructions = {
    'nop': 0x01,
    'iadd': 0x02,
    'isub': 0x05,
    'iand': 0x08,
    'ior': 0x0b,
    'dup': 0x0e,
    'pop': 0x10,
    'swap': 0x13,
    'wide': 0x28,
    'ireturn': 0x6b,
    'if_icmpeq': 0x4b,  # Desvios
    'goto': 0x3c,
    'iflt': 0x43,
    'ifeq': 0x47,
    'invokevirtual': 0x55,  # Operando de dois bytes.
    'ldc_w': 0x32,
    'iinc': 0x36,  # Operando de um byte.
    'bipush': 0x19,
    'iload': 0x1c,
    'istore': 0x22,
}
