CC     = g++

BUILD_DIR = build

# Include debug-symbols in release builds
MINISAT_RELSYM ?= -g

# General flags
G_FLAGS  = -O3 -std=c++11 -D NDEBUG -w

SAT_SRCS = $(wildcard sources/yasat/*.cpp)
SAT_OBJS = $(addprefix build/, $(SAT_SRCS:.cpp=.o))
SRCS     = $(wildcard sources/*.cpp)
OBJS     = $(addprefix build/, $(SRCS:.cpp=.o))

C_FLAGS  = $(G_FLAGS) -I. -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS$(MINISAT_RELSYM)
L_FLAGS  = $(G_FLAGS) -lz $(MINISAT_RELSYM)

# Copile minisat
$(BUILD_DIR)/%.o: %.cpp
	@echo Compiling: $@
	@mkdir -p $(dir $@)
	@$(CC) $(C_FLAGS) -c -o $@ $<

# Compile our code
$(BUILD_DIR)/%.o: %.cpp
	@echo Compiling: $@
	@mkdir -p $(dir $@)
	@$(CC) $(C_FLAGS) -c -o $@ $<

# Linking rule
$(BUILD_DIR)/CAD2016: $(SAT_OBJS) $(OBJS)
	@echo Linking Binary: $@
	@mkdir -p $(dir $@)
	@$(CC) $(L_FLAGS) -o $@ $^

clean:
	rm -r build/*
