# Define compiler and flags
CC = gcc
CFLAGS = -lSDL2

# Define the target binary
TARGET = notify

# Define the source file
SRC = notify.c

# Define all the targets
all: $(TARGET)

$(TARGET): $(SRC) alarm.h
	$(CC) -o $@ $^ $(CFLAGS)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean install

