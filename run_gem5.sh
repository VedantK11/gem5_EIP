# export M5_PATH=$(pwd)/bin/m5
# GEM5_HOME=$(pwd)/gem5
# GEM5_CFG=$GEM5_HOME/configs/example/arm/starter_fs.py

# TEST=test
# CKPT_DIR=$(pwd)/checkpoints/${TEST}

# OUTDIR=sim_outs/m5out.$TEST
# mkdir -p $OUTDIR
# touch ${OUTDIR}

# $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR} --debug-file=debug.insts  $GEM5_CFG -I 100000000000 --disk-image="${CKPT_DIR}/ubuntu-arm.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type AtomicSimpleCPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB


export M5_PATH=$(pwd)/bin/m5
GEM5_HOME=$(pwd)/gem5
GEM5_CFG=$GEM5_HOME/configs/example/arm/starter_fs.py
# ALL_CKPT_DIR=/home/vedant/DatacenterTraces
ALL_CKPT_DIR=/new_disk/DatacenterTraces

CKPT_DIR=$ALL_CKPT_DIR/$1
echo $CKPT_DIR

OUTDIR=sim_outs/m5out.$2/$1
mkdir -p $OUTDIR
touch ${OUTDIR}

# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR} --debug-file=debug.insts  $GEM5_CFG --m1 -I 100000000 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then


# #Ideal l1i
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR} --debug-file=debug.insts  $GEM5_CFG --m1 --l2_rp=LRUEmissary -I 10000 -W 50000  --ftqSize=24  --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16  --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE  --perfectICache --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt; then



#EMISSARY RP
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR} --debug-file=debug.insts  $GEM5_CFG --m1  -W 500000 -I 10000000  --ftqSize=24 --btb-entries=65536  --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --l2_rp=LRUEmissary --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB  > ${OUTDIR}/out.txt 2>&1; then 

# LRU RP
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR} --debug-file=debug.insts  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24 --btb-entries=65536  --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then 

# #Zen 2
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24 --btb-entries=65536  --l1i_size=32kB --l1d_size=32kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=512kB --l2_assoc=8 --l3_size=2MB --l3_assoc=16 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then 
#Zen 3
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 10000000 -I 200000000  --ftqSize=24 --btb-entries=65536  --l1i_size=32kB --l1d_size=32kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=512kB --l2_assoc=8 --l3_size=2MB --l3_assoc=16 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then

#Neoverse
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24 --btb-entries=65536  --l1i_size=64kB --l1d_size=64kB --l1i_assoc=4 --l1d_assoc=4 --l2_size=512kB --l2_assoc=8 --l3_size=2MB --l3_assoc=16 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then 


#Alderlake
#NO FDIP--->># if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000   --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU  --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then 
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24 --btb-entries=65536  --l1i_size=64kB --l1d_size=64kB --l1i_assoc=4 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24  --btb-entries=16384  --l1i_size=64kB --l1d_size=48kB --l1i_assoc=8 --l1d_assoc=12 --l2_size=2MB --l2_assoc=16 --l3_size=3MB --l3_assoc=12  --l2_rp=DCLIP --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24 --btb-entries=16384  --l1i_size=64kB --l1d_size=48kB --l1i_assoc=8 --l1d_assoc=12 --l2_size=2MB --l2_assoc=16 --l3_size=3MB --l3_assoc=12   --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then
#Redwood Cove
# if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24  --btb-entries=16384  --l1i_size=64kB --l1d_size=48kB --l1i_assoc=8 --l1d_assoc=12 --l2_size=2MB --l2_assoc=16 --l3_size=3MB --l3_assoc=12 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then
if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24 --btb-entries=16384 --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then
   echo "###--->$CKPT_DIR executed succesfully <---#####"
   rm -rf "${OUTDIR}/trace.out.gz"
else 
   echo "***$CKPT_DIR not executed***"
fi

# gdb --args $(pwd)/gem5/build/ARM/gem5.opt  --outdir=sim_outs/m5out.eip/qemu.ckpt.tomcat_isolcpu  --debug-file=trace.out.gz  $(pwd)/gem5/configs/example/arm/starter_fs.py --m1  -W 500000 -I 10000000  --ftqSize=24 --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --disk-image="/home/vedant/DatacenterTraces/qemu.ckpt.tomcat_isolcpu/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "/home/vedant/DatacenterTraces/qemu.ckpt.tomcat_isolcpu/" --num-cores 1 -n 1 --mem-size 16384MiB > sim_outs/m5out.eip/qemu.ckpt.tomcat_isolcpu/out.txt 2>&1
# gdb --args $(pwd)/gem5/build/ARM/gem5.opt  --outdir=sim_outs/m5out.eip/qemu.ckpt.benchbase_tpcc_test2  --debug-file=trace.out.gz  $(pwd)/gem5/configs/example/arm/starter_fs.py --m1  -W 5000000 -I 100000000  --ftqSize=24 --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --disk-image="/home/vedant/DatacenterTraces/qemu.ckpt.tomcat_isolcpu/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "/home/vedant/DatacenterTraces/qemu.ckpt.benchbase_tpcc_test2/" --num-cores 1 -n 1 --mem-size 16384MiB > sim_outs/m5out.eip_noSquash/qemu.ckpt.benchbase_tpcc_test2/out.txt 2>&1
# gdb --args $(pwd)/gem5/build/ARM/gem5.opt  --outdir=sim_outs/m5out.eip_asplos25/qemu.ckpt.finagle-chirper_test  --debug-file=trace.out.gz  $(pwd)/gem5/configs/example/arm/starter_fs.py --m1  -W 500000 -I 10000000  --ftqSize=24 --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --disk-image="/new_disk/DatacenterTraces/qemu.ckpt.finagle-chirper_test/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "/new_disk/DatacenterTraces/qemu.ckpt.finagle-chirper_test/" --num-cores 1 -n 1 --mem-size 16384MiB > sim_outs/m5out.eip/qemu.ckpt.finagle-chirper_test/out.txt 2>&1