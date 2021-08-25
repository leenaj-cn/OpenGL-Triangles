#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include "MathHelper.h"

using namespace std;

#define WIDTH 800
#define HEIGHT 600

#define USING_INDEX_BUFFER 1

#if USING_INDEX_BUFFER
    #define NUM_VERTICES 6
    #define NUM_INDICES 12
#else
    #define NUM_VERTICES 9
#endif

GLuint ShaderProgramID;

//========Matrix===========
GLfloat theta;
GLfloat scaleAmount;

GLfloat* M;	
GLfloat* V;
GLfloat* P;

GLfloat* scaleMatrix;

GLfloat* rotXMatrix;
GLfloat* rotYMatrix;
GLfloat* rotZMatrix;

GLfloat* transMatrix;
GLfloat* tempMatrix1;

GLuint perspectiveMatrixID, viewMatrixID, modelMatrixID;

void initMatrices() {

	theta = 0.0f;
	scaleAmount = 1.0f;

	// Allocate memory for the matrices and initialize them to the Identity matrix
	rotXMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotXMatrix);
	rotYMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotYMatrix);
	rotZMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotZMatrix);
	transMatrix = new GLfloat[16];	MathHelper::makeIdentity(transMatrix);
	scaleMatrix = new GLfloat[16];	MathHelper::makeIdentity(scaleMatrix);
	tempMatrix1 = new GLfloat[16];	MathHelper::makeIdentity(tempMatrix1);
	M = new GLfloat[16];			MathHelper::makeIdentity(M);
	V = new GLfloat[16];			MathHelper::makeIdentity(V);
	P = new GLfloat[16];			MathHelper::makeIdentity(P);

	// Set up the (P)erspective matrix only once! Arguments are 1) the resulting matrix, 2) FoV, 3) aspect ratio, 4) near plane 5) far plane
	MathHelper::makePerspectiveMatrix(P, 60.0f, 1.0f, 1.0f, 1000.0f);
}

#pragma region Shader Related Function
string vertexShader = R"(
    #version 460
    in vec4 s_vPosition;

    in vec4 s_vColor;
    out vec4 color;

    uniform mat4 mM; //from local to camera
    uniform mat4 mV; //from camera to world
    uniform mat4 mP; //from world to NDCs, perspective matrix for depth

    void main()
    {
        color = s_vColor;

        gl_Position = mP * mV * mM * s_vPosition; 
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
#pragma endregion Shader Function

void ChangeViewport(int w, int h)
{
	glViewport(0, 0, w, h);
}

void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(ShaderProgramID);

    theta += 0.01f;
    scaleAmount = sin(theta);

    MathHelper::makeScale(scaleMatrix, 0.1f, 0.1f, 0.1f);
    MathHelper::makeRotateY(rotYMatrix, theta);
    MathHelper::makeTranslate(transMatrix, 0.0f, -0.25f, -2.0f);
    // MathHelper::makeTranslate(V, 0.0f, 1.0f, 0.0f);

    MathHelper::matrixMult4x4(M, transMatrix, rotYMatrix);
   // MathHelper::matrixMult4x4(M, rotYMatrix, transMatrix);
    //MathHelper::matrixMult4x4(tempMatrix1, rotYMatrix, scaleMatrix);
    //MathHelper::matrixMult4x4(M, transMatrix, tempMatrix1);

    //Pass data to shader variables
    glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, M);
    glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, V);
    glUniformMatrix4fv(perspectiveMatrixID, 1, GL_TRUE, P);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#if USING_INDEX_BUFFER   
    glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL); //do not use glDrawArrays() which is non-index buffer way
#else
    glDrawArrays(GL_TRIANGLES,0,NUM_VERTICES);
#endif
    
	glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Matrix");
    glutReshapeFunc(ChangeViewport);
	glutDisplayFunc(&render);
	glewInit();

    initMatrices();

    //shaders
    GLuint vertexShaderID = compileShader(vertexShader, GL_VERTEX_SHADER); //vertexShader
    GLuint fragmentShaderID = compileShader(fragmentShader, GL_FRAGMENT_SHADER); //fragmentShader
    ShaderProgramID = LinkedProgram(vertexShaderID,fragmentShaderID);

    cout << "vertexShaderID = "<< vertexShaderID << endl;
    cout << "fragmentShaderID = "<< fragmentShaderID << endl;
    cout << "ShaderProgramID = "<< ShaderProgramID << endl;

#if USING_INDEX_BUFFER
    GLfloat vertices[] = {0.0f, 0.5f, 0.0f, //0
                        -0.25f, 0.0f, 0.0f, //1
                        0.25f, 0.0f, 0.0f, //2
                        -0.5f, -0.5f, 0.0f, //3
                        0.0f, -0.5f, 0.0f, //4
                        0.5f, -0.5f, 0.0f //5
    };

    GLfloat colors[] = { 1.0f, 0.0f, 0.0f, 1.0f, //0
                        1.0f, 1.0f, 0.0f, 1.0f, //1
                        1.0f, 1.0f, 0.0f, 1.0f, //2
                        0.0f, 0.0f, 1.0f, 1.0f, //3
                        1.0f, 1.0f, 0.0f, 1.0f, //4
                        0.0f, 1.0f, 0.0f, 1.0f //5

    };

    GLuint indices[] = {0, 1, 2, 2, 4, 5, 1, 3, 4, 1,2,4};
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

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, 7 * NUM_VERTICES * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * NUM_VERTICES * sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * NUM_VERTICES * sizeof(GLfloat), 4 * NUM_VERTICES * sizeof(GLfloat), colors);    

#if USING_INDEX_BUFFER
    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES*sizeof(GLuint), indices, GL_STATIC_DRAW);
#endif

    //location -- variable position, color from shader
    GLuint positionID, colorID;
    positionID = glGetAttribLocation(ShaderProgramID, "s_vPosition");
    colorID = glGetAttribLocation(ShaderProgramID, "s_vColor");
    cout << "postionID = " << positionID <<endl;
    cout << "colorID = " << colorID <<endl;

    //location -- find matrix from shader
    
    perspectiveMatrixID = glGetUniformLocation(ShaderProgramID, "mP");
    viewMatrixID = glGetUniformLocation(ShaderProgramID, "mV");
    modelMatrixID = glGetUniformLocation(ShaderProgramID, "mM");
    cout << "perspectiveMatrixID = " << perspectiveMatrixID <<endl;
    cout << "viewMatrixID = " << viewMatrixID <<endl;    
    cout << "modelMatrixID = " << modelMatrixID <<endl;  

    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, ((char*)NULL + sizeof(vertices)) );
    
    glUseProgram(ShaderProgramID);
    glEnableVertexAttribArray(positionID);
    glEnableVertexAttribArray(colorID);

	glutMainLoop();

	return 0;
}
