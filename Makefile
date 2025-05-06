CC = gcc
CFLAGS = -Wall -O2 $(shell pkg-config --cflags gmp 2>/dev/null) -I/opt/homebrew/include -I/usr/local/include
LDLIBS = $(shell pkg-config --libs gmp 2>/dev/null) -L/opt/homebrew/lib -L/usr/local/lib -lgmp

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
