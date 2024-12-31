CC = gcc
TARGETS = shell pipe_ls2wc

all: $(TARGETS)

.PHONY: all clean

%: %.c
	$(CC) $< -o $@

clean:
	rm -f $(TARGETS) myfile