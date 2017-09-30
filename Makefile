CC?=gcc

# Build the vector shell.
vector-cli: vector.c cli.c
	$(CC) -o $@ $^
