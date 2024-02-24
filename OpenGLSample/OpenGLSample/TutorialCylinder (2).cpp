#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "camera.h"

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const int SCR_WIDTH = 800;
    const int SCR_HEIGHT = 600;
    // camera
    Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    const char* const WINDOW_TITLE = "Tutorial 3.4"; // Macro for window title

    // offset to use to encompass vertex position and color; change this value if using 
    // textures, etc.
    const int STRIDE = 7;


    // Variables for window width and height
    

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMeshCube;
    GLMesh gMeshCylinder;
    // Shader program
    GLuint gProgramId;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateCylinder(GLfloat verts[], GLushort indices[], int numSides, float radius, float halfLen);
void UCreateCube(GLfloat verts[], GLushort indices[], int numSides, float radius, float halfLen);
void UCreateMeshCylinder(GLMesh& mesh);
void UCreateMeshCube(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1

out vec4 vertexColor; // variable to transfer color data to the fragment shader

//Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexColor = color; // references incoming color data
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader

out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(vertexColor);
}
);


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMeshCube(gMeshCube); // Calls the function to create the Vertex Buffer Object
    UCreateMeshCylinder(gMeshCylinder);

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMeshCube);
    UDestroyMesh(gMeshCylinder);

    // Release shader program
    UDestroyShaderProgram(gProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// Functioned called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
    // 2. Rotates shape by 15 degrees in the x axis
    glm::mat4 rotation = glm::rotate(45.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Transforms the camera: move the camera back (z axis)
    glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -5.0f));

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMeshCube.vao);
    glBindVertexArray(gMeshCylinder.vao);

    // Draws the triangles
    glDrawElements(GL_TRIANGLES, gMeshCube.nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glDrawElements(GL_TRIANGLES, gMeshCylinder.nIndices, GL_UNSIGNED_SHORT, NULL);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

// creates a prism based on the number of side turned in; assumes a stride of seven
void UCreateCylinder(GLfloat verts[], GLushort indices[], int numSides, float radius, float halfLen)
{
    // create constant for 2PI used in calculations
    const float TWO_PI = 2.0f * 3.1415926f;
    const float radiansPerSide = TWO_PI / numSides;

    // value to increment after each vertex is created
    int currentVertex = 0;

    // in this  algorithm, vertex zero is the top center vertex, and vertex one is the bottom center
    // each is offset by the step size
    verts[0] = 0.0f;    // 0 x
    verts[1] = halfLen; // 0 y
    verts[2] = 0.0f;    // 0 z
    verts[3] = 1.0f;    // 0 r
    verts[4] = 0.0f;    // 0 g
    verts[5] = 0.0f;    // 0 b
    verts[6] = 1.0f;    // 0 a
    currentVertex++;

    verts[7] = 0.0f;    // 1 x
    verts[8] = -halfLen;// 1 y
    verts[9] = 0.0f;    // 1 z
    verts[10] = 1.0f;   // 1 r
    verts[11] = 0.0f;   // 1 g
    verts[12] = 0.0f;   // 1 b
    verts[13] = 1.0f;   // 1 a
    currentVertex++;

    // variable to keep track of every triangle added to indices
    int currentTriangle = 0;

    // note: the number of flat sides is equal to the number of edge on the sides
    for (int edge = 0; edge < numSides; edge++)
    {
        // calculate theta, which is the angle from the center point to the next vertex
        float theta = ((float)edge) * radiansPerSide;

        // top triangle first perimeter vertex
        verts[currentVertex * STRIDE + 0] = radius * cos(theta);    // x
        verts[currentVertex * STRIDE + 1] = halfLen;                // y
        verts[currentVertex * STRIDE + 2] = radius * sin(theta);    // z
        verts[currentVertex * STRIDE + 3] = 0.0f;                   // r
        verts[currentVertex * STRIDE + 4] = 1.0f;                   // g
        verts[currentVertex * STRIDE + 5] = 0.0f;                   // b
        verts[currentVertex * STRIDE + 6] = 1.0f;                   // a
        currentVertex++;

        // bottom triangle first perimeter vertex
        verts[currentVertex * STRIDE + 0] = radius * cos(theta);    // x
        verts[currentVertex * STRIDE + 1] = -halfLen;               // y
        verts[currentVertex * STRIDE + 2] = radius * sin(theta);    // z
        verts[currentVertex * STRIDE + 3] = 0.0f;                   // r
        verts[currentVertex * STRIDE + 4] = 0.0f;                   // g
        verts[currentVertex * STRIDE + 5] = 1.0f;                   // b
        verts[currentVertex * STRIDE + 6] = 1.0f;                   // a
        currentVertex++;

        if (edge > 0) 
        {
            // now to create the indices for the triangles
            // top triangle
            indices[(3 * currentTriangle) + 0] = 0;                 // center of top of prism
            indices[(3 * currentTriangle) + 1] = currentVertex - 4; // upper left vertex of side
            indices[(3 * currentTriangle) + 2] = currentVertex - 2; // upper right vertex of side
            currentTriangle++;

            // bottom triangle
            indices[(3 * currentTriangle) + 0] = 1;                 // center of bottom of prism
            indices[(3 * currentTriangle) + 1] = currentVertex - 3; // bottom left vertex of side
            indices[(3 * currentTriangle) + 2] = currentVertex - 1; // bottom right vertex of side
            currentTriangle++;

            // triangle for 1/2 retangular side
            indices[(3 * currentTriangle) + 0] = currentVertex - 4; // upper left vertex of side
            indices[(3 * currentTriangle) + 1] = currentVertex - 3; // bottom left vertex of side
            indices[(3 * currentTriangle) + 2] = currentVertex - 1; // bottom right vertex of side
            currentTriangle++;

            // triangle for second 1/2 retangular side
            indices[(3 * currentTriangle) + 0] = currentVertex - 1; // bottom right vertex of side
            indices[(3 * currentTriangle) + 1] = currentVertex - 2; // upper right vertex of side
            indices[(3 * currentTriangle) + 2] = currentVertex - 4; // upper left vertex of side
            currentTriangle++;

        }

    }

    // now, just need to wire up the last side
    // now to create the indices for the triangles
    // top triangle
    indices[(3 * currentTriangle) + 0] = 0;                 // center of top of prism
    indices[(3 * currentTriangle) + 1] = currentVertex - 2; // upper left vertex of side
    indices[(3 * currentTriangle) + 2] = 2;                 // first upper left vertex created, now right
    currentTriangle++;

    // bottom triangle
    indices[(3 * currentTriangle) + 0] = 1;                 // center of bottom of prism
    indices[(3 * currentTriangle) + 1] = currentVertex - 1; // bottom left vertex of side
    indices[(3 * currentTriangle) + 2] = 3;                 // first bottom left vertex created, now right
    currentTriangle++;

    // triangle for 1/2 retangular side
    indices[(3 * currentTriangle) + 0] = currentVertex - 2; // upper left vertex of side
    indices[(3 * currentTriangle) + 1] = currentVertex - 1; // bottom left vertex of side
    indices[(3 * currentTriangle) + 2] = 3;                 // bottom right vertex of side
    currentTriangle++;

    // triangle for second 1/2 retangular side
    indices[(3 * currentTriangle) + 0] = 3;                 // bottom right vertex of side
    indices[(3 * currentTriangle) + 1] = 2;                 // upper right vertex of side
    indices[(3 * currentTriangle) + 2] = currentVertex - 2; // upper left vertex of side
    currentTriangle++; 

}

void UCreateCube(GLfloat verts[], GLushort indices[], int numSides, float radius, float halfLen)
{
    // create constant for 2PI used in calculations
    const float TWO_PI = 2.0f * 3.1415926f;
    const float radiansPerSide = TWO_PI / numSides;

    // value to increment after each vertex is created
    int currentVertex = 0;

    // in this  algorithm, vertex zero is the top center vertex, and vertex one is the bottom center
    // each is offset by the step size
    verts[0] = 0.0f;    // 0 x
    verts[1] = halfLen; // 0 y
    verts[2] = 0.0f;    // 0 z
    verts[3] = 1.0f;    // 0 r
    verts[4] = 0.0f;    // 0 g
    verts[5] = 0.0f;    // 0 b
    verts[6] = 1.0f;    // 0 a
    currentVertex++;

    verts[7] = 0.0f;    // 1 x
    verts[8] = -halfLen;// 1 y
    verts[9] = 0.0f;    // 1 z
    verts[10] = 1.0f;   // 1 r
    verts[11] = 0.0f;   // 1 g
    verts[12] = 0.0f;   // 1 b
    verts[13] = 1.0f;   // 1 a
    currentVertex++;

    // variable to keep track of every triangle added to indices
    int currentTriangle = 0;

    // note: the number of flat sides is equal to the number of edge on the sides
    for (int edge = 0; edge < numSides; edge++)
    {
        // calculate theta, which is the angle from the center point to the next vertex
        float theta = ((float)edge) * radiansPerSide;

        // top triangle first perimeter vertex
        verts[currentVertex * STRIDE + 0] = radius * cos(theta);    // x
        verts[currentVertex * STRIDE + 1] = halfLen;                // y
        verts[currentVertex * STRIDE + 2] = radius * sin(theta);    // z
        verts[currentVertex * STRIDE + 3] = 0.0f;                   // r
        verts[currentVertex * STRIDE + 4] = 1.0f;                   // g
        verts[currentVertex * STRIDE + 5] = 0.0f;                   // b
        verts[currentVertex * STRIDE + 6] = 1.0f;                   // a
        currentVertex++;

        // bottom triangle first perimeter vertex
        verts[currentVertex * STRIDE + 0] = radius * cos(theta);    // x
        verts[currentVertex * STRIDE + 1] = -halfLen;               // y
        verts[currentVertex * STRIDE + 2] = radius * sin(theta);    // z
        verts[currentVertex * STRIDE + 3] = 0.0f;                   // r
        verts[currentVertex * STRIDE + 4] = 0.0f;                   // g
        verts[currentVertex * STRIDE + 5] = 1.0f;                   // b
        verts[currentVertex * STRIDE + 6] = 1.0f;                   // a
        currentVertex++;

        if (edge > 0)
        {
            // now to create the indices for the triangles
            // top triangle
            indices[(3 * currentTriangle) + 0] = 0;                 // center of top of prism
            indices[(3 * currentTriangle) + 1] = currentVertex - 4; // upper left vertex of side
            indices[(3 * currentTriangle) + 2] = currentVertex - 2; // upper right vertex of side
            currentTriangle++;

            // bottom triangle
            indices[(3 * currentTriangle) + 0] = 1;                 // center of bottom of prism
            indices[(3 * currentTriangle) + 1] = currentVertex - 3; // bottom left vertex of side
            indices[(3 * currentTriangle) + 2] = currentVertex - 1; // bottom right vertex of side
            currentTriangle++;

            // triangle for 1/2 retangular side
            indices[(3 * currentTriangle) + 0] = currentVertex - 4; // upper left vertex of side
            indices[(3 * currentTriangle) + 1] = currentVertex - 3; // bottom left vertex of side
            indices[(3 * currentTriangle) + 2] = currentVertex - 1; // bottom right vertex of side
            currentTriangle++;

            // triangle for second 1/2 retangular side
            indices[(3 * currentTriangle) + 0] = currentVertex - 1; // bottom right vertex of side
            indices[(3 * currentTriangle) + 1] = currentVertex - 2; // upper right vertex of side
            indices[(3 * currentTriangle) + 2] = currentVertex - 4; // upper left vertex of side
            currentTriangle++;

        }

    }

    // now, just need to wire up the last side
    // now to create the indices for the triangles
    // top triangle
    indices[(3 * currentTriangle) + 0] = 0;                 // center of top of prism
    indices[(3 * currentTriangle) + 1] = currentVertex - 2; // upper left vertex of side
    indices[(3 * currentTriangle) + 2] = 2;                 // first upper left vertex created, now right
    currentTriangle++;

    // bottom triangle
    indices[(3 * currentTriangle) + 0] = 1;                 // center of bottom of prism
    indices[(3 * currentTriangle) + 1] = currentVertex - 1; // bottom left vertex of side
    indices[(3 * currentTriangle) + 2] = 3;                 // first bottom left vertex created, now right
    currentTriangle++;

    // triangle for 1/2 retangular side
    indices[(3 * currentTriangle) + 0] = currentVertex - 2; // upper left vertex of side
    indices[(3 * currentTriangle) + 1] = currentVertex - 1; // bottom left vertex of side
    indices[(3 * currentTriangle) + 2] = 3;                 // bottom right vertex of side
    currentTriangle++;

    // triangle for second 1/2 retangular side
    indices[(3 * currentTriangle) + 0] = 3;                 // bottom right vertex of side
    indices[(3 * currentTriangle) + 1] = 2;                 // upper right vertex of side
    indices[(3 * currentTriangle) + 2] = currentVertex - 2; // upper left vertex of side
    currentTriangle++;

}

// Implements the UCreateMesh function
void UCreateMeshCylinder(GLMesh& mesh)
{
    // number of sides for the prism we will create
    const int NUM_SIDES = 100;

    // the number of vertices is the number of sides * 2 (think, two vertices per edge line), plus 
    // 2 for the center points at the top and bottom; since each vertex has a stride of 7 (x,y,z,r,g,b,a) would
    // also need to multiply by seven
    const int NUM_VERTICES = STRIDE * (2 + (2 * NUM_SIDES));

    // the number of indices of a prism is 3 * the number of triangle that will be drawn; therefore, for a cube prism
    // with a center point on each end (i.e. the smaller ends will be made up of four triangles instead of two) there 
    // would be (4 triangles * 2 ends) + (2 triangles * 4 sides) = 16 triangles. Therefore, a total of 3 * 16, or 48 vertices
    // for a cube prism. Or, more generally, 4 triangles needed for every side (top slice of pie, bottom, and two for the rectangle 
    // on the side). Or, 12 * num sides is the amount of indices needed.
    const int NUM_INDICES = 12 * NUM_SIDES;

    // Position and Color data
    GLfloat verts[NUM_VERTICES];

    // Index data to share position data
    GLushort indices[NUM_INDICES];

    // fill the verts and indices arrays with data
    UCreateCylinder(verts, indices, NUM_SIDES, 0.25f, 1.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    if (STRIDE != floatsPerVertex + floatsPerColor)
    {
        exit(EXIT_FAILURE);
    }

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}

void UCreateMeshCube(GLMesh& mesh)
{
    // number of sides for the prism we will create
    const int NUM_SIDES = 4;

    // the number of vertices is the number of sides * 2 (think, two vertices per edge line), plus 
    // 2 for the center points at the top and bottom; since each vertex has a stride of 7 (x,y,z,r,g,b,a) would
    // also need to multiply by seven
    const int NUM_VERTICES = STRIDE * (2 + (2 * NUM_SIDES));

    // the number of indices of a prism is 3 * the number of triangle that will be drawn; therefore, for a cube prism
    // with a center point on each end (i.e. the smaller ends will be made up of four triangles instead of two) there 
    // would be (4 triangles * 2 ends) + (2 triangles * 4 sides) = 16 triangles. Therefore, a total of 3 * 16, or 48 vertices
    // for a cube prism. Or, more generally, 4 triangles needed for every side (top slice of pie, bottom, and two for the rectangle 
    // on the side). Or, 12 * num sides is the amount of indices needed.
    const int NUM_INDICES = 12 * NUM_SIDES;

    // Position and Color data
    GLfloat verts[NUM_VERTICES];

    // Index data to share position data
    GLushort indices[NUM_INDICES];

    // fill the verts and indices arrays with data
    UCreateCylinder(verts, indices, NUM_SIDES, 0.25f, 1.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    if (STRIDE != floatsPerVertex + floatsPerColor)
    {
        exit(EXIT_FAILURE);
    }

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

