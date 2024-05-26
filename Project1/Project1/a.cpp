#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "pixeldye.h"

#include <iostream>
#include "../Vector4.h"
#include "../Matrix.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Vector4 vertices[] = {
    Vector4(-0.5f, -0.5f, 0.0f, 1.0f),
    Vector4(0.5f, -0.2f, 0.0f, 1.0f),
    Vector4(0.0f,  0.5f, 0.0f, 1.0f)
    };

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    unsigned char* data = new unsigned char[3 * SCR_WIDTH * SCR_HEIGHT * sizeof(unsigned char)];
    for (unsigned int i = 0; i < SCR_WIDTH * SCR_HEIGHT; i++)
    {
        data[i * 3] = (unsigned char)(0.0f);
        data[i * 3 + 1] = (unsigned char)(255.0f);
        data[i * 3 + 2] = (unsigned char)(0.0f);
    }


    // render loop
    // -----------

    Vector4 cameraPos (0.0f, 0.0f, 3.0f, 1.0f);
    Vector4 cameraTarget(0.0f, 0.0f, 0.0f, .0f);
    Vector4 up(0.0f, 1.0f, 0.0f, 0.0f);

    Matrix model = Matrix::Scale(1.0f, 1.0f, 1.0f);
    Matrix view = Matrix::View(cameraPos, cameraTarget, up);
    Matrix proj = Matrix::Persp(45.0f, 0.1f, 100.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT);

    Vector4 verticesPersp[] = {
         proj* (view* (model * vertices[0])),
         proj* (view* (model * vertices[1])),
         proj* (view* (model * vertices[2]))
    };


    Vector4 verticesClip[] = {
     verticesPersp[0]/ verticesPersp[0].w,
     verticesPersp[1]/ verticesPersp[1].w,
     verticesPersp[2]/ verticesPersp[2].w
    };


    float a = (SCR_WIDTH) / 2.0;
    float b = (SCR_HEIGHT) / 2.0;
    Matrix scale = Matrix::Scale(a, b, 1.0f);
    Matrix trans = Matrix::Translate(a, b, 0.0f);

    Vector4 verticesResult[] = {
         trans* (scale* verticesClip[0]),
         trans* (scale* verticesClip[1]),
         trans* (scale* verticesClip[2])
    };


    scan(verticesResult,  3,  data);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLuint fboId = 0;
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, texture1, 0);

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // if not already bound
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}