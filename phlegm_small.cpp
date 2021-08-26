#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include "MathHelper.h"
#include "phlegm_small.h"

using namespace std;

#define WIDTH 800
#define HEIGHT 600

#define USING_INDEX_BUFFER 1

#if USING_INDEX_BUFFER
    #define NUM_VERTICES num_vertices
    #define NUM_INDICES num_indices
#else
    #define NUM_VERTICES num_vertices
#endif

GLuint ShaderProgramID;
GLfloat light[] = {0.0f, 1.0f, 1.0f, 1.0f}; 
GLuint normalID;

// ==================== BEGIN NEW STUFF ============================
GLuint	perspectiveMatrixID, viewMatrixID, modelMatrixID;	// IDs of variables mP, mV and mM in the shader
GLuint lightID;
GLuint allRotsMatrixID;

GLfloat* allRotsMatrix;

GLfloat* rotXMatrix;	// Matrix for rotations about the X axis
GLfloat* rotYMatrix;	// Matrix for rotations about the Y axis
GLfloat* rotZMatrix;	// Matrix for rotations about the Z axis
GLfloat* transMatrix;	// Matrix for changing the position of the object
GLfloat* scaleMatrix;	// Duh..
GLfloat* tempMatrix1;	// A temporary matrix for holding intermediate multiplications
GLfloat* M;				// The final model matrix M to change into world coordinates

GLfloat* V;				// The camera matrix (for position/rotation) to change into camera coordinates
GLfloat* P;				// The perspective matrix for the camera (to give the scene depth); initialize this ONLY ONCE!

GLfloat  theta;			// An amount of rotation along one axis
GLfloat	 scaleAmount;	// In case the object is too big or small

void initMatrices() {

	theta = 0.0f;
	scaleAmount = 1.0f;

	// Allocate memory for the matrices and initialize them to the Identity matrix
	rotXMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotXMatrix);
	rotYMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotYMatrix);
	rotZMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotZMatrix);

    allRotsMatrix = new GLfloat[16]; MathHelper::makeIdentity(allRotsMatrix);

	transMatrix = new GLfloat[16];	MathHelper::makeIdentity(transMatrix);
	scaleMatrix = new GLfloat[16];	MathHelper::makeIdentity(scaleMatrix);
	tempMatrix1 = new GLfloat[16];	MathHelper::makeIdentity(tempMatrix1);

	M = new GLfloat[16];			MathHelper::makeIdentity(M);
	V = new GLfloat[16];			MathHelper::makeIdentity(V);
	P = new GLfloat[16];			MathHelper::makeIdentity(P);

	// Set up the (P)erspective matrix only once! Arguments are 1) the resulting matrix, 2) FoV, 3) aspect ratio, 4) near plane 5) far plane
	MathHelper::makePerspectiveMatrix(P, 60.0f, 1.0f, 1.0f, 1000.0f);
}
//==================== END NEW STUFF ==========================================

#pragma region Shader Related Function
string vertexShader = R"(
    #version 460

    in vec4 s_vPosition;
    in vec4 s_vNormal;

    //in vec4 s_vColor;
    //out vec4 color;

    uniform mat4 mM;
    uniform mat4 mV;
    uniform mat4 mP;
    uniform mat4 mRotations;

    uniform vec4 vLight;

    out vec3 fN;
    out vec3 fL;

    void main()
    {
        fN = (mRotations*s_vNormal).xyz;	// Rotate the normal! only take the first 3 parts, since fN is a vec3
	    fL = vLight.xyz;
        gl_Position = mP * mV * mM * s_vPosition; 
    }

)";

string fragmentShader = R"(
    #version 460

    in vec3 fN;
    in vec3 fL;

    out vec4 fColor;

    void main() 
    {
        vec3 N = normalize(fN);
        vec3 L = normalize(fL);
        float diffuse_intensity = max(dot(N, L), 0.0);
        //fColor = vec4(diffuse_intensity, diffuse_intensity, diffuse_intensity, 0.5);
        
        if (diffuse_intensity == 0) {
            fColor = vec4(0.05, 0.05, 0.05, 1.0);
        }
        else {
            fColor = vec4(diffuse_intensity, diffuse_intensity, diffuse_intensity, 0.5);
        }
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

	theta+= 0.01f;
	scaleAmount = sin(theta);

	// Fill the matrices with valid data
	MathHelper::makeScale(scaleMatrix, 0.01f, 0.01f, 0.01f);			// Fill the scaleMatrix variable
	MathHelper::makeRotateY(rotYMatrix, theta);						// Fill the rotYMatrix variable
	MathHelper::makeTranslate(transMatrix, 0.0f, -0.25f, -2.0f);		// Fill the transMatrix to push the model back 1 "unit" into the scene
	
	// Multiply them together 
	//MathHelper::matrixMult4x4(M, transMatrix, rotYMatrix);	// Order is important!
	//MathHelper::matrixMult4x4(M, rotYMatrix, transMatrix);	// This is backwards, unless you want orbiting!

	MathHelper::matrixMult4x4(tempMatrix1, rotYMatrix, scaleMatrix);	// Scale, then rotate...
	MathHelper::matrixMult4x4(M, transMatrix, tempMatrix1);	// ... then multiply THAT by the translate
    
    MathHelper::copyMatrix(rotYMatrix, allRotsMatrix);
		
    //Set the (V)iew matrix if you want to "move" around the scene
    // MathHelper::makeTranslate(V, 0.0f, 1.0f, 0.0f);			// Fill the (V)iew matrix to push the world up 1 unit

	// Important! Pass that data to the shader variables
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, M);
	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, V);
	glUniformMatrix4fv(perspectiveMatrixID, 1, GL_TRUE, P);
    glUniformMatrix4fv(allRotsMatrixID, 1, GL_TRUE, allRotsMatrix);

    glUniform4fv(lightID, 1, light);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
	glutCreateWindow("small phlegm");
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
    // GLfloat vertices[] = {0.0f, 0.5f, 0.0f, //0
    //                     -0.25f, 0.0f, 0.0f, //1
    //                     0.25f, 0.0f, 0.0f, //2
    //                     -0.5f, -0.5f, 0.0f, //3
    //                     0.0f, -0.5f, 0.0f, //4
    //                     0.5f, -0.5f, 0.0f //5
    // };

    // GLfloat colors[] = { 1.0f, 0.0f, 0.0f, 1.0f, //0
    //                     1.0f, 1.0f, 0.0f, 1.0f, //1
    //                     1.0f, 1.0f, 0.0f, 1.0f, //2
    //                     0.0f, 0.0f, 1.0f, 1.0f, //3
    //                     1.0f, 1.0f, 0.0f, 1.0f, //4
    //                     0.0f, 1.0f, 0.0f, 1.0f //5

    // };

    // GLuint indices[] = {0, 1, 2, 2, 4, 5, 1, 3, 4, 1,2,4};
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
	// Create the "remember all"
    GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * NUM_VERTICES * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * NUM_VERTICES * sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * NUM_VERTICES * sizeof(GLfloat), 3 * NUM_VERTICES * sizeof(GLfloat), normals);    

#if USING_INDEX_BUFFER
    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES*sizeof(GLuint), indices, GL_STATIC_DRAW);
#endif

    GLuint positionID, colorID;
    positionID = glGetAttribLocation(ShaderProgramID, "s_vPosition");
    normalID = glGetAttribLocation(ShaderProgramID, "s_vNormal");
    lightID = glGetAttribLocation(ShaderProgramID, "vLight");

    cout << "postionID = " << positionID <<endl;
    cout << "colorID = " << colorID <<endl;

 	perspectiveMatrixID = glGetUniformLocation(ShaderProgramID, "mP");
	viewMatrixID = glGetUniformLocation(ShaderProgramID, "mV");
	modelMatrixID = glGetUniformLocation(ShaderProgramID, "mM"); 
    allRotsMatrixID = glGetUniformLocation(allRotsMatrixID, "mRotations") ;

    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(colorID, 3, GL_FLOAT, GL_FALSE, 0, ((char*)NULL + sizeof(vertices)) );
    
    glUseProgram(ShaderProgramID);

    glEnableVertexAttribArray(positionID);
    glEnableVertexAttribArray(colorID);

	glEnable(GL_CULL_FACE);  // NEW! - we're doing real 3D now...  Cull (don't render) the backsides of triangles
	glCullFace(GL_BACK);	// Other options?  GL_FRONT and GL_FRONT_AND_BACK
	glEnable(GL_DEPTH_TEST);// Make sure the depth buffer is on.  As you draw a pixel, update the screen only if it's closer than previous ones


	glutMainLoop();

	return 0;
}
