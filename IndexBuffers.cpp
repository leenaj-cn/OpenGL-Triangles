#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>

using namespace std;

#define WIDTH 800
#define HEIGHT 600

GLuint vao = 0;
GLuint vbo;
GLuint positionID, colorID;

string vertexShader = R"(
    #version 460
    in vec4 s_vPosition;
    in vec4 s_vColor;
    out vec4 color;

    void main()
    {
        color = s_vColor;
        gl_Position = s_vPosition; 
    }
)";

string fragmentShader = R"(
    #version 460
    in vec4 color;
    out vec4 fColor;

    void main() 
    {
        fColor = color;//vec4(1, 0, 0, 1); //  color
    }
)";

void ChangeViewport(int w, int h)
{
	glViewport(0, 0, w, h);
}

void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
	glutSwapBuffers();
}

GLuint compileShader(string shader, GLenum type)
{
    const char *shaderCode = shader.c_str();
    GLuint ShaderID = glCreateShader(type);
    glShaderSource (ShaderID, 1, (const GLchar**)&shader, NULL);
    glCompileShader(ShaderID);
    
    return ShaderID;
}

GLuint LinkedProgram(GLuint vertexShaderID, GLuint fragmentShaderID)
{
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    return programID;
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("shaders");
    glutReshapeFunc(ChangeViewport);
	glutDisplayFunc(&render);
	glewInit();

    //Init();
    GLfloat vertices[] = {-0.5f, -0.5f, 0.0f, 
                        0.5f, -0.5f, 0.0f, 
                        0.0f, 0.5f, 0.0f};
    GLfloat colors[] = {1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 1.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 1.0f,};

    //shaders
    GLuint vertexShaderID = compileShader(vertexShader, GL_VERTEX_SHADER); //vertexShader
    GLuint fragmentShaderID = compileShader(fragmentShader, GL_FRAGMENT_SHADER); //fragmentShader

    GLuint ShaderProgramID = LinkedProgram(vertexShaderID,fragmentShaderID);

    cout << "vertexShaderID = "<< vertexShaderID << endl;
    cout << "fragmentShaderID = "<< fragmentShaderID << endl;
    cout << "ShaderProgramID = "<< ShaderProgramID << endl;

    //vertex buffers 
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, 7*3*sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*3*sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3*3*sizeof(GLfloat), 3*4*sizeof(GLfloat), colors);

    positionID = glGetAttribLocation(ShaderProgramID, "s_vPosition");
    colorID = glGetAttribLocation(ShaderProgramID, "s_vColor");

    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, ((char*)NULL + 3*3*sizeof(GLfloat)) );
    
    glUseProgram(ShaderProgramID);
    glEnableVertexAttribArray(positionID);
    glEnableVertexAttribArray(colorID);

	glutMainLoop();

	return 0;
}
