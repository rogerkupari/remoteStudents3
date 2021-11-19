CC=gcc

all: converterModel

converterModel:
	$(CC) converterModelTest.c -o converterModelTest

clean:
	rm converterModelTest
