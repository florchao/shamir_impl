# Makefile

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Iinclude -fsanitize=address

# Directories
SRCDIR := src
BUILDDIR := build
OBJDIR := $(BUILDDIR)/obj
DISTRIBUTE_DIR := $(SRCDIR)/Distribute
RECOVER_DIR := $(SRCDIR)/Recover
UTILS_DIR := $(SRCDIR)/Utils
BMP_DIR := $(SRCDIR)/Bmp


# Source files
SOURCES := $(wildcard $(SRCDIR)/*.c) $(wildcard $(DISTRIBUTE_DIR)/*.c) $(wildcard $(RECOVER_DIR)/*.c ) $(wildcard $(BMP_DIR)/*.c) $(wildcard $(UTILS_DIR)/*.c)
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