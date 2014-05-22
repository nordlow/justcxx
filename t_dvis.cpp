/*! \file t_dvis.cpp
 * \brief Test of the (DVIS) Engine.
 */

//#include <dvis/dvis.hpp>
#include <GL/glew.h>
#include <oglplus/all.hpp>
#include <wx/wx.h>
#include <wx/glcanvas.h>

class Example
{
private:
    oglplus::Context gl;

    // Vertex shader
    oglplus::VertexShader vs;

    // Fragment shader
    oglplus::FragmentShader fs;

    // Program
    oglplus::Program prog;

    // A vertex array object for the rendered triangle
    oglplus::VertexArray triangle;
    // VBO for the triangle's vertices
    oglplus::Buffer verts;
public:
    Example(void)
    {
        using namespace oglplus;

        // Set the vertex shader source
        vs.Source(R"**(
            #version 330
            in vec3 Position;
            void main(void)
            {
                gl_Position = vec4(Position, 1.0);
            }
        )**");
        // compile it
        vs.Compile();

        // set the fragment shader source
        fs.Source(R"**(
            #version 330
            out vec4 fragColor;
            void main(void)
            {
                fragColor = vec4(1.0, 0.0, 0.0, 1.0);
            }
        )**");
        // compile it
        fs.Compile();

        // attach the shaders to the program
        prog.AttachShader(vs);
        prog.AttachShader(fs);
        // link and use it
        prog.Link();
        prog.Use();

        // bind the VAO for the triangle
        triangle.Bind();

        GLfloat triangle_verts[9] = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };
        // bind the VBO for the triangle vertices
        verts.Bind(Buffer::Target::Array);
        // upload the data
        Buffer::Data(Buffer::Target::Array, 9, triangle_verts);
        // setup the vertex attribs array for the vertices
        VertexAttribArray vert_attr(prog, "Position");
        vert_attr.Setup(3, DataType::Float);
        vert_attr.Enable();

        gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gl.ClearDepth(1.0f);
    }

    void Display(void)
    {
        using namespace oglplus;

        gl.Clear().ColorBuffer().DepthBuffer();

        gl.DrawArrays(PrimitiveType::Triangles, 0, 3);
    }

    void Reshape(size_t w, size_t h)
    {
        gl.Viewport(w, h);
    }
};

class ExampleFrame : public wxFrame
{
private:
    wxPanel* oglPanel;
    wxGLCanvas* oglCanvas;
    wxGLContext oglContext;

    static int* get_gl_config(void)
    {
        static int conf[] = {
            WX_GL_RGBA,
            WX_GL_DOUBLEBUFFER,
            WX_GL_DEPTH_SIZE, 24,
            0
        };
        return conf;
    }

    Example* oglExample;

    void Update(void)
    {
        if(oglExample) oglExample->Display();
        oglCanvas->SwapBuffers();
    }

    void OnIdle(wxIdleEvent& event)
    {
        Update();
        wxYield();
        wxMilliSleep(1);
        if(oglExample) event.RequestMore();
    }

    void OnResize(wxSizeEvent& event)
    {
        if(oglExample) oglExample->Reshape(GetSize().x, GetSize().y);
        event.Skip();
    }
public:
    ExampleFrame(void)
        : wxFrame(
            (wxWindow*)0,
            wxID_ANY,
            wxT("OGLplus + wxGL"),
            wxDefaultPosition,
            wxSize(800, 600)
                  ), oglPanel(
                      new wxPanel(
                          (wxWindow*)this,
                          wxID_ANY,
                          wxDefaultPosition,
                          wxDefaultSize
                                  )
                              ), oglCanvas(
                                  new wxGLCanvas(
                                      (wxWindow*)oglPanel,
                                      wxID_ANY,
                                      get_gl_config(),
                                      wxDefaultPosition,
                                      wxDefaultSize
                                                 )
                                           ), oglContext(oglCanvas)
        , oglExample(0)
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(oglPanel, 1, wxEXPAND);
        SetSizer(mainSizer);
        Show();
        //
        wxBoxSizer* oglSizer = new wxBoxSizer(wxVERTICAL);
        oglSizer->Add(oglCanvas, 1, wxEXPAND);
        oglPanel->SetSizer(oglSizer);
        //
        oglContext.SetCurrent(*oglCanvas);
        //
        if(glewInit() != GLEW_OK) throw std::runtime_error("GLEW init error");
        //
        oglExample = new Example();
        //
        Connect(
            wxEVT_SIZE,
            wxSizeEventHandler(ExampleFrame::OnResize)
                );
        SetExtraStyle(wxWS_EX_PROCESS_IDLE);
        wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED);
        Connect(
            wxEVT_IDLE,
            wxIdleEventHandler(ExampleFrame::OnIdle)
                );
        Layout();
        oglExample->Reshape(GetSize().x, GetSize().y);
    }

    ~ExampleFrame(void)
    {
        Disconnect(wxEVT_SIZE);
        Disconnect(wxEVT_IDLE);
        wxIdleEvent::SetMode(wxIDLE_PROCESS_ALL);
        wxYield();
        wxSafeYield();
        wxYield();
        delete oglExample;
        oglExample = nullptr;
    }
};

class ExampleApp : public wxApp
{
public:
    bool OnInit(void)
    {
        bool result = false;
        try
        {
            SetTopWindow(new ExampleFrame());
            result = true;
        }
        catch(oglplus::Error& err)
        {
            std::cerr <<
                "Error (in gl" << err.GLSymbol() << ", " <<
                err.ClassName() << ": '" <<
                err.ObjectDescription() << "'): " <<
                err.what() <<
                " [" << err.File() << ":" << err.Line() << "] ";
            auto i = err.Properties().begin(), e = err.Properties().end();
            while(i != e)
            {
                std::cerr << "<" << i->first << "='" << i->second << "'>";
                ++i;
            }
            std::cerr << std::endl;
            err.Cleanup();
        }
        catch(const std::exception& se)
        {
            std::cerr << "Unspecified error: " << se.what() << std::endl;
        }
        return result;
    }
};

IMPLEMENT_APP(ExampleApp)

// return dvis_test(100, 100, 640, 480, argc, argv);
