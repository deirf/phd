NAME = cli
OUT = ../bin/${NAME}
BUILDDIR = ../build/${NAME}
LIBDIR = ../build/

_NVCCFLAGS = $(NVCCFLAGS) -Wno-deprecated-gpu-targets -L /usr/local/cuda/lib64/

DEPS = *.h
CPP = $(wildcard *.cpp)
LIBS = ${LIBDIR}/SolverAlgorithms/nFWA/nFWA.a \
			 ${LIBDIR}/SolverAlgorithms/oFWA/oFWA.a \
			 ${LIBDIR}/SolverAlgorithms/PSO/PSO.a \
			 ${LIBDIR}/ProblemFunctions_CPU/ProblemFunctions_CPU.a \
			 ${LIBDIR}/ProblemFunctions_GPU/ProblemFunctions_GPU.a \
			 ${LIBDIR}/common/common.a
OBJ = $(addprefix ${BUILDDIR}/,$(notdir $(CPP:.cpp=.o)))

$(OUT): $(OBJ) ${LIBS}
	$(NVCC) $(_NVCCFLAGS) $(NVCFLAGS) -o $(OUT) $(OBJ) ${LIBS}

${BUILDDIR}/%.o: %.cpp $(DEPS)
	$(CC) $(CCFLAGS) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f ${BUILDDIR}/*.o
	rm -f ${BUILDDIR}/*.a
