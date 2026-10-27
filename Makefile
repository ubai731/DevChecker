
CXX         := g++
CXX_FLAGS   := -Wall -Wextra -ggdb -MMD -MP

BIN         := bin
SRC         := src
INCLUDE     := include
LIB         := lib
LIBRARIES   := 
EXECUTABLE  := main

SRC_EXT     := cpp
OBJ_EXT     := o
DEP_EXT     := d

SOURCES     := $(wildcard $(SRC)/*.$(SRC_EXT))
OBJECTS     := $(SOURCES:$(SRC)/%.$(SRC_EXT)=$(BIN)/%.$(OBJ_EXT))
DEPS        := $(OBJECTS:.$(OBJ_EXT)=.$(DEP_EXT))

.PHONY: all clean run

all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

$(BIN)/%.$(OBJ_EXT): $(SRC)/%.$(SRC_EXT)
	@mkdir -p $(BIN)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -c $< -o $@

clean:
	-rm -f $(BIN)/*.$(OBJ_EXT) $(BIN)/*.$(DEP_EXT) $(BIN)/$(EXECUTABLE)

-include $(DEPS)
        