#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "iostream"
#include "PerlinNoise.hpp"
#include "thread"
#include "chrono"
#include "sstream"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Shader.h"


#define WIDTH 600
#define HEIGHT 480

struct Vec2f{
    float x;
    float y;
    Vec2f(){};
    Vec2f(float a,float b):x(a),y(b){}
};

void line(const Vec2f& p1,const Vec2f& p2){
    glBegin(GL_LINES);
    glVertex2f(p1.x,p1.y);
    glVertex2f(p2.x,p2.y);
    glEnd();
}

void line(int x1,int y1,int x2,int y2){
    glBegin(GL_LINES);
    glVertex2f(x1,y1);
    glVertex2f(x2,y2);
    glEnd();
}

void putpixel(float x1,float y1){
    glBegin(GL_POINTS);
    glVertex2f(x1,y1);
    glEnd();
}

double lastTime;
int nbFrames=0;

void showFPS(GLFWwindow *pWindow)
{
    // Measure speed
     double currentTime = glfwGetTime();
     double delta = currentTime - lastTime;
     nbFrames++;
     if ( delta >= 1.0 ){
         double fps = double(nbFrames) / delta;
         std::stringstream ss;
         ss <<" [" << fps << " FPS]";
         glfwSetWindowTitle(pWindow, ss.str().c_str());
         nbFrames = 0;
         lastTime = currentTime;
     }
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if(glewInit()!=GLEW_OK) std::cout << "Error" << std::endl;


    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[]=
    { //               COORDINATES                  /     COLORS           //
        -0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
         0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
         0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
        -0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
         0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
         0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
    };

    unsigned int indices[]={
        0,3,5,
        3,2,4,
        5,4,1
    };

    unsigned int VBO,EBO;
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(positions),positions,GL_STATIC_DRAW);


    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*6,(const void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(float)*6,(const void*)(3*sizeof(float)));

    glViewport(0, 0, WIDTH, HEIGHT);
    glOrtho(0, WIDTH, HEIGHT, 0, 0,1);

    Shader* sh=Shader::FromGLSLTextFiles("./shaders/vertex.shader", "./shaders/fragment.shader");
    glUseProgram(sh->GetRendererID());

    unsigned int uId=glGetUniformLocation(sh->GetRendererID(),"scale");
    glUniform1f(uId,0.5f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glDeleteVertexArrays();
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);


    glfwTerminate();
    return 0;
}