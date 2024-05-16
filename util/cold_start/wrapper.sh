#!/usr/bin/bash

TMPDIR="/tmp"

# Usage: ???

#                           instructions                   accesses
# Helper to compute IPC = -----------------; \Delta = -------------------- * reuse * (DRAM - L2)
#                          cycles - \Delta             controllers * line

show_help ()
{
  echo "Usage: $1 <AccelSim output TXT> <memory traces directory> <GPGPU-Sim config> <workload/output name>"
  echo " -> Using temp directory $TMPDIR"
}

if [[ "$1" = "-h" ]]; then show_help "$0"; exit 0; fi 
if [[ "$#" != "4" ]]; then show_help "$0"; exit -1; fi

here=$(dirname "$0")
acc_out="$1"
trace_dir="$2"
config="$3"
out="$4"

echo ' --- Parameters: --- '
echo " -> Script directory:                         $here"
echo " -> Temp directory (for intermediary output): $TMPDIR"
echo " -> AccelSim output TXT file:                 $acc_out"
echo " -> (Memory) traces directory:                $trace_dir"
echo " -> GPGPU-Sim configuration file:             $config"
echo " -> Workload name (output):                   $out"

# STEP 1: gather basic data from AccelSim output
# -> basic information = kernel name, cycle count, instruction count, IPC

echo '  -- Gathering basic data --  '
grep 'launching\|gpu_sim_cycle\|gpu_sim_insn\|gpu_sim_insn\|gpu_ipc' "$acc_out" |
  sed 's/launching kernel name: \(.*\) uid.*/@\1/' | sed 's/gpu_sim_cycle = /#/' | 
  sed 's/gpu_sim_insn = /!/' | sed 's/gpu_ipc = /^/' | 
  python3 "$here/basics.py" >"$TMPDIR/${out}_basics.csv"

# STEP 2: gather accesses data from AccelSim output

echo '  -- Gathering access data --  '
grep 'launching\|\[COLD_START_DRAM_REQ]' "$acc_out" |
  sed 's/launching kernel name: \(.*\)/@\1/' | sed 's/COLD_START_DRAM_REQ]: //' | 
  "$here/mem_access" >"$TMPDIR/${out}_access.csv"

# STEP 3: gather memory controller data from AccelSim output 

echo '  -- Gathering memory controller data --  '
grep 'launching\|number of total \(read\|write\) accesses\|dram\[[0-9]*]' "$acc_out" |
  sed 's/launching kernel name: \(.*\) uid.*/@\1/' | tr -s ' ' | 
  "$here/mem_controller" >"$TMPDIR/${out}_ctrl.csv"

# STEP 4: gather reuse data from traces 

echo '  -- Gathering forward reuse data --  '
echo 'kernel index,forward reuse' >"$TMPDIR/${out}_reuse.csv"
find "$trace_dir" -name '*.traceg' | while IFS= read -r line; do 
  idx=$(echo "$line" | sed 's#.*/kernel-mem-\([0-9]*\).traceg#\1#')
  from="$trace_dir/kernel-mem-$idx.traceg"
  incr=$((idx+1))
  to="$trace_dir/kernel-mem-$incr.traceg"

  if [[ -f "$to" ]]; then
    echo "    -> Computing forward reuse for $from ($from -> $to)"
    reuse=$("$here/reuse" "$from" "$to")
    echo "$idx,$reuse" >>"$TMPDIR/${out}_reuse.csv"
  fi
done

# STEP 5: gather DRAM, L2 latency; cache line size from config

echo '  -- Gathering relevant config options --  '
l2=$(grep '[-]gpgpu_l2_rop_latency' "$config" | cut -d' ' -f2)
dram=$(grep '[-]dram_latency' "$config" | cut -d' ' -f2)
line=$(grep '[-]gpgpu_cache:dl2 ' "$config" | cut -d' ' -f2 | cut -d':' -f3) # format in config: S:64:128:16 - need 2nd 

# STEP 6: combine everything

echo '  -- Combining all intermediary outputs --  '
python3 "$here/merge.py" "$TMPDIR/${out}_basics.csv" "$TMPDIR/${out}_access.csv" \
  "$TMPDIR/${out}_ctrl.csv" "$TMPDIR/${out}_reuse.csv" "$dram" "$l2" "$line" >"$out.csv"
