from multiprocessing import Process
from multiprocessing import Pool
import os
import sys

if len(sys.argv) < 3:
    print("Usage: python sim.py <benchmark_dir> <output_dir> [--eip]", file=sys.stderr)
    sys.exit(1)

benchmark_dir = sys.argv[1]
output_dir = sys.argv[2]
flags = sys.argv[3:]

if not os.path.isdir(benchmark_dir):
    print(f"Error: benchmark_dir '{benchmark_dir}' is not a valid directory", file=sys.stderr)
    sys.exit(1)

if benchmark_dir.endswith("/"):
    benchmark_dir = benchmark_dir[:-1]

if output_dir.endswith("/"):
    output_dir = output_dir[:-1]

if flags and flags[0] != "--eip":
    print(f"Error: unrecognized flag '{flags[0]}'. Only '--eip' is supported.", file=sys.stderr)
    sys.exit(1)

if flags and flags[0] == "--eip":
    run_eip = True
else:
    run_eip = False    

def run(arg: tuple):
    benchmark = arg
    if run_eip:
        cmd=f'bash run_gem5_eip.sh {benchmark_dir} {benchmark} {output_dir}'
    else:   
        cmd=f'bash run_gem5.sh {benchmark_dir} {benchmark} {output_dir}'
    # print(f"Running command: {cmd}")
    os.system(cmd)

if __name__ == "__main__": # Here
    
    benchmarks = []

    benchmarks += [filename for filename in os.listdir(benchmark_dir) if "bzip2_m1" not in filename and "mcf_m1" not in filename and "xz_m1" not in filename and "data-serving" not in filename and "emissary" not in filename and "perlbench" not in filename and "cassandra" not in filename]
    # benchmarks += [filename for filename in os.listdir(benchmark_dir)]


    pool = Pool()
    pool.map(run, benchmarks)
    pool.close()

   
