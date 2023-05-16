LIB_DIRS := /opt/homebrew/cellar/sfml/2.5.1_2/lib
LIB_NAME := sfml-graphics sfml-window sfml-system
LIB_FLAGS := $(addprefix -L, $(LIB_DIRS))
LIB_FLAGS += $(addprefix -l, $(LIB_NAME))

INC_DIRS := /opt/homebrew/Cellar/sfml/2.5.1_2/include
INC_FLAGS := $(addprefix -I, $(INC_DIRS))

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++11
CXXFLAGS += $(INC_FLAGS) $(LIB_FLAGS) -MMD -MP 

SOURCES := main.cpp
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: clean all
.DEFAULT_GOAL := all

all: game

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -c

game: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f *.o game *.d

-include $(OBJECTS:.o=.d)
