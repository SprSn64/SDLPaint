DIRS := src

CC := gcc
CFLAGS := -Wall -Wextra -O3 -Iinclude

ifeq ($(OS),Windows_NT)
	TARGET := sdlpaint
	LDFLAGS := -lSDL3 -lSDL3_image -lgdi32
else
	TARGET := sdlpaint.$(shell uname -m)
	LDFLAGS := -lm -lSDL3_image -lSDL3
endif

C_SOURCES   := $(wildcard $(addsuffix /*.c,$(DIRS)))

OBJECTS := \
	$(C_SOURCES:.c=.o) \

LINKER := $(CC)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
