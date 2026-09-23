CC = gcc
CFLAGS = -Wall -g

TARGET = user
OBJDIR = objects

SOURCES = client_main.c api.c parser.c commands.c validation.c
OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
HEADERS = api.h client_main.h commands.h messages.h parser.h validation.h

TESTS = tests/test_api tests/test_validation

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

# Crude but adequate at this size: rebuild every object if any header changes.
# OBJDIR is an order-only prerequisite (after the |) so that writing a file into
# the directory, which updates its timestamp, does not force a rebuild.
$(OBJDIR)/%.o: %.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

# The test binaries link only the modules they exercise, not the whole program
tests/test_api: tests/test_api.c api.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ tests/test_api.c api.c

tests/test_validation: tests/test_validation.c validation.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ tests/test_validation.c validation.c

test: $(TESTS)
	./tests/test_api
	./tests/test_validation

clean:
	rm -rf $(OBJDIR) $(TARGET) $(TESTS)

.PHONY: all test clean
