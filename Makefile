CFLAGS = -O3 -mfma -march=native -mavx2 -malign-double -fstrict-aliasing
# CFLAGS = -std=c++11 -O3 -mfma -march=native -mavx2 -malign-double -fstrict-aliasing -fno-omit-frame-pointer
# CFLAGS = -std=c++11 -O0 -g -fno-omit-frame-pointer


NVCFLAGS =
# NVCFLAGS = -g -G -O0

CC = g++
NVCC = nvcc

CCFLAGS = \
					-Wconversion                      \
					-Wsign-conversion                 \
					-Wfloat-conversion                \
					-Wcast-align=strict               \
					-Wcast-function-type              \
 					-Wall                             \
 					-Wpedantic                        \
					-Wstrict-aliasing                 \
					-Wno-deprecated-declarations      \
					-Wclobbered                       \
					-Wcast-function-type              \
					-Wdeprecated-copy                 \
					-Wempty-body                      \
					-Wignored-qualifiers              \
					-Wimplicit-fallthrough=3          \
					-Wmissing-field-initializers      \
					-Wsign-compare                    \
					-Wredundant-move                  \
					-Wtype-limits                     \
					-Wuninitialized                   \
					-Wshift-negative-value            \
					-Wunused-parameter                \
					-Wunused-but-set-parameter        \
					-std=c++11                        \
 					-Werror


NVCCFLAGS = -std=c++11 -I /usr/local/cuda/include/ -arch=sm_61 -lcudadevrt -rdc=false -lz -lconfig++

export CFLAGS
export CCFLAGS
export NVCFLAGS
export NVCCFLAGS
export CC
export NVCC

NPROCS = $(shell grep -c 'processor' /proc/cpuinfo)
MAKEFLAGS += -j$(NPROCS)

.PHONY: clean check all

all: dir common modules cli

dir:
	mkdir -p bin
	mkdir -p build
	mkdir -p build/common
	mkdir -p build/ProblemFunctions_CPU
	mkdir -p build/ProblemFunctions_GPU
	mkdir -p build/SolverAlgorithms/nFWA
	mkdir -p build/SolverAlgorithms/PSO
	mkdir -p build/SolverAlgorithms/oFWA
	mkdir -p build/cli

common: dir
	$(MAKE) -C common

cli: dir common modules
	$(MAKE) -C cli

modules: dir common modules_PF modules_PSO modules_nFWA modules_oFWA

modules_PF: dir common
	$(MAKE) -C ProblemFunctions_CPU
	$(MAKE) -C ProblemFunctions_GPU

modules_nFWA: dir common
	$(MAKE) -C SolverAlgorithms/nFWA

modules_oFWA: dir common
	$(MAKE) -C SolverAlgorithms/oFWA

modules_PSO: dir common
	$(MAKE) -C SolverAlgorithms/PSO

check:
	find ./ \( -name "*.cu" -o -name "*.cpp" -o -name "*.h" -o -name "*.cuh" \) -not -path "./Lib/*" > ./build/cppcheck_files
	cppcheck --file-list=./build/cppcheck_files --cppcheck-build-dir=./build/cppcheck/ --enable=all --std=c++11 --platform=unix64 \
	         --report-progress --language=c++ --suppress=functionConst --suppress=noOperatorEq --suppress=unusedFunction \
	         --suppress=noCopyConstructor --suppress=syntaxError --output-file=cppcheck.txt ./

clean:
	rm -rf build/
	rm -f bin/cli
