FUNCTION_UNDER_TEST := cbrt

include ../support/Makefile.univariate

all:: check_special

check_special: check_special.o $(CORE_MATH_OBJS) $(FUNCTION_UNDER_TEST)_mpfr.o
	$(CC) $(LDFLAGS) -fopenmp -o $@ $^ -lmpfr -lm

check_special.o: check_special.c
	$(CC) $(CFLAGS) -fopenmp $(CORE_MATH_DEFINES) -c -o $@ $<

clean::
	rm -f check_special
