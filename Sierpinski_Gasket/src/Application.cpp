#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include "Application.h"

typedef struct {
    float x, y;
} point2;

// Function to draw a single triangle
void triangle(point2 a, point2 b, point2 c) {
    glBegin(GL_TRIANGLES);
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    glVertex2f(c.x, c.y);
    glEnd();
}

// Recursive function to divide a triangle and create the Sierpinski Gasket
void divide_triangle(point2 a, point2 b, point2 c, int k) {
    point2 ab, ac, bc;
    int j;
    if (k > 0) {
        // Compute the midpoints of the sides
        ab.x = (a.x + b.x) / 2;
        ab.y = (a.y + b.y) / 2;
        ac.x = (a.x + c.x) / 2;
        ac.y = (a.y + c.y) / 2;
        bc.x = (b.x + c.x) / 2;
        bc.y = (b.y + c.y) / 2;
        // Subdivide all but the inner triangle
        divide_triangle(a, ab, ac, k - 1);
        divide_triangle(c, ac, bc, k - 1);
        divide_triangle(b, bc, ab, k - 1);
    }
    else {
        triangle(a, b, c);
    }
}

// Display function to draw the Sierpinski Gasket
void display(int n) {
    glClear(GL_COLOR_BUFFER_BIT);
    point2 v[] = { { -1.0, -1.0 }, { 1.0, -1.0 }, { 0.0, 1.0 } };
    divide_triangle(v[0], v[1], v[2], n);
    glFlush();
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(GL_VERTEX_SHADER);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // TODO: Error Handling

    return id;
}
static int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);

}
int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Sierpinski Gasket", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Set up the display function */
    int n = 5; // Number of subdivision steps, you can change this
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        display(n);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
