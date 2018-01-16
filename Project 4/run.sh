#run.sh
/usr/local/cuda-9.1/bin/nvcc angle.cu -o angle;#
./angle 1000000 32
./angle 1000000 64
./angle 1000000 128
./angle 1000000 256
./angle 1000000 512
./angle 5000000 32
./angle 5000000 64
./angle 5000000 128
./angle 5000000 256
./angle 5000000 512
./angle 10000000 32
./angle 10000000 64
./angle 10000000 128
./angle 10000000 256
./angle 10000000 512
