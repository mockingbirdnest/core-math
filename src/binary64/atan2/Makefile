FUNCTION_UNDER_TEST := atan2

include ../support/Makefile.bivariate

check_special: check_special.o $(CORE_MATH_OBJS) $(FUNCTION_UNDER_TEST)_mpfr.o
	$(CC) $(LDFLAGS) -fopenmp -o $@ $^ -lmpfr -lm

check_special.o: check_special.c
	$(CC) $(CFLAGS) $(CORE_MATH_DEFINES) -I../support -c -fopenmp -o $@ $<

clean_all: clean
	rm -f *.out
