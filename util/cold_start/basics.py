#!/usr/bin/env python3

import sys

kernel = ''
insn = ''
cycle = ''
ipc = ''

print('kernel name,ipc,instruction count,cycle count')
for pre_line in sys.stdin:
    line = pre_line.strip()
    if line[0] == '@': # kernel
        if kernel != '':
            print(f'{kernel},{ipc},{insn},{cycle}')
            insn = ''
            cycle = ''
            ipc = ''
        kernel = line[1:].strip()
    elif line[0] == '^': # ipc
        ipc = line[1:].strip()
    elif line[0] == '!': # instructions
        insn = line[1:].strip()
    elif line[0] == '#': # cycles
        cycle = line[1:].strip()

print(f'{kernel},{ipc},{insn},{cycle}')
