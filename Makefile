OS := $(shell uname)

ifeq ($(OS),Darwin)
  EXT := dylib
  SOFLAGS := -dynamiclib
else
  EXT := so
  SOFLAGS := -fPIC -shared
endif

.PHONY: all filename quiet

all: attach_gdb.$(EXT)

quiet: all
	@:

attach_gdb.$(EXT): attach_gdb.c
	@cc -pthread $(SOFLAGS) -g $^ -o $@

test: test.c
	@cc -g $^ -o $@

segfaulty: segfaulty.c
	@cc -g $^ -o $@

clean:
	rm -f attach_gdb.$(EXT) test segfaulty

filename: attach_gdb.$(EXT)
	@echo $(realpath $^)
