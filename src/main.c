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

const char* getVertexShader (char *filePath, char type){
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

//int tam = 0;
float rotation = 0.0f;

void closeWindow(GLFWwindow *window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		printf ("You pressed ESC\n");
	//glfwTerminate();
}

typedef struct {
	float position[3];
	float color[4];
} Vertex;

void fullscrean (GLFWwindow *window, int key, int scancode, int action, int mods){
	/*if (key == GLFW_KEY_F && action == GLFW_PRESS && tam == 0){
		//glfwWindowHint (GLFW_MAXIMIZED, GLFW_TRUE);
		//glfwWindowHint (GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
		glfwSetWindowSize(window, 1920, 1080);
		tam = 1;
	} else if (key == GLFW_KEY_F && action == GLFW_PRESS && tam == 1){
		glfwSetWindowSize(window, 960, 960);
		tam = 0;
	}*/
}

GLfloat directionX = 1.0f;
GLfloat directionY = 1.0f;

int main(void)
{	
	GLFWwindow* window;
	
	const char *vertexShaderSource = getVertexShader("default.shader", VERTEX);
	//printf ("%s", vertexShaderSource);
	
	const char *fragmentShaderSource = getVertexShader("default.shader", FRAGMENT);
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
	
	GLuint shaderProgram = glCreateProgram();
	
	glAttachShader (shaderProgram, vertexShader);
	glAttachShader (shaderProgram, fragmentShader);
	
	glLinkProgram (shaderProgram);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	
	glfwSetKeyCallback(window, closeWindow);
	glfwSetKeyCallback(window, fullscrean);
	
/*	GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      
	-0.5f, 0.0f,  0.5f,     0.00f, 1.00f, 0.00f,
	-0.5f, 0.0f, -0.5f,     1.00f, 0.00f, 0.00f,
	 0.5f, 0.0f, -0.5f,     0.00f, 1.00f, 0.00f,
	 0.5f, 0.0f,  0.5f,     1.00f, 0.00f, 0.00f,
	 0.0f, 0.8f,  0.0f,     0.00f, 0.00f, 1.00f
};*/

	GLfloat vertices[] =
	{ //     COORDINATES     /        COLORS      
		-0.5f, -0.5f, -0.5f,	1.00f, 0.00f, 0.00f,
		-0.5f, -0.5f, 0.5f,		0.00f, 1.00f, 0.00f,
		-0.5f, 0.5f, 0.5f,		0.00f, 0.00f, 1.00f,
		0.5f, 0.5f, -0.5f,		1.00f, 0.00f, 0.00f,
		-0.5f, 0.5f, -0.5f,		0.00f, 1.00f, 0.00f,
		0.5f,-0.5f, 0.5f,		0.00f, 0.00f, 1.00f,
		0.5f,-0.5f,-0.5f,		1.00f, 0.00f, 0.00f,
		0.5f, 0.5f, 0.5f,		0.00f, 1.00f, 0.00f,
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






// Indices for vertices order
/*GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};*/

	GLuint VAO, VBO, EBO;
	
	//Gerando o VertexArry, VertexBuffer e o IndexBuffer.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	
	//Bindando o VAO, precisa ser feito antes de trabalhar com o VBO e EBO.
	glBindVertexArray(VAO);
	
	//Bindando o VBO e passando as informações pro buffer.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//Bindando o EBO e passando as informações pro buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	//Passando a localização de onde as informações  devem ser retiradas do buffer.
	//1 - location, 2 - size, 3 - type, 4 - ???, 5 - numero de elementos por vertex, 6 - onde comeca os elementos dessa location.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	
	double prevTime = glfwGetTime();
	
	glEnable(GL_DEPTH_TEST);
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		
		
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1/60){
			rotation += 0.2f;
			prevTime = crntTime;
		}
		
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(directionX, directionY, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		proj = glm::perspective(glm::radians(60.0f), (float)(windowWidth/windowHeight), 0.1f, 100.0f);
		
		int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		
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