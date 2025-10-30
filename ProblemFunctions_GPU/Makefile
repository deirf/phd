NAME = ProblemFunctions_GPU
BUILDDIR = ../build/${NAME}
OUT = ${BUILDDIR}/${NAME}.a

_CCFLAGS = $(CCFLAGS) -I /usr/local/cuda/include

DEPS = *.h
CPP = $(wildcard *.cpp)
OBJ = $(addprefix ${BUILDDIR}/,$(notdir $(CPP:.cpp=.o)))

NDEPS = *.cuh
NCPP = $(wildcard *.cu)
NOBJ = $(addprefix ${BUILDDIR}/,$(notdir $(NCPP:.cu=.o)))

$(OUT): $(OBJ) $(NOBJ)
	ar rvs $(OUT) $^

${BUILDDIR}/%.o: %.cpp $(DEPS)
	$(CC) $(_CCFLAGS) -c -o $@ $< $(CFLAGS)

${BUILDDIR}/%.o: %.cu $(NDEPS)
	$(NVCC) $(NVCCFLAGS) -c -o $@ $< $(NVCFLAGS)

.PHONY: clean

clean:
	rm -f ${BUILDDIR}/*.o
	rm -f ${BUILDDIR}/*.a
