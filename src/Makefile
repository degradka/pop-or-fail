BUILDTYPE ?= DEBUG

LDFLAGS := -lraylib

ifeq ($(BUILDTYPE), DEBUG)
	CFLAGS := -g
	EXT := _d
endif
ifeq ($(BUILDTYPE), RELEASE)
	CFLAGS := -O2 -s
	EXT := 
endif


ifeq ($(OS), Windows_NT)
	# -Wl,--subsystem,windows hides the console window
	LDFLAGS += -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,windows
	EXT := $(EXT).exe
else
	LDFLAGS += -lGL -lm -lpthread -ldl -lrt -lX11
endif

pop-or-fail$(EXT): main.c
	gcc $< $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -fv pop-or-fail*