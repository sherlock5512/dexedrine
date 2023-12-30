# Define the variables for the compiler and the flags
CC = gcc
CFLAGS = -Wall -g

# Define the libraries to link
LIBS = -lsystemd -lbsd

# Define the source file and the output binary
BIN = dexedrine
SRC := $(BIN).c

# Define the default rule to build the program
all: $(BIN)

# Define the rule to compile the source file
$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LIBS)

# Define the rule to clean the generated files
clean:
	rm -f $(BIN)
