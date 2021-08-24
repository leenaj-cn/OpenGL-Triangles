//glew is used for run time loading of the OpenGL API(for modern ogl api,version 3.2+)
#include <GL\glew.h>
// glfw/freeglut allow us to create a window, and receive mouse and keyboard input in a cross-platform way. 
#include <GL\freeglut.h>

#include <iostream>
#include <string>

using namespace std;

#define WIDTH 800
#define HEIGHT 600

string vertexShader = R"(
    #version 460
    in vec4 s_vPosition;
    void main()
    {
        gl_Position = s_vPosition; 
    }
)";

string fragmentShader = R"(
    #version 460
    out vec4 fColor;

    void main() 
    {
        fColor = vec4(1, 0, 0, 1); 
    }
)";

void ChangeViewport(int w, int h);
void Render();
GLuint CompileShaders(string shader, GLenum type)
{
    const char *shaderCode = shader.c_str();
    GLuint shaderId = glCreateShader(type);
    if (shaderId == 0)
    { // Error: Cannot create shader object
        cout << "Error creating shaders!";
        return 0;
    }

    // Attach source code to this object
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId); // compile the shader object

    //check status
    GLint compileStatus;
    // check for compilation status
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus)
    { // If compilation was not successfull
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char *cMessage = new char[length];

        // Get additional information
        glGetShaderInfoLog(shaderId, length, &length, cMessage);
        cout << "Cannot Compile Shader: " << cMessage;
        delete[] cMessage;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

GLuint LinkedProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
   GLuint programId = glCreateProgram(); // crate a program
    if (programId == 0)
    {
        cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Create executable of this program
    glLinkProgram(programId);

    //check status
    GLint linkStatus;
    // Get the link status for this program
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    { // If the linking failed
        cout << "Error Linking program";
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);
        glDeleteProgram(programId);

        return 0;
    }

    return programId;
}


int main(int argc, char** argv)
{
    //Init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Hell, Triangle");
    glewInit();

    //called when the window is resized
    glutReshapeFunc(ChangeViewport);
    //called when the window needs to be redrawn
    glutDisplayFunc(Render);
    
    //make a shader
    GLuint vertexShaderID = CompileShaders(vertexShader, GL_VERTEX_SHADER);
    GLuint fragmentShaderID = CompileShaders(fragmentShader, GL_FRAGMENT_SHADER);
    GLuint ShaderProgramID = LinkedProgram(vertexShaderID,fragmentShaderID);

    //vertex buffer
    GLfloat vertices[] = {// triangle vertex coordinates
                          -0.5, -0.5, 0,  //lower-left
                          0.5, -0.5, 0,  //lower-right
                          0, 0.5, 0};  //top

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*3*sizeof(GLfloat), vertices);

    GLuint positionID;
    positionID = glGetAttribLocation(ShaderProgramID, "s_vPosition");
    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionID);

    glUseProgram(ShaderProgramID);

    glutMainLoop();
    return 0;

}

void ChangeViewport(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Render()
{
    glClear(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glutSwapBuffers();
}


