#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "iostream"
#include "PerlinNoise.hpp"
#include "thread"
#include "chrono"
#include "sstream"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"


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


    window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if(glewInit()!=GLEW_OK) std::cout << "Error" << std::endl;


    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6]={
        -0.5f,-0.5f,
        0.0f,0.5f,
        0.5f,-0.5f
    };

    unsigned int buffer;
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glBufferData(GL_ARRAY_BUFFER,6*sizeof(float),positions,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,0);

    glViewport(0, 0, WIDTH, HEIGHT);
    glOrtho(0, WIDTH, HEIGHT, 0, 0,1);

    siv::PerlinNoise noise;
    float inc=0.03;
    float start=0.0;

    Vec2f prev;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

        showFPS(window);
        float xoff=start;
        bool isFirst=true;
        for (int x = 0; x < 50; x++){
            double ny=noise.octave1D_01(xoff,4);
            if(isFirst){
                putpixel(x,ny*HEIGHT);
                isFirst=false;
            }else{
                line(prev.x,prev.y,x*12,ny*HEIGHT);
                glColor3f(0.0,0.756,0.6274);
                line(x*12,ny*HEIGHT,x*12,ny*HEIGHT+10);
                glColor3f(1.0,1.0,1.0);
            }
            prev.x=x*12;
            prev.y=ny*HEIGHT;
            xoff+=inc;
        }
        start+=inc;
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}