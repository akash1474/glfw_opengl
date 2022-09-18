#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "iostream"
#include "PerlinNoise.hpp"
#include "thread"
#include "chrono"
#include "sstream"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"


#define WIDTH 480
#define HEIGHT 480

struct Vec2f {
    float x;
    float y;
    Vec2f() {};
    Vec2f(float a, float b): x(a), y(b) {}
};

void line(const Vec2f& p1, const Vec2f& p2) {
    glBegin(GL_LINES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
}

void line(int x1, int y1, int x2, int y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void putpixel(float x1, float y1) {
    glBegin(GL_POINTS);
    glVertex2f(x1, y1);
    glEnd();
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
    if (glewInit() != GLEW_OK) std::cout << "Error" << std::endl;


    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] =
    { //     COORDINATES     /        COLORS      /   TexCoord  //
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,    0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,    5.0f, 0.0f,
         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,    0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,    5.0f, 0.0f,
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,    2.5f, 5.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);


    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(6 * sizeof(float)));

    glViewport(0, 0, WIDTH, HEIGHT);
    glOrtho(0, WIDTH, HEIGHT, 0, 1, -1);

    Shader* sh = Shader::FromGLSLTextFiles("./shaders/vertex.glsl", "./shaders/fragment.glsl");
    glUseProgram(sh->GetRendererID());

    unsigned int uId = glGetUniformLocation(sh->GetRendererID(), "scale");
    glUniform1f(uId, 1.0f);

    Texture popCat("./assets/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    popCat.texUnit(sh, "tex0", 0);
    popCat.Bind();


    // Creates camera object
    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handles camera inputs
        camera.Inputs(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.Matrix(45.0f, 0.1f, 100.0f, sh, "camMatrix");

        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);


        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glDeleteVertexArrays();
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    popCat.Delete();


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}