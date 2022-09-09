#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    GLFWwindow* window;
    // Initialize GLFW
    if (!glfwInit())
        exit(EXIT_FAILURE);



    // Tell GLFW what version of OpenGL we are using 
    // In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    int WIDTH=400;
    int HEIGHT=400;


    // Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
    window = glfwCreateWindow(WIDTH, HEIGHT, "YoutubeOpenGL", NULL, NULL);
    // Error check if the window fails to create
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Introduce the window into the current context
    glfwMakeContextCurrent(window);


    gladLoadGL();

    glViewport(0, 0, WIDTH, HEIGHT);
    glOrtho(0, WIDTH, 0, HEIGHT, 0,1);

    GLfloat lineVert[]={
        100.0f,100.0f,0.0f,
        200.f,200.f,0.0,
        300.f,100.f,0.0,
        400.f,200.f,0.0
    };




    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        // Specify the color of the background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // Clean the back buffer and assign the new color to it
        glClear(GL_COLOR_BUFFER_BIT);


        glEnable(GL_LINE_SMOOTH);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3,GL_FLOAT,0,lineVert);
        glDrawArrays(GL_LINES,0,4);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_LINE_SMOOTH);

        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);

        // Take care of all GLFW events
        glfwPollEvents();
    }



    // Delete window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
    return 0;
}