#!/usr/bin/env python3

# Usage: merge.py <ipc/insn/cycles CSV> <accesses CSV> <mem ctrl CSV> <reuse CSV> <DRAM latency> <L2 latency> <cache line size>

import pandas as pd
import sys

in_basic = sys.argv[1]
in_access = sys.argv[2]
in_controllers = sys.argv[3]
in_reuse = sys.argv[4]

DRAM_LATENCY = int(sys.argv[5])
L2_LATENCY = int(sys.argv[6])
CACHE_LINE_SIZE = int(sys.argv[7])

with open(in_basic) as f:
    df_b = pd.read_csv(f)
with open(in_access) as f:
    df_a = pd.read_csv(f)
with open(in_controllers) as f:
    df_c = pd.read_csv(f)
with open(in_reuse) as f:
    df_r = pd.read_csv(f)

print(
    'kernel name,ipc,instructions,cycles,forward reuse,reuse factor,unique DRAM accesses,memory controller usage,corrected ipc')


def undf(d, col):
    return [x for x in d[col]]


cols_unzip = [
    undf(df_b, 'kernel name'), undf(df_b, 'ipc'), undf(df_b, 'instruction count'), undf(df_b, 'cycle count'),
    undf(df_r, 'forward reuse') + [0], [0] + undf(df_r, 'forward reuse'),  # actual value, factor
    undf(df_a, 'unique DRAM accesses'), undf(df_c, 'active memory controllers')
]

cols = zip(*cols_unzip)

for (name, ipc, insn, cycles, f_reuse, reuse, access, mem_ctr) in cols:
    delta = (access / (mem_ctr * CACHE_LINE_SIZE)) * reuse * (DRAM_LATENCY - L2_LATENCY)
    cycles_upd = cycles - delta
    ipc_upd = insn / cycles_upd

    print(','.join([str(x) for x in [name, ipc, insn, cycles, f_reuse, reuse, access, mem_ctr, ipc_upd]]))
