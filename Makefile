
CFLAGS := -std=c11
CFLAGS += -Werror
CFLAGS += -Wall
CFLAGS += -pedantic
CFLAGS += $(shell sdl-config --cflags)

INCLUDE := -I ./include

LIB := -lc
LIB += -lm
LIB += $(shell sdl-config --libs)
LIB += -lSDL_image

SRC := $(wildcard src/*.c)

OBJ := $(SRC:.c=.o)

OUT := skane

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LIB)

all: $(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $(LIB)

clean:
	rm $(OBJ) $(OUT)
