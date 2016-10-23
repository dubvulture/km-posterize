CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic $(OPT)
CCOMPILE = -fpic -c -Isrc/includes/
CSHARED = -shared -o
CLINK = -lm
OPT =

SOURCEDIR = ./src
BUILDDIR = ./build

SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

all: dir wrapper.so

dir:
	mkdir -p $(BUILDDIR)

wrapper.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(CSHARED) $@ $^ $(CLINK)

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) $(CCOMPILE) -o $@ $^


clean:
	rm -rf $(BUILDDIR)
