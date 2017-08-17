CC = g++

CFLAGS = -Wall

TARGET = pbf

RM = rm

RM-F = rm -f

STD = -std=c++11

MP = -fopenmp

GL = -lSOIL -lGL -lGLU -lglut -lglui -lGLEW -lpng -fpermissive

DICTORY = -I/usr/local/include -I/./glui/

SOURCE = Shader.cpp texture.cpp Data.cpp FileLoader.cpp ColorFunc.cpp DrawFunc.cpp main.cpp PCA_Cluster.cpp

HEADER = Shader.h texture.h Data.h FileLoader.h ColorFunc.h DrawFunc.h PCA_Cluster.h

OBJECT = $(SOURCE:.cpp = .o)

all: $(SOURCE) $(TARGET)

clean: 
	$(RM) $(TARGET) 

$(TARGET): $(OBJECT)
	$(CC) $(OBJECT) -o $@ $(MP) $(GL)

.cpp.o: $(HEADER)
	$(CC) $(CFLAGS) $(DICTORY) $< -c -o $@ $(MP) $(GL)


