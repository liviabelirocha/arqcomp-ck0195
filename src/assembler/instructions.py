instructions = {
    'nop':       [0x01, []],         'iadd':          [0x02, []],         'isub':    [0x05, []],                 'iand': [0x08, []],
    'ior':       [0x0b, []],         'dup':           [0x0e, []],         'pop':     [0x10, []],                 'swap': [0x13, []],
   
    'bipush':    [0x19, ['byte']],   'iload':         [0x1c, ['varnum']], 'istore':  [0x22, ['new_varnum']],
    'wide':      [0x28, []],         'ldc_w':         [0x32, ['index']],  'iinc':    [0x36, ['varnum', 'const']],
    'goto':      [0x3c, ['offset']], 'iflt':          [0x43, ['offset']], 'ifeq':    [0x47, ['offset']],
    'if_icmpeq': [0x4b, ['offset']], 'invokevirtual': [0x55, ['disp']],   'ireturn': [0x6b, []]
}
