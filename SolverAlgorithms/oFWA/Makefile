NAME = oFWA
BUILDDIR = ../../build/SolverAlgorithms/${NAME}
OUT = ${BUILDDIR}/${NAME}.a

DEPS = *.h
CPP = $(wildcard *.cpp)
OBJ = $(addprefix ${BUILDDIR}/,$(notdir $(CPP:.cpp=.o)))

$(OUT): $(OBJ)
	ar rvs $(OUT) $^

${BUILDDIR}/%.o: %.cpp $(DEPS)
	$(CC) $(CCFLAGS) -c -o $@ $< $(CFLAGS)


.PHONY: clean

clean:
	rm -f ${BUILDDIR}/*.o
	rm -f ${BUILDDIR}/*.a
