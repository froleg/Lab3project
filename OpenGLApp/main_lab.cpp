#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL\glew.h>
#include <glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VBO, VAO, IBO, shader, uniformModel, uniformProjection;

const int NumVertices = 36;
bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

typedef  glm::vec4  point4;
typedef  glm::vec4  color4;

int Index = 0;  // global variable indexing into VBO arrays
point4  vPositions[NumVertices];
color4  vColors[NumVertices];

point4 positions[8] = {
point4(-0.5, -0.5,  0.5, 1.0),
point4(-0.5,  0.5,  0.5, 1.0),
point4(0.5,  0.5,  0.5, 1.0),
point4(0.5, -0.5,  0.5, 1.0),
point4(-0.5, -0.5, -0.5, 1.0),
point4(-0.5,  0.5, -0.5, 1.0),
point4(0.5,  0.5, -0.5, 1.0),
point4(0.5, -0.5, -0.5, 1.0)
};
color4 colors[8] = {
color4(0.0, 0.0, 0.0, 1.0),  // black
color4(1.0, 0.0, 0.0, 1.0),  // red
color4(1.0, 1.0, 0.0, 1.0),  // yellow
color4(0.0, 1.0, 0.0, 1.0),  // green
color4(0.0, 0.0, 1.0, 1.0),  // blue
color4(1.0, 0.0, 1.0, 1.0),  // magenta
color4(1.0, 1.0, 1.0, 1.0),  // white
color4(0.0, 1.0, 1.0, 1.0)   // cyan
};
void quad(int a, int b, int c, int d)
{
	vColors[Index] = colors[a]; vPositions[Index] = positions[a]; Index++;
	vColors[Index] = colors[b]; vPositions[Index] = positions[b]; Index++;
	vColors[Index] = colors[c]; vPositions[Index] = positions[c]; Index++;
	vColors[Index] = colors[a]; vPositions[Index] = positions[a]; Index++;
	vColors[Index] = colors[c]; vPositions[Index] = positions[c]; Index++;
	vColors[Index] = colors[d]; vPositions[Index] = positions[d]; Index++;
}

void colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}



// Vertex Shader code
static const char* vShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
in vec4 vPosition;																\n\
in vec4 vColor;																	\n\
out vec4 color;																  \n\
                                                                              \n\
uniform mat4 model;                                                           \n\
uniform mat4 projection;                                                      \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    gl_Position = projection * model * vPosition;						  \n\
	color = vColor;								  \n\
}";

// Fragment Shader
static const char* fShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
in vec4 color;																  \n\
                                                                              \n\
out vec4 fColor;                                                              \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    fColor= color;															  \n\
}";

void CreateTriangle()
{
//	unsigned int indices[] = { 
//		0, 3, 1,
//		1, 3, 2,
//		2, 3, 0,
//		0, 1, 2 
//	};

//	GLfloat vertices[] = {
//		-1.0f, -1.0f, 0.0f,
//		0.0f, -1.0f, 1.0f,
//		1.0f, -1.0f, 0.0f,
//		0.0f, 1.0f, 0.0f
//	};

	colorcube();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//glGenBuffers(1, &IBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositions) + sizeof(vColors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vPositions), vPositions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vPositions), sizeof(vColors), vColors);


	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);

	


	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) 
	{
		glGetShaderInfoLog(theShader, 1024, NULL, eLog);
		fprintf(stderr, "Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	shader = glCreateProgram();

	if (!shader) 
	{
		printf("Failed to create shader\n");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result) 
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result) 
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	GLuint vPosition = glGetAttribLocation(shader, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(shader, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vPositions)));

	uniformModel = glGetUniformLocation(shader, "model");
	uniformProjection = glGetUniformLocation(shader, "projection");
}

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW initialisation failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow Forward Compatbility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the window
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get Buffer Size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	glm::mat4 projection = glm::perspective(glm::radians(40.0f), (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		curAngle += 0.01f;
		if (curAngle >= 360)
		{
			curAngle -= 360;
		}

		if (direction)
		{
			curSize += 0.0001f;
		}
		else {
			curSize -= 0.0001f;
		}

		if (curSize >= maxSize || curSize <= minSize)
		{
			sizeDirection = !sizeDirection;
		}

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.5f, 0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));




		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//		glDrawElements(GL_TRIANGLES, NumVertices, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}