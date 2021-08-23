#define NDEBUG
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>

using namespace std;

#define WIDTH 800
#define HEIGHT 600

#define BUFFER_OFFSET(i) ((char*)NULL + （i))
GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint positionID, colorID;


static char* readFile(const char* filename)
{
    FILE* fp = fopen(filename,"r");
    fseek(fp, 0, SEEK_END);
    long file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* contents = new char[file_length+1];
    for(int i=0; i < file_length+1; i++)
    {
        contents[i] = 0;
    }

    fread(contents, 1, file_length, fp);

    contents[file_length+1] = '\0';
    fclose(fp);

    return contents;
}

void ChangeViewport(int w, int h)
{
	glViewport(0, 0, w, h);
}

void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    //GLfloat vRed[] = {0.0f, 0.0f, 1.0f, 1.0f};
	//glRectf(-0.9f, -0.9f, 0.5f, 0.5f);
	//
    glDrawArrays(GL_TRIANGLES, 0, 3);
    //glFlush();
	glutSwapBuffers();
}

string vertexShader = R"(
    #version 430
    in vec3 pos;
    void main()
    {
        gl_Position = vec4(pos, 1);
    }
)";

string fragmentShader = R"(
    #version 430
    void main() 
    {
        gl_FragColor = vec4(0, 0, 1, 1); // white color
    }
)";

// Compile and create shader object and returns its id
GLuint compileShaders(string shader, GLenum type)
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

// Creates a program containing vertex and fragment shader
// links it and returns its ID
GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
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

// Load data in VBO (Vertex Buffer Object) and return the vbo's id
GLuint loadDataInBuffers()
{
    GLfloat vertices[] = {// triangle vertex coordinates
                          -0.5, -0.5, 0,
                          0.5, -0.5, 0,
                          0, 0.5, 0};

    GLuint vboId;

    // allocate buffer sapce and pass data to it
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // unbind the active buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vboId;
}

void Init()
{
    glClearColor(0,0,0,0);

    GLuint vboId = loadDataInBuffers();

    GLuint vShaderId = compileShaders(vertexShader, GL_VERTEX_SHADER);
    GLuint fShaderId = compileShaders(fragmentShader, GL_FRAGMENT_SHADER);

    GLuint programId = linkProgram(vShaderId, fShaderId);

    // Get the 'pos' variable location inside this program
    GLuint posAttributePosition = glGetAttribLocation(programId, "pos");

    GLuint vaoId;
    glGenVertexArrays(1, &vaoId); // Generate VAO  (Vertex Array Object)

    // Bind it so that rest of vao operations affect this vao
    glBindVertexArray(vaoId);

    // buffer from which 'pos' will receive its data and the format of that data
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(posAttributePosition, 3, GL_FLOAT, false, 0, 0);

    // Enable this attribute array linked to 'pos'
    glEnableVertexAttribArray(posAttributePosition);

    // Use this program for rendering.
    glUseProgram(programId);

}

GLuint compileShader(const char* shaderSource, GLenum type)
{
    GLuint ShaderID = glCreateShader(type);
    glShaderSource (ShaderID, 1, (const GLchar**)&shaderSource, NULL);
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
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
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
   char* vertexShadeSourceCode = readFile("triangles.vs");
   char* fragmentShaderSourceCode = readFile("triangles.fs");
   GLuint vertexShaderID = compileShader(vertexShadeSourceCode, GL_VERTEX_SHADER);
   GLuint fragmentShaderID = compileShader(fragmentShaderSourceCode, GL_FRAGMENT_SHADER);
   GLuint ShaderProgramID = LinkedProgram(vertexShaderID,fragmentShaderID);

   cout << "vertexShaderID = "<< vertexShaderID << endl;
   cout << "fragmentShaderID = "<< fragmentShaderID << endl;
   cout << "ShaderProgramID = "<< ShaderProgramID << endl;
   cout << "vertexShadeSourceCode[0]=" << vertexShadeSourceCode[0]<< endl;
   cout << "fragmentShaderSourceCode[0]=" << fragmentShaderSourceCode[0]<< endl;

    //vertex buffers 
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, 7*3*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*3*sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3*3*sizeof(GLfloat), 3*4*sizeof(GLfloat), colors);

    positionID = glGetAttribLocation(ShaderProgramID, "i_vPosition");
    colorID = glGetAttribLocation(ShaderProgramID, "i_Color");

    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, ((char*)NULL + 3*3*sizeof(GLfloat) ) );
    glUseProgram(ShaderProgramID);
    glEnableVertexAttribArray(positionID);
    glEnableVertexAttribArray(colorID);


	glutMainLoop();

	return 0;
}
