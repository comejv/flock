CC=gcc

### Parameters ###
DEBUG := OFF
$(info DEBUG is $(DEBUG))

PLUG := ON
$(info PLUG is $(PLUG))

MAKEFLAGS := --jobs=$(shell nproc) --output-sync=target
$(info Running with $(shell nproc) jobs.)

### Directories ###
PLUGDIR := plug
BINDIR := bin
OBJDIR := $(BINDIR)/obj
SRCDIR := src
HEADERSDIR := headers
LIBDIR := lib

### Compilation and linker flags ###
CFLAGS := -Wall
CPPFLAGS := -I$(LIBDIR)/raylib/src -I$(HEADERSDIR)
LDFLAGS :=

ifeq ($(DEBUG), ON)
	CFLAGS += -g
endif

RAYLIB_MAKE_FLAGS := $(MAKEFLAGS) PLATFORM=PLATFORM_DESKTOP

PLUG_LDFLAGS := -Wl,-rpath=$(abspath $(BINDIR))
RAYLIB_IMPORT_FLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -Llib/raylib/src

ifeq ($(PLUG), ON)
	CFLAGS += -DPLUG
	LDFLAGS += -shared
	RAYLIB_MAKE_FLAGS += RAYLIB_LIBTYPE=SHARED
	RAYLIB_IMPORT_FLAGS += -Wl,-rpath=$(abspath $(LIBDIR)/raylib/src)
endif

### Files ###
PLUGSRCS := $(wildcard $(PLUGDIR)/*.c)
SRCS := $(wildcard $(SRCDIR)/*.c)
HEADERS := $(wildcard $(HEADERSDIR)/*.h)
OBJS := $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

### Targets ###
MODULE_NAME := flock.so
MODULE_PATH := $(BINDIR)/$(MODULE_NAME)
EXEC := $(BINDIR)/flock

### Rules ###
.PHONY: all clean cleanall ensureDirs raylib help

all: ensureDirs $(EXEC)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

ifeq ($(PLUG), ON)
$(MODULE_PATH): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(RAYLIB_IMPORT_FLAGS)

$(EXEC): $(PLUGSRCS) $(MODULE_PATH)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -DMODULE_NAME=\"$(MODULE_NAME)\" $^ $(PLUG_LDFLAGS) $(RAYLIB_IMPORT_FLAGS)
else
$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(RAYLIB_IMPORT_FLAGS)
endif

### Raylib ###
raylib: ## Compile Raylib into a static library
	@$(MAKE) -C $(LIBDIR)/raylib/src $(RAYLIB_MAKE_FLAGS)

### Utils ###
ensureDirs:
	@mkdir -p $(OBJDIR) $(BINDIR)

clean: ## Remove object files in the bin directory.
	rm -rf $(OBJDIR)/*.o

cleanall: ## Remove the bin directory, the executable and clean libraries.
	rm -rf $(BINDIR)
	$(MAKE) -C $(LIBDIR)/raylib/src clean >/dev/null

help: ## Prints help for targets with comments
	@cat $(MAKEFILE_LIST) | grep -E '^[a-zA-Z_-]+:.*?## .*$$' | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
	@printf "\033[32m%-30s\033[0m %s\n" "PLUG {[ON]/OFF}" "ON for shared libraries and hot reload"


