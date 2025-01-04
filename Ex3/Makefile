CC = gcc
TARGETS = shell pipe_ls2wc

all: $(TARGETS)

.PHONY: all clean

shell: shell.o utils.o
	$(CC) $^ -o $@

shell.o: shell.c utils.h
	$(CC) -c $< -o $@

utils.o: utils.c utils.h
	$(CC) -c $< -o $@

pipe_ls2wc: pipe_ls2wc.c
	$(CC) $< -o $@

clean:
	rm -f $(TARGETS) myfile *.o