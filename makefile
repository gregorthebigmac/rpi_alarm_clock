# OBJS specifies which files to compile as part of the project
OBJS = alarm.cpp

# CC specifies which compiler we're using
CC = g++

# COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w

# LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL_mixer

# Using SDL_CFLAGS and SDL_LDFLAGS
SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

# OBJ_NAME specifies the name of our executable
OBJ_NAME = alarm

# This is the target that compiles our executable
all: $(OBJS)
	g++ $(SDL_CFLAGS) $(SDL_LDFLAGS) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)