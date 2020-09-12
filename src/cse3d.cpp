#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#define VERTEX_SHADER "res/shaders/vtx.glsl"
#define FRAGMENT_SHADER "res/shaders/frag.glsl"
#define scale 100.0f
#define sensitivity 100.0f
unsigned int shader;
static std::string readFile(const std::string& path) {
    std::ifstream stream;
    stream.open(path);
    std::string line;
    std::string ret;
    while (stream) {
        std::getline(stream, line);
        ret += line;
        ret += "\n";
        if (stream.eof()) {
            break;
        }
    }
    stream.close();
    return ret;
}
static int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader: " << std::endl << message << std::endl;
        glDeleteShader(id);
        return -1;
    }
    return id;
}
static int constructShader(const std::string& vertex, const std::string& fragment) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertex);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragment);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}
void drawLine(GLFWwindow* window, float x1, float y1, float x2, float y2) {
    float positions[] = { x1, y1, x2, y2 };
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glUseProgram(shader);
    glDrawArrays(GL_LINES, 0, 3);
}
void drawCross(GLFWwindow* window, float x, float y, float length) {
    drawLine(window, x - length, y - length, x + length, y + length);
    drawLine(window, x + length, y - length, x - length, y + length);
}
void drawCube(GLFWwindow* window) {
    float ptX[] = {-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f};
    float ptY[] = {1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.0f};
    float ptZ[] = {1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f};
    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    float xTransform;
    float yTransform;
    float viewAngleX;
    float viewAngleY;
    for (int i = 0; i < sizeof(ptX) / sizeof(float); i++) {
        float zAngle = atan(ptZ[i] / ptX[i]);
        if (ptX[i] == 0.0f && ptZ[i] == 0.0f) {
            zAngle = 0.0f;
        }
        float mag = sqrt(pow(ptX[i], 2) + pow(ptZ[i], 2));
        viewAngleX = (mouseX - width / 2.0f) / sensitivity;
        viewAngleY = (mouseY - height / 2.0f) / sensitivity;
        if (ptX[i] < 0.0f) {
            xTransform = -mag * scale * cos(viewAngleX - zAngle);
            yTransform = -mag * scale * sin(viewAngleX - zAngle) * sin(viewAngleY) + ptY[i] * scale * cos(viewAngleY);
        }
        else {
            xTransform = mag * scale * cos(viewAngleX + zAngle);
            yTransform = mag * scale * sin(viewAngleX + zAngle) * sin(viewAngleY) + ptY[i] * scale * cos(viewAngleY);
        }

        float x = static_cast<float>(width) / 2.0f + xTransform;
        float y = static_cast<float>(height) / 2.0f - yTransform;
        x /= static_cast<float>(width);
        y /= static_cast<float>(height);
        x *= 2.0f;
        y *= 2.0f;
        x -= 1.0f;
        y -= 1.0f;
        drawCross(window, x, y, 0.05f);
    }
}
int main() {
    if (!glfwInit()) {
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(640, 480, "test window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();
    shader = constructShader(readFile(VERTEX_SHADER), readFile(FRAGMENT_SHADER));
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        drawCube(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    glDeleteProgram(shader);
    return 0;
}