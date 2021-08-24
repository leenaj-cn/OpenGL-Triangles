#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>

using namespace std;

#define WIDTH 800
#define HEIGHT 600

#define USING_INDEX_BUFFER 0

#if USING_INDEX_BUFFER
    #define NUM_VERTICES 6
    #define NUM_INDICES 9
#else
    #define NUM_VERTICES 9
#endif

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
        fColor = color;
    }
)";

void ChangeViewport(int w, int h)
{
	glViewport(0, 0, w, h);
}

void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#if USING_INDEX_BUFFER   
    glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL); //do not use glDrawArrays() which is non-index buffer way
#else
    glDrawArrays(GL_TRIANGLES,0,NUM_VERTICES);
#endif
    

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
	glutCreateWindow("index buffers");
    glutReshapeFunc(ChangeViewport);
	glutDisplayFunc(&render);
	glewInit();

#if USING_INDEX_BUFFER
    GLfloat vertices[] = {0.0f, 0.5f, 0.0f, //0
                        -0.25f, 0.0f, 0.0f, //1
                        0.25f, 0.0f, 0.0f, //2
                        -0.5f, -0.5f, 0.0f, //3
                        0.0f, -0.5f, 0.0f, //4
                        0.5f, -0.5f, 0.0f //5
    };

    GLfloat colors[] = { 1.0f, 0.0f, 0.0f, 1.0f, //0
                        0.0f, 1.0f, 0.0f, 1.0f, //1
                        0.0f, 0.0f, 1.0f, 1.0f, //2
                        0.0f, 0.0f, 1.0f, 1.0f, //3
                        1.0f, 0.0f, 0.0f, 1.0f, //4
                        0.0f, 1.0f, 0.0f, 1.0f //5

    };

    GLfloat indices[] = {0, 1, 2, 1, 3, 4, 2, 4, 5};
#else
    GLfloat vertices[] = {-0.5f, -0.5f, 0.0f, //3
                        0.0f, -0.5f, 0.0f, //4
                        -0.25f, 0.0f, 0.0f, //1
                        0.25f, 0.0f, 0.0f, //2
                        0.0f, -0.5f, 0.0f, //4
                        0.5f, -0.5f, 0.0f, //5
                        0.0f, 0.5f, 0.0f, //0
                        -0.25f, 0.0f, 0.0f, //1
                        0.25f, 0.0f, 0.0f //2
    };
    GLfloat colors[] = {0.0f, 0.0f, 1.0f, 1.0f, //B
                        1.0f, 0.0f, 0.0f, 1.0f, //R
                        0.0f, 1.0f, 0.0f, 1.0f, //G
                        0.0f, 1.0f, 0.0f, 1.0f,
                        1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 1.0f,
                        1.0f, 0.0f, 0.0f, 1.0f, 
                        0.0f, 1.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 1.0f

    };
#endif

    //shaders
    GLuint vertexShaderID = compileShader(vertexShader, GL_VERTEX_SHADER); //vertexShader
    GLuint fragmentShaderID = compileShader(fragmentShader, GL_FRAGMENT_SHADER); //fragmentShader

    GLuint ShaderProgramID = LinkedProgram(vertexShaderID,fragmentShaderID);

    cout << "vertexShaderID = "<< vertexShaderID << endl;
    cout << "fragmentShaderID = "<< fragmentShaderID << endl;
    cout << "ShaderProgramID = "<< ShaderProgramID << endl;
    
    //vertex buffers 
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 7 * NUM_VERTICES * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * NUM_VERTICES * sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * NUM_VERTICES * sizeof(GLfloat), 4*NUM_VERTICES*sizeof(GLfloat), colors);    

#if USING_INDEX_BUFFER
    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES*sizeof(GLuint), indices, GL_STATIC_DRAW);
#endif

    GLuint positionID, colorID;
    positionID = glGetAttribLocation(ShaderProgramID, "s_vPosition");
    colorID = glGetAttribLocation(ShaderProgramID, "s_vColor");
    cout << "postionID = " << positionID <<endl;
    cout << "colorID = " << colorID <<endl;

    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, ((char*)NULL + sizeof(vertices)) );
    
    glUseProgram(ShaderProgramID);
    glEnableVertexAttribArray(positionID);
    glEnableVertexAttribArray(colorID);

	glutMainLoop();

	return 0;
}
