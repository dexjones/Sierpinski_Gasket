#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include "Application.h"

typedef struct {
    float x, y;
} point2;

// Function to draw a single triangle
void triangle(point2 a, point2 b, point2 c, GLuint VAO, GLuint VBO) {
    float vertices[] = {
        a.x, a.y,
        b.x, b.y,
        c.x, c.y
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

// Recursive function to divide a triangle and create the Sierpinski Gasket
void divide_triangle(point2 a, point2 b, point2 c, int k, GLuint VAO, GLuint VBO) {
    point2 ab, ac, bc;
    if (k > 0) {
        // Compute the midpoints of the sides
        ab.x = (a.x + b.x) / 2;
        ab.y = (a.y + b.y) / 2;
        ac.x = (a.x + c.x) / 2;
        ac.y = (a.y + c.y) / 2;
        bc.x = (b.x + c.x) / 2;
        bc.y = (b.y + c.y) / 2;
        // Subdivide all but the inner triangle
        divide_triangle(a, ab, ac, k - 1, VAO, VBO);
        divide_triangle(c, ac, bc, k - 1, VAO, VBO);
        divide_triangle(b, bc, ab, k - 1, VAO, VBO);
    }
    else {
        triangle(a, b, c, VAO, VBO);
    }
}
// Display function to draw the Sierpinski Gasket
void display(int n, GLuint VAO, GLuint VBO, GLuint shaderProgram) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    point2 v[] = { { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 0.0f, 1.0f } };
    divide_triangle(v[0], v[1], v[2], n, VAO, VBO);
    glFlush();
}

GLuint compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}
GLuint createShader(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "layout(location = 0) out vec4 color;\n"
        "void main() {\n"
        "    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shaderProgram = createShader(vertexShader, fragmentShader);

    // Set up vertex data and configure vertex attributes
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* Set up the display function */
    int n = 5; // Number of subdivision steps, you can change this
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        display(n, VAO, VBO, shaderProgram);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
