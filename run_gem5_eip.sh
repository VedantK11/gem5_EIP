export M5_PATH=$(pwd)/bin/m5
GEM5_HOME=$(pwd)/gem5
GEM5_CFG=$GEM5_HOME/configs/example/arm/starter_fs.py

if [ "$#" -ne 3 ]; then
      echo "Usage: $0 <benchmark_dir> <benchmark_name> <output_dir>"
      exit 1
fi

ALL_CKPT_DIR=$1
CKPT_DIR=$ALL_CKPT_DIR/$2
echo $CKPT_DIR

OUTDIR=sim_outs/m5out.$3/$2
mkdir -p $OUTDIR
touch ${OUTDIR}

if $GEM5_HOME/build/ARM/gem5.opt  --outdir=${OUTDIR}  --debug-file=trace.out.gz  $GEM5_CFG --m1  -W 5000000 -I 100000000  --ftqSize=24 --btb-entries=16384 --l1i_size=32kB --l1d_size=64kB --l1i_assoc=8 --l1d_assoc=8 --l2_size=1MB --l2_assoc=16 --l3_size=2MB --l3_assoc=16 --l1i-hwp-type=EIPPrefetcher --disk-image="${CKPT_DIR}/ubuntu-image.img" --bootloader="${M5_PATH}/binaries/boot_v2_qemu_virt.arm64" --caches --cpu-type O3CPU --fdip --bp-type TAGE --restore "${CKPT_DIR}" --num-cores 1 -n 1 --mem-size 16384MiB > ${OUTDIR}/out.txt 2>&1; then
   echo "###--->$CKPT_DIR executed succesfully <---#####"
   rm -rf "${OUTDIR}/trace.out.gz"
else 
   echo "***$CKPT_DIR not executed***"
fi