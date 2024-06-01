# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++2b -Wall -Wextra

# Linker flags
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Source files
SRC = src/Agent.cpp src/SimulatedAnnealing.cpp src/Visualization.cpp src/main.cpp src/Utils.cpp

# Object files directory
OBJDIR = build

# Object files
OBJ = $(SRC:src/%.cpp=$(OBJDIR)/%.o)

# Executable
EXEC = mapf_visualizer

# Default target
all: $(EXEC)

# Create the object files directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Link object files to create the executable
$(EXEC): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(EXEC)
	rm -rf $(OBJDIR)

.PHONY: all clean
