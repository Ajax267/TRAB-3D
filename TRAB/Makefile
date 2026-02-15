CXX = g++ -std=c++17
CFLAGS = -g -Wall
LINKING = -lglut -lGL -lGLU

SOURCES = $(wildcard *.cpp *.h)
TARGET = trabalhocg

all: $(TARGET)

# Compila todos os .cpp em um único executável
$(TARGET): $(SOURCES)
	$(CXX) $(CFLAGS) -o $@ $^ $(LINKING)

clean:
	rm -f $(TARGET)
