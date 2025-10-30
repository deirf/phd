NAME = common
BUILDDIR = ../build/${NAME}
OUT = ${BUILDDIR}/${NAME}.a

_CCFLAGS = ${CCFLAGS} -lconfig++

DEPS = *.h
CPP = $(wildcard *.cpp)
OBJ = $(addprefix ${BUILDDIR}/,$(notdir $(CPP:.cpp=.o)))

$(OUT): $(OBJ)
	ar rvs $(OUT) $^

${BUILDDIR}/%.o: %.cpp $(DEPS)
	$(CC) $(_CCFLAGS) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f ${BUILDDIR}/*.o
	rm -f ${BUILDDIR}/*.a
