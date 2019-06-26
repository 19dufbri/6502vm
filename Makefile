# Simple Makefile for the 6502 Emulator

IDIR   = ./include
ODIR   = ./bin
SDIR   = ./src
TDIR   = ./test
OUT    = 6502vm
CFLAGS = -std=c11 -I$(IDIR) -Wall
CC     = gcc
HEAD   = $(wildcard $(IDIR)/*.h)
OBJ    = $(ODIR)/6502.o $(ODIR)/linked_list.o $(ODIR)/mapper.o 
TEST   = $(patsubst $(TDIR)/%.c,$(ODIR)/%,$(wildcard $(TDIR)/*.c))

all: $(OUT)

$(ODIR)/%.o: $(SDIR)/%.c $(HEAD) $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(ODIR):
	mkdir $(ODIR)

run: all
	./$(OUT)

test: $(TEST)

install: all
	cp $(OUT) /usr/local/bin/$(OUT)

clean:
	rm -rf $(OUT) $(ODIR)

$(ODIR)/test_%.o: $(TDIR)/test_%.c $(HEAD) $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

.SECONDEXPANSION:
$(TEST): $$@.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
	./$@
