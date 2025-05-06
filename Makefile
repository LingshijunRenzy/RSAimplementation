CC = gcc

UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
  # Windows (MSYS2 MinGW64), allow custom install via MINGW_HOME
  ifeq ($(MINGW_HOME),)
    $(error MINGW_HOME not defined; please set it to your MinGW64 installation prefix)
  endif
  HOME_INC := -I$(MINGW_HOME)/include
  HOME_LIB := -L$(MINGW_HOME)/lib
else ifeq ($(UNAME_S),Darwin)
  HOME_INC := -I/opt/homebrew/include -I/usr/local/include
  HOME_LIB := -L/opt/homebrew/lib -L/usr/local/lib
else
  HOME_INC :=
  HOME_LIB :=
endif

# Attempt to detect GMP via pkg-config
GMP_CFLAGS := $(shell pkg-config --cflags gmp 2>/dev/null)
GMP_LIBS := $(shell pkg-config --libs gmp 2>/dev/null)
ifeq ($(strip $(GMP_CFLAGS)),)
  # Fallback include paths if pkg-config not available
  GMP_CFLAGS := -I/usr/include -I/usr/local/include
endif
ifeq ($(strip $(GMP_LIBS)),)
  # Fallback library paths if pkg-config not available
  GMP_LIBS := -L/usr/lib -L/usr/local/lib -lgmp
endif

CFLAGS = -Wall -O2 $(GMP_CFLAGS) $(HOME_INC)
LDLIBS = $(GMP_LIBS) $(HOME_LIB) -lgmp

TARGET = e3rsa

all: $(TARGET)

$(TARGET): main.o rsa.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o rsa.o $(LDLIBS)

main.o: main.c rsa.h
	$(CC) $(CFLAGS) -c main.c

rsa.o: rsa.c rsa.h
	$(CC) $(CFLAGS) -c rsa.c

clean:
	rm -f *.o $(TARGET)
	rm -rf generated_txts

.PHONY: test-encrypt test-sign

# Default encryption test: compare generated cipher with expected
test-encrypt: all
	@echo "Running encryption test..."
	@./e3rsa -p txts/plain.txt -n txts/n.txt -e txts/e.txt -c txts/cipher_out.txt
	@diff -u txts/cipher.txt txts/cipher_out.txt || (echo "Encryption test failed"; exit 1)
	@echo "Encryption test passed"

# Default signature test: compare generated signature with expected
test-sign: all
	@echo "Running signature test..."
	@./e3rsa -p txts/plain.txt -n txts/n.txt -d txts/d.txt -c txts/sign_out.txt
	@diff -u txts/sign.txt txts/sign_out.txt || (echo "Signature test failed"; exit 1)
	@echo "Signature test passed"
