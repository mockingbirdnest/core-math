UNAME_S := $(shell uname -s)

CXX              := clang++
COMPILER_OPTIONS := \
	-std=c++1z -stdlib=libc++ -O3 -g                           \
	-fPIC -fexceptions -ferror-limit=1 -fno-omit-frame-pointer \
	-Wall -Wpedantic                                           \
	-DNDEBUG -Iinclude -I../deps/abseil-cpp

LIBRARY_TRANSLATION_UNITS := \
	src/binary64/cos/cos.c \
	src/binary64/sin/sin.c
LIBRARY_OBJECTS := $(LIBRARY_TRANSLATION_UNITS:.c=.o)

ifeq ($(UNAME_S),Darwin)
    COMPILER_OPTIONS += -mmacosx-version-min=10.11 -arch x86_64
endif

all: $(LIBRARY_OBJECTS)
	ar -rcs libcore-math.a $^

clean:
	rm -f src/*/*/*.o

%.o: %.c
	$(CXX) -c -o $@ $< $(COMPILER_OPTIONS)
