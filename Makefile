CC=gcc
BIN=chip8
BIN_TEST=chip8_test
DEBUG=1
CVERSION=c99

UNAME=$(shell uname)
ifeq ($(findstring MINGW, $(UNAME)), MINGW)
	PLATFORM=WINDOWS
	WIN_CONSOLE=1
endif
ifeq ($(findstring Darwin, $(UNAME)), Darwin)
	PLATFORM=DARWIN
endif

INCLUDE= -I./include
CFLAGS= -MMD -std=${CVERSION} -Wall -Werror -DPLATFORM_$(PLATFORM)

ifneq ($(DEBUG), 0)
	CFLAGS+= -g -O0 -DDEBUG
endif

ifeq ($(PLATFORM), WINDOWS)
	BIN:=$(BIN).exe
	BIN_TEST:=$(BIN_TEST).exe
	ifneq ($(WIN_CONSOLE), 0)
		LIBS= -lgdi32 -lwinmm -limm32 -lversion -loleaut32 -lOle32 -luuid
	else
		LIBS= -mwindows
	endif
	SSE2 = -msse2 -DSSE2
	CFLAGS+= $(INCLUDE) -I./ext/include
	LDFLAGS= -L./ext/lib -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32 $(LIBS)
endif

ifeq ($(PLATFORM), DARWIN)
	CFLAGS+= $(INCLUDE) -I/usr/local/include -D_THREAD_SAFE
	LDFLAGS= -framework OpenGL -L/usr/local/lib -lSDL2 -lglew
endif

CFILES=\
	main.c

OFILES=$(addprefix obj/, $(notdir $(CFILES:.c=.o)))
DFILES=$(OFILES:.o=.d)

all: $(BIN)

test: $(BIN_TEST)

$(BIN): $(OFILES)
	$(CC) $(OFILES) $(LDFLAGS) -o $@

$(BIN_TEST): EXTRA_CFLAGS := -DUNIT_TESTS
$(BIN_TEST): $(OFILES)
	$(CC) $(OFILES) $(LDFLAGS) -o $@

obj/%.o: ./src/%.c | obj
	$(CC) -c $< $(CFLAGS) $(EXTRA_CFLAGS) -o $@

# -MMD generated header dependencies
-include obj/*.d

obj:
	mkdir ./obj

run: all
	./$(BIN)

clean:
	-rm $(OFILES) $(DFILES) $(BIN) $(BIN_TEST)
