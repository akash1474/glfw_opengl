> Terminal Command
	g++ main.cpp ./glad.c -I./include -L./lib -o main -lglfw3 -lopengl32

* Explanation *
	lib(folder)  -> all dll/lib files
	include(folder) -> all the header files

	Here I downloaded glad from it's official website (v3.3 core opengl)
	You can download the binaries directly from the website but here I compiled the source code that I downloaded from the official website using cmake-gui(makefile mingw) the I moved the dll file to lib after build the makefile

	I moved all the files in include to the current_folder/include
	Here I also compile the glad.c with main.cpp because it is not compiled