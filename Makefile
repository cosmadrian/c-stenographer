CC=gcc
CFLAGS=-c -Wall 
SRC=sten.c
OBJ=$(SRC:.c=.o)
EXE=hideme


all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -lmcrypt -o $@

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC)

build:
	make $(EXE)

run_1: $(EXE)
	./$(EXE) -e "Omul este mic, dar mare e Allah." -i testing 

run_2: $(EXE)
	./$(EXE) -e "bunica bate toba, bunica bate tare." -i testing

run_3: $(EXE)
	./$(EXE) -e "Should I kill myself, or have a cup of coffee?" -i testing -k "mamamamamamamama"

run_4: $(EXE)
	./$(EXE) -d -i testing

test_file:
	for i in `seq 1 1000`; do printf '/' >> testing; done

clean:
	rm -fr $(OBJ) 
	rm -fr testing
