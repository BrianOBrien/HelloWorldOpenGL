#include <GLFW/glfw3.h>
#include "TheSim.hpp"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    TheSim s;

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    s.beginUse();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //glBegin(GL_TRIANGLES);
        //glVertex2f(-0.5f, -0.5f);
        //glVertex2f( 0.0f,  0.5f);
        //glVertex2f(0.5f, -0.5f);
        //glEnd();
        s.drawScene();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    s.endUse();
    glfwTerminate();
    return 0;
}