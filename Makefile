CC = gcc
SRC = main.c
RAYLIB_PATH = $(HOME)/software/raylib/src/
LDLIBS = -L$(RAYLIB_PATH) -lraylib -lGL -lm -ldl -lpthread
INCLUDE = -I$(RAYLIB_PATH)
DEFINES =
CAT_DEBUG ?= 0
OUT = main

ifeq ($(CAT_DEBUG),1)
	DEFINES+=-DCAT_DEBUG
endif

default:
	$(CC) $(SRC) $(LDLIBS) $(INCLUDE) $(DEFINES) -o $(OUT)

clean:
	@rm $(OUT)

run: default
	@./$(OUT)
	@rm $(OUT)

