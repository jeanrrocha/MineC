#include <stdio.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#define FRAGMENT 'f'
#define VERTEX 'v'

#define CUBE 192
//CUBE = sizeof(vertices)

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 direction	= glm::vec3(0.0f, 0.0f, 0.0f);

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 proj = glm::mat4(1.0f);

GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

GLfloat lastX = 400, lastY = 400;
int firstMouse = true;

GLuint VAO, VBO, EBO;
GLuint shaderProgram;

const char* getShader (char *filePath, char type);
void closeWindow (GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

int main(void)
{	
	GLFWwindow* window;
	
	const char *vertexShaderSource = getShader("default.shader", VERTEX);
	//printf ("%s", vertexShaderSource);
	
	const char *fragmentShaderSource = getShader("default.shader", FRAGMENT);
	//printf ("%s", fragmentShaderSource);
	
	if (!glfwInit()){
		printf ("Failed to initialize GLFW.\n");
		return -1;
	}
	
	const int windowWidth = 800;
	const int windowHeight = 800;
	const char* windowTitle = "MineC";
	
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	if (!window){
		printf ("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK){
		printf ("Failed to initialize GLEW\n");
		return -1;
	}
	
	glViewport(0, 0, windowWidth, windowHeight);
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	
	shaderProgram = glCreateProgram();
	
	glAttachShader (shaderProgram, vertexShader);
	glAttachShader (shaderProgram, fragmentShader);
	
	glLinkProgram (shaderProgram);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLfloat vertices[] =
	{ //     COORDINATES     /        COLORS      
		-0.5f, -0.5f, -0.5f,	0.3411f, 0.2549f, 0.1569f,
		-0.5f, -0.5f, 0.5f,		0.3411f, 0.2549f, 0.1569f,
		-0.5f, 0.5f, 0.5f,		0.3411f, 0.2549f, 0.1569f,
		0.5f, 0.5f, -0.5f,		0.3411f, 0.2549f, 0.1569f,
		-0.5f, 0.5f, -0.5f,		0.3411f, 0.2549f, 0.1569f,
		0.5f,-0.5f, 0.5f,		0.3411f, 0.2549f, 0.1569f,
		0.5f,-0.5f,-0.5f,		0.3411f, 0.2549f, 0.1569f,
		0.5f, 0.5f, 0.5f,		0.3411f, 0.2549f, 0.1569f
	};
	
	GLfloat grass[] = 
	{ //     COORDINATES     /        COLORS      
		-0.5f, -0.5f, -0.5f,	0.3411f, 0.2549f, 0.1569f,
		-0.5f, -0.5f, 0.5f,		0.3411f, 0.2549f, 0.1569f,
		-0.5f, 0.5f, 0.5f,		0.0078f, 0.5412f, 0.0588f,
		0.5f, 0.5f, -0.5f,		0.0078f, 0.5412f, 0.0588f,
		-0.5f, 0.5f, -0.5f,		0.0078f, 0.5412f, 0.0588f,
		0.5f,-0.5f, 0.5f,		0.3411f, 0.2549f, 0.1569f,
		0.5f,-0.5f,-0.5f,		0.3411f, 0.2549f, 0.1569f,
		0.5f, 0.5f, 0.5f,		0.0078f, 0.5412f, 0.0588f
	};
	
	GLfloat log[] = 
	{ //     COORDINATES     /        COLORS      
		-0.5f, -0.5f, -0.5f,	0.2902, 0.1725f, 0.1725f,
		-0.5f, -0.5f, 0.5f,		0.2902, 0.1725f, 0.1725f,
		-0.5f, 0.5f, 0.5f,		0.2902, 0.1725f, 0.1725f,
		0.5f, 0.5f, -0.5f,		0.2902, 0.1725f, 0.1725f,
		-0.5f, 0.5f, -0.5f,		0.2902, 0.1725f, 0.1725f,
		0.5f,-0.5f, 0.5f,		0.2902, 0.1725f, 0.1725f,
		0.5f,-0.5f,-0.5f,		0.2902, 0.1725f, 0.1725f,
		0.5f, 0.5f, 0.5f,		0.2902, 0.1725f, 0.1725f
	};
	
	GLfloat leaves[] = 
	{ //     COORDINATES     /        COLORS      
		-0.5f, -0.5f, -0.5f,	0.0078f, 0.5412f, 0.0588f,
		-0.5f, -0.5f, 0.5f,		0.0078f, 0.5412f, 0.0588f,
		-0.5f, 0.5f, 0.5f,		0.0078f, 0.5412f, 0.0588f,
		0.5f, 0.5f, -0.5f,		0.0078f, 0.5412f, 0.0588f,
		-0.5f, 0.5f, -0.5f,		0.0078f, 0.5412f, 0.0588f,
		0.5f,-0.5f, 0.5f,		0.0078f, 0.5412f, 0.0588f,
		0.5f,-0.5f,-0.5f,		0.0078f, 0.5412f, 0.0588f,
		0.5f, 0.5f, 0.5f,		0.0078f, 0.5412f, 0.0588f
	};

	GLuint indices[] =
	{
		0, 1, 2,
		3, 0, 4,
		5, 0, 6,
		3, 6, 0,
		0, 2, 4,
		5, 1, 0,
		2, 1, 5,
		7, 6, 3,
		6, 7, 5,
		7, 3, 4,
		7, 4, 2,
		7, 2, 5
	};
	
	
	
	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 0.0f, 1.0f),
	glm::vec3(-1.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(1.0f, 0.0f, -1.0f),
	glm::vec3(-1.0f, 0.0f, -1.0f),
	
	glm::vec3(-3.0f, 0.0f, 0.0f),
	glm::vec3(-2.0f, 0.0f, 0.0f),
	glm::vec3(-4.0f, 0.0f, 0.0f),
	glm::vec3(-3.0f, 0.0f, 1.0f),
	glm::vec3(-2.0f, 0.0f, 1.0f),
	glm::vec3(-4.0f, 0.0f, 1.0f),
	glm::vec3(-3.0f, 0.0f, -1.0f),
	glm::vec3(-2.0f, 0.0f, -1.0f),
	glm::vec3(-4.0f, 0.0f, -1.0f),
	
	glm::vec3(-3.0f, 0.0f, 3.0f),
	glm::vec3(-2.0f, 0.0f, 3.0f),
	glm::vec3(-4.0f, 0.0f, 3.0f),
	glm::vec3(-3.0f, 0.0f, 4.0f),
	glm::vec3(-2.0f, 0.0f, 4.0f),
	glm::vec3(-4.0f, 0.0f, 4.0f),
	glm::vec3(-3.0f, 0.0f, 2.0f),
	glm::vec3(-2.0f, 0.0f, 2.0f),
	glm::vec3(-4.0f, 0.0f, 2.0f),
	
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(1.0f, -1.0f, 0.0f),
	glm::vec3(-1.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 1.0f),
	glm::vec3(1.0f, -1.0f, 1.0f),
	glm::vec3(-1.0f, -1.0f, 1.0f),
	glm::vec3(0.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f),
	
	glm::vec3(-3.0f, -1.0f, 0.0f),
	glm::vec3(-2.0f, -1.0f, 0.0f),
	glm::vec3(-4.0f, -1.0f, 0.0f),
	glm::vec3(-3.0f, -1.0f, 1.0f),
	glm::vec3(-2.0f, -1.0f, 1.0f),
	glm::vec3(-4.0f, -1.0f, 1.0f),
	glm::vec3(-3.0f, -1.0f, -1.0f),
	glm::vec3(-2.0f, -1.0f, -1.0f),
	glm::vec3(-4.0f, -1.0f, -1.0f),
	
	glm::vec3(-3.0f, -1.0f, 3.0f),
	glm::vec3(-2.0f, -1.0f, 3.0f),
	glm::vec3(-4.0f, -1.0f, 3.0f),
	glm::vec3(-3.0f, -1.0f, 4.0f),
	glm::vec3(-2.0f, -1.0f, 4.0f),
	glm::vec3(-4.0f, -1.0f, 4.0f),
	glm::vec3(-3.0f, -1.0f, 2.0f),
	glm::vec3(-2.0f, -1.0f, 2.0f),
	glm::vec3(-4.0f, -1.0f, 2.0f),
	
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(-1.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f),
	glm::vec3(0.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, 1.0f, -1.0f),
	
	glm::vec3(-3.0f, 1.0f, 0.0f),
	glm::vec3(-2.0f, 1.0f, 0.0f),
	glm::vec3(-4.0f, 1.0f, 0.0f),
	glm::vec3(-3.0f, 1.0f, 1.0f),
	glm::vec3(-2.0f, 1.0f, 1.0f),
	glm::vec3(-4.0f, 1.0f, 1.0f),
	glm::vec3(-3.0f, 1.0f, -1.0f),
	glm::vec3(-2.0f, 1.0f, -1.0f),
	glm::vec3(-4.0f, 1.0f, -1.0f),
	
	glm::vec3(-3.0f, 1.0f, 3.0f),
	glm::vec3(-2.0f, 1.0f, 3.0f),
	glm::vec3(-4.0f, 1.0f, 3.0f),
	glm::vec3(-3.0f, 1.0f, 4.0f),
	glm::vec3(-2.0f, 1.0f, 4.0f),
	glm::vec3(-4.0f, 1.0f, 4.0f),
	glm::vec3(-3.0f, 1.0f, 2.0f),
	glm::vec3(-2.0f, 1.0f, 2.0f),
	glm::vec3(-4.0f, 1.0f, 2.0f),
	
	glm::vec3(-3.0f, 2.0f, 0.0f),
	glm::vec3(-3.0f, 3.0f, 0.0f),
	glm::vec3(-3.0f, 4.0f, 0.0f),
	glm::vec3(-3.0f, 5.0f, 0.0f),
	glm::vec3(-3.0f, 6.0f, 0.0f),
	
	glm::vec3(-2.0f, 6.0f, 0.0f),
	glm::vec3(-4.0f, 6.0f, 0.0f),
	glm::vec3(-3.0f, 6.0f, 1.0f),
	glm::vec3(-2.0f, 6.0f, 1.0f),
	glm::vec3(-4.0f, 6.0f, 1.0f),
	glm::vec3(-3.0f, 6.0f, -1.0f),
	glm::vec3(-2.0f, 6.0f, -1.0f),
	glm::vec3(-4.0f, 6.0f, -1.0f),
	
	glm::vec3(-3.0f, 7.0f, 0.0f),
	glm::vec3(-2.0f, 7.0f, 0.0f),
	glm::vec3(-4.0f, 7.0f, 0.0f),
	glm::vec3(-3.0f, 7.0f, 1.0f),
	glm::vec3(-2.0f, 7.0f, 1.0f),
	glm::vec3(-4.0f, 7.0f, 1.0f),
	glm::vec3(-3.0f, 7.0f, -1.0f),
	glm::vec3(-2.0f, 7.0f, -1.0f),
	glm::vec3(-4.0f, 7.0f, -1.0f),
	
	glm::vec3(-3.0f, 8.0f, 0.0f)
};
	
	//Gerando o VertexArry, VertexBuffer e o IndexBuffer.
	
	GLuint VAOID[4] = {0};
	
	for (int i = 0; i < sizeof(VAOID) / sizeof(GLuint); i++)
		glGenVertexArrays(1, &VAOID[i]);
	
	//glGenVertexArrays(1, &VAOID[0]);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
		//glGenVertexArrays(1, &VAOID[1]);
	
	GLuint EBOGrass;
	glGenBuffers(1, &EBOGrass);
	
	
	
	
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, CUBE * sizeof(VAOID) / sizeof(GLuint), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*0, CUBE, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*1, CUBE, grass);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*2, CUBE, log);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*3, CUBE, leaves);

	//GLint size = 0;
	//glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	for (int i = 0; i < sizeof(VAOID) / sizeof(GLuint); i++){
		glBindVertexArray(VAOID[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(CUBE*i));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(CUBE*i + 3*sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	
	/*glBindVertexArray(VAOID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)CUBE*1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(CUBE*1 + 3*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/

	
	
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetKeyCallback(window, closeWindow);
	
	float cameraSpeed = 0.05f;
	
	
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.5059f, 0.8314f, 0.9804f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		
		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		proj = glm::mat4(1.0f);
		
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		    cameraPos += cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		    cameraPos -= cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		    cameraPos += cameraSpeed * cameraUp;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		    cameraPos -= cameraSpeed * cameraUp;
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		    cameraSpeed += 0.001f;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		    cameraSpeed -= 0.001f;
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
			cameraPos		= glm::vec3(0.0f, 0.0f, 0.0f);
			cameraFront		= glm::vec3(0.0f, 0.0f, -1.0f);
			cameraUp		= glm::vec3(0.0f, 1.0f, 0.0f);
			lastX = 400;
			lastY = 400;
			yaw = -90.0f;
			pitch = 0.0f;
		}
		    		

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		proj = glm::perspective(glm::radians(90.0f), (float)(windowWidth/windowHeight), 0.1f, 100.0f);
		
		int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		
		glBindVertexArray(VAOID[0]);
		for (int i = 0; i < 54; i++){
			model = glm::translate(model, cubePositions[i]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			model = glm::mat4(1.0f);
			glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		}
		
		glBindVertexArray(VAOID[1]);
		for (int i = 54; i < 81; i++){
			model = glm::translate(model, cubePositions[i]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			model = glm::mat4(1.0f);
			glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		}
		
		glBindVertexArray(VAOID[2]);
		for (int i = 81; i < 86; i++){
			model = glm::translate(model, cubePositions[i]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			model = glm::mat4(1.0f);
			glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		}
		
		glBindVertexArray(VAOID[3]);
		for (int i = 86; i < 104; i++){
			model = glm::translate(model, cubePositions[i]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			model = glm::mat4(1.0f);
			glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}





const char* getShader (char *filePath, char type)
{
	FILE *filePointer;
	char *shaderGLSL = (char*)malloc(sizeof(char)*100000);
	char *tmp = (char*)malloc(sizeof(char)*100000);
	int write = 0;
	
	strcpy (shaderGLSL, "\0");
	
	filePointer = fopen(filePath,"r");
	
	if(filePointer == NULL) {
		perror("Error in opening file");
		return NULL;
	}
	
	while (fgets (tmp, 100000, filePointer)){
		if (strstr (tmp, "#shader") != NULL && type == 'v'){
			if (strstr (tmp, "vertex") != NULL){
				fgets (tmp, 100000, filePointer);
				write = 1;
			}
			else if (strstr (tmp, "fragment") != NULL)
				break;
		} else if (strstr (tmp, "#shader") != NULL && type == 'f'){
			if (strstr (tmp, "vertex") != NULL){
				continue;
			}
			else if (strstr (tmp, "fragment") != NULL){
				fgets (tmp, 100000, filePointer);
				write = 1;
			} 
		} else if (strstr (tmp, "#END") != NULL)
			break;
		
		if (write)
			strcat (shaderGLSL, tmp);
	}
	fclose (filePointer);
	free (tmp);
	if (!realloc (shaderGLSL, strlen(shaderGLSL)+1))
		return NULL;
	return shaderGLSL;
}

void closeWindow (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.01f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}
