/*! \brief Shows the basic usage of OGLplus with GLUT/GLFW and GLEW.
 * \todo Can perhaps be made even safer using <type_traits> to enable only for specific element types such as GLfloat.
 * \todo In what order should we place Display, glfwSwapBuffers and glfwPollEvents
 */

#include <vector>
// OpenGL Stuff
// #define USE_GLUT
#include <GL/glew.h>
#ifdef USE_GLUT
#  include <GL/glut.h>
#else
#  define GLFW_INCLUDE_GLU
#  include <GLFW/glfw3.h>
#endif
#include <oglplus/all.hpp>
#include "enforce.hpp"
#include "rand.hpp"
#include "geometry/vec.hpp"
#include "show_all.hpp"
#include "glutils.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

namespace oglplus {

template<class Form>
class Shape {
public:
    Shape();
private:
    std::vector<Form> m_forms;
    std::vector<float> m_colors; ///< Shape Colors.
};
}

class Example
{
private:
    oglplus::Context gl;
    oglplus::Program prog;
    oglplus::VertexShader vs;
    oglplus::FragmentShader fs;

    oglplus::VertexArray triangle; // Wraps VAO
    oglplus::Buffer      verts; // Wraps VBO
    oglplus::Buffer      colors; // Wraps VBO

    size_t    nvert  = 600;      ///< number of vertices
    const int ndims  = 3;       ///< number of dimensions
    const int ncomps = 3;       ///< number of color components

    std::vector<float> triV;  ///< Vertices
    std::vector<float> triC;  ///< Colors

public:
    Example() {
        using namespace oglplus;

        triV.resize(nvert*ndims);
        triC.resize(nvert*ncomps);

        vs.Source(R"*GLSL*(
            #version 330
            in vec3 Position;
            in vec3 Color;
            out vec4 vertColor;
            void main(void)
            {
                gl_Position = vec4(Position, 1.0);
                vertColor = vec4(Color, 1.0);
            }
        )*GLSL*").Compile();           // Vertex Shader

        fs.Source(R"*GLSL*(
            #version 330
            in vec4 vertColor;
            out vec4 fragColor;
            void main(void)
            {
                fragColor = vertColor;
            }
        )*GLSL*").Compile();           // Fragement Shader

        (prog << vs << fs).Link().Use(); // prog.AttachShader(vs).AttachShader(fs).Link().Use();

        triangle.Bind();        // VAO: Triangle Object

        gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gl.ClearDepth(1.0f);
    }

    void set_vertices() {
        using namespace oglplus;
        rrand(triV, -1, 1);
        verts.Bind(Buffer::Target::Array); // VBO: Triangle Vertices
        Buffer::Data(Buffer::Target::Array, triV); // upload data
        VertexAttribArray attrV(prog, "Position"); // uploaded data is the "Position" variable in Vertex Shader
        attrV.Setup(ndims, DataType::Float).Enable(); // 3D-vector
    }

    void set_colors() {
        using namespace oglplus;
        rrand(triC, 0.0f, +1.0f); // randomize vertices
        colors.Bind(Buffer::Target::Array);        // VBO: Triangle Colors
        Buffer::Data(Buffer::Target::Array, triC); // upload data
        VertexAttribArray attrC(prog, "Color");        // uploaded data is the "Color" variable in Vertex Shader
        attrC.Setup(ncomps, DataType::Float).Enable(); // RGB-vector
    }

    void Display()
    {
        using namespace oglplus;
        set_vertices();
        set_colors();
        gl.Clear().ColorBuffer().DepthBuffer();

        gl.DrawArrays(PrimitiveType::Triangles, 0, nvert);
    }
};

class SingleExample
{
private:
    static Example*& SingleInstance() {
        static Example* test = nullptr;
        return test;
    }
    SingleExample(const SingleExample&); // disable copy constructor
public:
    SingleExample() {
        enforce(not SingleInstance());
        SingleInstance() = new Example();
    }
    ~SingleExample() {
        enforce(SingleInstance());
        delete SingleInstance();
        SingleInstance() = nullptr;
    }
    static void Display() {
        enforce(SingleInstance());
        SingleInstance()->Display();
#ifdef USE_GLUT
        glutSwapBuffers();
        glutPostRedisplay();
#endif
    }
};

#ifdef USE_GLUT
void glut_setup(int argc, char* argv[], int w = 640, int h = 480, const char* title = nullptr)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutInitWindowPosition(100,100);
    if (not title) {
        title = "OGLplus+GLFW+GLEW Demo"; }
    glutCreateWindow(title);
}
#else
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
GLFWwindow* glfw_setup(int w = 640, int h = 480, const char* title = nullptr)
{
    glfwSetErrorCallback(error_callback);
    if (not glfwInit()) {
        exit(EXIT_FAILURE); }
    if (not title) {
        title = "OGLplus+GLFW+GLEW Demo"; }
    GLFWwindow* win = glfwCreateWindow(w, h, title, nullptr, nullptr); // Open a win and create its OpenGL context
    if (not win) { glfwTerminate(); exit(EXIT_FAILURE); }
    glfwMakeContextCurrent(win); // enable vertical sync (on cards that support it)
    glfwSwapInterval(1);         // smooth animation 60 fps. \see http://www.opengl.org/wiki/Swap_Interval
    glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key being pressed below
    return win;
}
#endif

/*! Enable OpenGL Fullscreen */
void glEnableFullScreen() {
}

/*! Disable OpenGL Fullscreen */
void glDisableFullScreen() {
}


int main(int argc, char* argv[])
{
#ifdef USE_GLUT
    glut_setup(argc, argv);
#else
    auto win = glfw_setup();     // GLFW
#endif
    glEnableFullScreen();

    if (glewInit() == GLEW_OK) { // GLEW Initialized OK
        try {
            SingleExample example;
#ifdef USE_GLUT
            glutDisplayFunc(&SingleExample::Display);
            glutMainLoop();
#else
            // GLFW uses explicit loop
            while (true) {
                double x, y; glfwGetCursorPos(win, &x, &y);
                int w, h; glfwGetWindowSize(win, &w, &h);
                glViewport(0, 0, w, h > 0 ? h : 1);

                SingleExample::Display();

                glfwSwapBuffers(win);
                glfwPollEvents();
                if (glfwGetKey(win, GLFW_KEY_ESCAPE)) break;
                if (glfwWindowShouldClose(win)) break;
            }
#endif
        } catch(oglplus::Error& err) {
            std::cerr <<
                "Error (in " << err.GLSymbol() << ", " <<
                err.ClassName() << ": '" <<
                err.ObjectDescription() << "'): " <<
                err.what() <<
                " [" << err.File() << ":" << err.Line() << "] ";
            std::cerr << std::endl;
            err.Cleanup();
        }
}

#ifndef USE_GLUT
    glfwTerminate();            // Close OpenGL window and terminate GLFW
#endif
    exit(EXIT_SUCCESS);
}
