# I want it to be able to compile when I type make all and to clean every object file and executable when I type make clean. It is worth noting that I am using the C programming language and compiling with gcc. The structure is as follows: there is a src directory where I have a main.c and utils.c files, also inside that I have a directory called Distribute and a directory called Recover with more source files with .c extension. Outside of the src directory, I have a directory called include with all the .h files. I want the name of the exectuable file to be "ss" and I want all the .o that are created to be stored in a directory called build that is in the same level of the include and src directories.

# Makefile

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Iinclude #-fsanitize=address

# Directories
SRCDIR := src
BUILDDIR := build
OBJDIR := $(BUILDDIR)/obj
DISTRIBUTE_DIR := $(SRCDIR)/Distribute
RECOVER_DIR := $(SRCDIR)/Recover
BMP_DIR := $(SRCDIR)/Bmp


# Source files
SOURCES := $(wildcard $(SRCDIR)/*.c) $(wildcard $(DISTRIBUTE_DIR)/*.c) $(wildcard $(RECOVER_DIR)/*.c ) $(wildcard $(BMP_DIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

# Target executable
TARGET := ss

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

# Cleaning generated files
clean:
	rm -rf $(BUILDDIR) $(TARGET)