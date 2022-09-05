CXXFLAGS= -O3 -s -I./include -L./bin -lglfw3 -lopengl32
SRC_DIR:=$(wildcard ./src/*.cpp)
SRC_OBJ:=$(patsubst %.cpp,%.o,$(SRC_DIR))
SRC_DEP:=$(patsubst %.cpp,%.d,$(SRC_DIR))

LIB_DIR:=$(wildcard ./lib/*.cpp)
LIB_OBJ:=$(patsubst %.cpp,%.o,$(LIB_DIR))

all:$(LIB_OBJ) $(SRC_OBJ)
	@echo main.exe
	@g++ $(SRC_OBJ) $(LIB_OBJ) $(CXXFLAGS) -o ./bin/main && ./bin/main.exe

%.o:%.cpp
	@echo $<
	@$(CXX) -std=c++17 $(CXXFLAGS) -c -o $@ $<
clean:
	rm ./src/*.o ./lib/*.o ./bin/*.exe

run:
	./bin/main.exe
