CXX = g++-14
CXXFLAGS = -std=c++23 -Wall -Wextra
TARGET = logparser
SOURCES = main.cpp $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)