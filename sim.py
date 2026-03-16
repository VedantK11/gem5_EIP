# from multiprocessing import Process
# from multiprocessing import Pool
# import os
# import sys

# if len(sys.argv) != 2:
#     print("<file name>")
#     sys.exit()

# warmup = int(50*1e6)
# actual = int(50*1e6)

# folder = sys.argv[1]

# def run(arg):
#     cmd = f'bin/{folder} --warmup-instructions {warmup} --simulation-instructions {actual} /data/hrishikesh/Converted_traces/{arg[1]}/All_imps/{arg[0]} > results/{folder}/{arg[0]}.txt'
#     os.system(cmd)

# if __name__ == "__main__": # Here
    
#     os.system(f'./config.sh champsim_config.json')
#     os.system(f'make')
#     os.system(f'mv bin/champsim bin/{folder}')
#     os.system(f'rm -r results/{folder}')
#     os.system(f'mkdir results/{folder}')
#     os.system(f'cp champsim_config.json results/{folder}/')

#     traces = []
#     traces += [(filename, "CVP1public") for filename in os.listdir(f'/data/hrishikesh/Converted_traces/CVP1public/All_imps/')]
#     traces += [(filename, "IPC1") for filename in os.listdir(f'/data/hrishikesh/Converted_traces/IPC1/All_imps/')]

#     pool = Pool()
#     pool.map(run, traces)
#     pool.close()

from multiprocessing import Process
from multiprocessing import Pool
import os
import sys

# if len(sys.argv) != 2:
#     print("<folder name>")
#     sys.exit()

# warmup = 50000000
# actual = 50000000

folder = sys.argv[1]
# folder = 'ideal-instr-hits-l2c'

def run(arg: tuple):
    trace = arg
    # cmd = f'bin/champsim --warmup-instructions {warmup} --simulation-instructions {actual} /home/vedant/Converted_traces/All_traces/{trace} > /home/vedant/initial/comp-arch/develop-champsim/results/{folder}/{trace}.txt'
    # cmd = f'bin/champsim --warmup-instructions {warmup} --simulation-instructions {actual} /home/ravi/Converted_traces/All_traces/{trace} > /home/ravi/vedant/comp-arch/develop-champsim/results/{folder}/{trace}.txt'
    cmd=f'bash run_gem5.sh {trace} {folder}'
    os.system(cmd)

if __name__ == "__main__": # Here
    
    # os.system(f'./config.sh champsim_config.json')
    # os.system(f'make')  
    # print("Make done")

    # os.system(f'mkdir -p  /home/vedant/initial/comp-arch/develop-champsim/results/{folder}')
    # os.system(f'cp champsim_config.json /home/vedant/initial/comp-arch/develop-champsim/results/{folder}')

    # os.system(f'mkdir -p  /home/ravi/vedant/comp-arch/develop-champsim/results/{folder}')
    # os.system(f'cp champsim_config.json /home/ravi/vedant/comp-arch/develop-champsim/results/{folder}')

    traces = []

    # os.system(f'mkdir -p /home/vedant/initial/results/{folder}')
    # traces += [filename for filename in os.listdir(f'/home/ravi/Converted_traces/All_traces/')  if   "tpc" not in filename and "cassandra" not in filename ]
    # traces += [filename for filename in os.listdir(f'/home/vedant/DatacenterTraces') ]
    # traces += [filename for filename in os.listdir(f'/home/vedant/DatacenterTraces') if "bzip2_m1" not in filename and "mcf_m1" not in filename and "xz_m1" not in filename and "data-serving" not in filename and "emissary" not in filename and "perlbench" not in filename and "cassandra" not in filename]
    traces += [filename for filename in os.listdir(f'/new_disk/DatacenterTraces') if "bzip2_m1" not in filename and "mcf_m1" not in filename and "xz_m1" not in filename and "data-serving" not in filename and "emissary" not in filename and "perlbench" not in filename and "cassandra" not in filename]


    pool = Pool()
    pool.map(run, traces)
    pool.close()
    # print(traces)

   
