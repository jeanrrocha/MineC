#include <stdio.h>
#include <avl.h>
#include <cstdlib>
#define GLEW_STATIC


#include <glew.h>
#include <glfw3.h>
#include <glut.h>


#include <math.h>
#include <string.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#define FRAGMENT 'f'
#define VERTEX 'v'

#define CUBE 192
//CUBE = sizeof(dirt)

const int windowWidth = 800;
const int windowHeight = 800;
const char* windowTitle = "MineC";

typedef struct {
	vector cameraPos;
	vector cameraFront;
	vector cameraUp;
	vector direction;
} Camera;

typedef struct {
	Camera camera;
} Player;





glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 direction	= glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 proj = glm::mat4(1.0f);

GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

GLfloat lastX = windowWidth/2, lastY = windowHeight/2;
int firstMouse = true;

GLuint VAO, VBO, EBO;
GLuint shaderProgram;
GLuint outlineShaderProgram;

vector blocksInRange[1331] = {(vector){0, 0, 0}};

typedef struct {
	vector origin;
	vector direction;
	vector invDirection;
	int sign[3];
} ray;

block vecToBlock (vector T);
ray initializeRay (vector origin, vector direction);
int intersect (vector position, ray r, float range);
int closestToZero (vector first, vector second);
block firstBlock (node *T, ray r);
int isBlock (node *T, vector pos);
const char* getShader (char *filePath, char type);
void closeWindow (GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void drawAllBlocks (node *blocks, GLuint shader, GLuint VAOID[]);
void drawALL (node *blocks, GLuint shaders[], GLuint VAOID[], ray r);
void fillSkyBlock (node **chunk);


vector addVectors(vector a,vector b){
	return (vector){a.x+b.x,a.y+b.y,a.z+b.z};
}

vector subVectors(vector a,vector b){
	return (vector){a.x-b.x,a.y-b.y,a.z-b.z};
}

double dotProduct(vector a,vector b){
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vector scaleVector(double l,vector a){
	return (vector){l*a.x,l*a.y,l*a.z};
}

vector normalizeVector (vector a){
	float w = sqrt (a.x * a.x + a.y * a.y + a.z * a.z);
	return (vector){a.x/w, a.y/w, a.z/w};
}



void printBal (node *T) {
	if (!T)
		return;
	
	printf ("%d\n", T->bal);
	if (T->bal != -1 && T->bal != 0 && T->bal != 1)
		exit (0);
	printBal (T->left);
	printBal (T->right);
}

int searchElement (node *T, block data) {
	while (T) {
		if (compareBlocks(data, T->data))
			T = T->left;
		else if (compareBlocks(T->data, data))
			T = T->right;
		else
			return 1;
	}
	return 0;
}

block getElement (node *T, block data) {
	while (T) {
		if (compareBlocks(data, T->data))
			T = T->left;
		else if (compareBlocks(T->data, data))
			T = T->right;
		else
			return T->data;
	}
	return (block){-1, -1, -1, -1};
}

int help = 0;
GLuint outlineVAO[1] = {0};
int main(void)
{	
	GLFWwindow* window;
	
	const char *vertexShaderSource = getShader("default.shader", VERTEX);
	//printf ("%s", vertexShaderSource);
	const char *vertexShaderOutline = getShader("outline.shader", VERTEX);
	
	
	const char *fragmentShaderSource = getShader("default.shader", FRAGMENT);
	//printf ("%s", fragmentShaderSource);
	const char *fragmentShaderOutline = getShader("outline.shader", FRAGMENT);
	
	if (!glfwInit()){
		printf ("Failed to initialize GLFW.\n");
		return -1;
	}
	
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
	GLuint outlineVertexShader = glCreateShader(GL_VERTEX_SHADER);
	
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glShaderSource(outlineVertexShader, 1, &vertexShaderOutline, NULL);
	glCompileShader(outlineVertexShader);
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint outlineFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glShaderSource(outlineFragmentShader, 1, &fragmentShaderOutline, NULL);
	glCompileShader(outlineFragmentShader);
	
	shaderProgram = glCreateProgram();
	outlineShaderProgram = glCreateProgram();
	
	glAttachShader (shaderProgram, vertexShader);
	glAttachShader (shaderProgram, fragmentShader);
	
	glAttachShader (outlineShaderProgram, outlineVertexShader);
	glAttachShader (outlineShaderProgram, outlineFragmentShader);
	
	glLinkProgram (shaderProgram);
	glLinkProgram (outlineShaderProgram);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(outlineVertexShader);
	glDeleteShader(outlineFragmentShader);

	GLfloat bedrock[] =
	{    
		-0.5f, -0.5f, -0.5f,	0.f, 0.f, 0.f,
		-0.5f, -0.5f, 0.5f,		0.f, 0.f, 0.f,
		-0.5f, 0.5f, 0.5f,		0.f, 0.f, 0.f,
		0.5f, 0.5f, -0.5f,		0.f, 0.f, 0.f,
		-0.5f, 0.5f, -0.5f,		0.f, 0.f, 0.f,
		0.5f,-0.5f, 0.5f,		0.f, 0.f, 0.f,
		0.5f,-0.5f,-0.5f,		0.f, 0.f, 0.f,
		0.5f, 0.5f, 0.5f,		0.f, 0.f, 0.f
	};
	
	GLfloat dirt[] =
	{    
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
	{ 
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
	{  
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
	{    
		-0.5f, -0.5f, -0.5f,	0.1961f, 0.8039f, 0.1961f,
		-0.5f, -0.5f, 0.5f,		0.1961f, 0.8039f, 0.1961f,
		-0.5f, 0.5f, 0.5f,		0.1961f, 0.8039f, 0.1961f,
		0.5f, 0.5f, -0.5f,		0.1961f, 0.8039f, 0.1961f,
		-0.5f, 0.5f, -0.5f,		0.1961f, 0.8039f, 0.1961f,
		0.5f,-0.5f, 0.5f,		0.1961f, 0.8039f, 0.1961f,
		0.5f,-0.5f,-0.5f,		0.1961f, 0.8039f, 0.1961f,
		0.5f, 0.5f, 0.5f,		0.1961f, 0.8039f, 0.1961f
	};
	
	GLfloat outline[] =
	{
		//FACE SUPERIOR
	0.5, 0.5, 0.5, //0
	-0.5, 0.5, 0.5, //1
	0.5, 0.5, -0.5, //2
	-0.5, 0.5, -0.5, //3
	
	0.49, 0.5, 0.49, //4
	-0.49, 0.5, 0.49, //5
	0.49, 0.5, -0.49, //6
	-0.49, 0.5, -0.49, //7
	
	//A
	0.49, 0.49, 0.5, //8
	0.5, 0.49, 0.49, //9
	-0.49, 0.49, 0.5, //10
	-0.5, 0.49, 0.49, //11
	0.49, 0.49, -0.5, //12
	0.5, 0.49, -0.49, //13
	-0.49, 0.49, -0.5, //14
	-0.5, 0.49, -0.49, //15
	
	0.49, -0.49, 0.5, //16
	0.5, -0.49, 0.49, //17
	-0.49, -0.49, 0.5, //18
	-0.5, -0.49, 0.49, //19
	0.49, -0.49, -0.5, //20
	0.5, -0.49, -0.49, //21
	-0.49, -0.49, -0.5, //22
	-0.5, -0.49, -0.49, //23
	
	0.5, -0.5, 0.5, //24
	-0.5, -0.5, 0.5, //25
	0.5, -0.5, -0.5, //26
	-0.5, -0.5, -0.5, //27
	
	0.49, -0.5, 0.49, //28
	-0.49, -0.5, 0.49, //29
	0.49, -0.5, -0.49, //30
	-0.49, -0.5, -0.49 //31
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
	
	GLuint indicesOutline[] =
	{
	0, 1, 5,
	0, 4, 5,
	0, 2, 4,
	2, 4, 6,
	2, 3, 6,
	3, 6, 7,
	1, 5, 7,
	1, 3, 7,
	
	0, 1, 10,
	0, 8, 10,
	0, 8, 24,
	8, 16, 24,
	16, 24, 25,
	16, 18, 25,
	1, 18, 25,
	1, 10, 18,
	
	0, 2, 9,
	2, 9, 13,
	2, 13, 26,
	13, 21, 26,
	21, 24, 26,
	17, 21, 24,
	0, 17, 24,
	0, 9, 17,
	
	//duvida
	1, 3, 15,
	1, 11, 15,
	1, 11, 25,
	11, 19, 25,
	19, 25, 27,
	19, 23, 27,
	3, 23, 27,
	3, 15, 23,

	//12 14
	
	2, 3, 12,
	3, 12, 14,
	3, 14, 27,
	14, 22, 27,
	20, 22, 26,
	22, 26, 27,
	2, 20, 26,
	2, 12, 20,

	24, 25, 29,
	24, 28, 29,
	24, 26, 28,
	26, 28, 30,
	26, 27, 30,
	27, 30, 31,
	25, 29, 31,
	25, 27, 31
	};
	
	node *chunk = NULL;
	
	fillSkyBlock (&chunk);
	
	glm::vec3 cubePositions[] =
	{
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
	
	GLuint VAOID[5] = {0};
	
	
	for (int i = 0; i < sizeof(VAOID) / sizeof(GLuint); i++)
		glGenVertexArrays(1, &VAOID[i]);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, CUBE * sizeof(VAOID) / sizeof(GLuint), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*0, CUBE, bedrock);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*1, CUBE, dirt);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*2, CUBE, grass);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*3, CUBE, log);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE*4, CUBE, leaves);
	
	GLuint outlineVBO;
	GLuint outlineEBO;
	
	glGenVertexArrays(1, &outlineVAO[0]);
	glGenBuffers(1, &outlineVBO);
	glGenBuffers(1, &outlineEBO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);
	glBufferData(GL_ARRAY_BUFFER, 384, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 384, outline);
	
	glBindVertexArray(outlineVAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outlineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesOutline), indicesOutline, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
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
	
	int pos = 0;
	
	for (int i = -5; i < 6; i++)
		for (int j = -5; j < 6; j++)
			for (int k = -5; k < 6; k++) {
				blocksInRange[pos].x = i;
				blocksInRange[pos].y = j;
				blocksInRange[pos++].z = k;
			}
	
	for (int i = 0; i < 1331; i++)
		for (int j = 0; j < 1331; j++)
			if (closestToZero(blocksInRange[i], blocksInRange[j])) {
				vector aux = blocksInRange[i];
				blocksInRange[i] = blocksInRange[j];
				blocksInRange[j] = aux;
			}
	
	
	
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetKeyCallback(window, closeWindow);
	
	float cameraSpeed = 0.01f;
	Player player;
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.5059f, 0.8314f, 0.9804f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		
		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		proj = glm::mat4(1.0f);
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		    cameraSpeed = 0.1f;
		
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			glm::vec3 aux = cameraFront;
			aux.y = 0;
			aux = glm::normalize(aux);
			
			int dirX = 0, dirZ = 0;
			
			int a = 1;
			
			if (aux.x > 0)
				dirX = a;
			else if (aux.x < 0)
				dirX = -a;
			
			if (aux.z > 0)
				dirZ = a;
			else if (aux.z < 0)
				dirZ = -a;
			
			aux *= cameraSpeed;
			
			vector sumDirectionX = (vector){aux.x, aux.y, 0};
			vector sumDirectionZ = (vector){0, aux.y, aux.z};
			vector aux2 = (vector){cameraPos.x, cameraPos.y, cameraPos.z};
			
			//if (isBlock (chunk, addVectors(aux2, sumDirectionX)))
			//	aux.x = 0;
			
			//if (isBlock (chunk, addVectors(aux2, sumDirectionZ)))
			//	aux.z = 0;
			
			if (isBlock (chunk, (vector){cameraPos.x, cameraPos.y-0.5, cameraPos.z}) ||
				isBlock (chunk, (vector){cameraPos.x, cameraPos.y-1.5, cameraPos.z}))
				aux.x = 0;
			
			if (isBlock (chunk, (vector){cameraPos.x, cameraPos.y-0.5, cameraPos.z}) ||
				isBlock (chunk, (vector){cameraPos.x, cameraPos.y-1.5, cameraPos.z}))
				aux.z = 0;
			
			cameraPos += aux;
			
			player.camera.cameraPos = addVectors (player.camera.cameraPos, scaleVector(cameraSpeed, normalizeVector (player.camera.cameraFront)));
			
		}
		
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			glm::vec3 aux = cameraFront;
			aux.y = 0;
			aux = glm::normalize(aux);
			
			cameraPos -= cameraSpeed * aux;
			
			player.camera.cameraPos = subVectors (player.camera.cameraPos, scaleVector(cameraSpeed, normalizeVector (player.camera.cameraFront)));
		}
		
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		    cameraPos += cameraSpeed * cameraUp;
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		    if (!isBlock (chunk, (vector){cameraPos.x, cameraPos.y-1.81, cameraPos.z}))
				cameraPos -= cameraSpeed * cameraUp;
		}
		
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		    cameraSpeed += 0.001f;
		
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		    cameraSpeed -= 0.001f;
		
		ray r = initializeRay ((point3D){cameraPos.x, cameraPos.y, cameraPos.z}, (point3D){cameraFront.x, cameraFront.y, cameraFront.z});
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && help == 0) {
		    block aux = firstBlock(chunk, r);
		    if (aux.id != -1)
		    	removeBlock (&chunk, aux);
			help = 20;
		}
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && help == 0) {
		    
			vector up = {0, 1, 0};
			vector down = {0, -1, 0};
			vector right = {1, 0, 0};
			vector left = {-1, 0, 0};
			vector front = {0, 0, 1};
			vector back = {0, 0, -1};
			
			block bAux = firstBlock(chunk, r);
		    
			vector aux = {bAux.x, bAux.y, bAux.z};
			
			block posBlock = vecToBlock (aux);
			
			
			if (!isBlock (chunk, addVectors (aux, up)) && intersect(addVectors (aux, up), r, 5) && cameraPos.y > aux.y+0.5) {
				
				posBlock = vecToBlock (addVectors (aux, up));
				posBlock.id = 1;
				
			} else if (!isBlock (chunk, addVectors (aux, down)) && intersect(addVectors (aux, down), r, 5) && cameraPos.y < aux.y-0.5) {
				
				posBlock = vecToBlock (addVectors (aux, down));
				posBlock.id = 1;
				
			} else if (!isBlock (chunk, addVectors (aux, right)) &&  intersect(addVectors (aux, right), r, 5) && cameraPos.x > aux.x+0.5) {
				
				posBlock = vecToBlock (addVectors (aux, right));
				posBlock.id = 1;
				
			} else if (!isBlock (chunk, addVectors (aux, left)) &&  intersect(addVectors (aux, left), r, 5) && cameraPos.x < aux.x-0.5) {
				
				posBlock = vecToBlock (addVectors (aux, left));
				posBlock.id = 1;
			 
			} else if (!isBlock (chunk, addVectors (aux, front)) &&  intersect(addVectors (aux, front), r, 5) && cameraPos.z > aux.z+0.5) {
				
				posBlock = vecToBlock (addVectors (aux, front));
				posBlock.id = 1;
			
			} else if (!isBlock (chunk, addVectors (aux, back)) &&  intersect(addVectors (aux, back), r, 5) && cameraPos.z < aux.z-0.5) {
				
				posBlock = vecToBlock (addVectors (aux, back));
				posBlock.id = 1;
				
			}
			
			if (posBlock.id != -1) {
				insertBlock (&chunk, posBlock);
				help = 20;
			}
		}
		
		if (help > 0)
			help--;
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
			cameraPos		= glm::vec3(0.0f, 3.5f, 0.0f);
			cameraFront		= glm::vec3(0.0f, 0.0f, -1.0f);
			cameraUp		= glm::vec3(0.0f, 1.0f, 0.0f);
			player.camera.cameraPos = (vector){0.0f, 3.5f, 0.0f};
			player.camera.cameraFront = (vector){0.0f, 0.0f, -1.0f};
			player.camera.cameraUp = (vector){0.0f, 1.0f, 0.0f};
			lastX = 400;
			lastY = 400;
			yaw = -90.0f;
			pitch = 0.0f;
			clearChunk (&chunk);
			fillSkyBlock (&chunk);
		}
		cameraSpeed = 0.05f;
		    		

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		proj = glm::perspective(glm::radians(90.0f), (float)(windowWidth/windowHeight), 0.2f, 100.0f);
		
		GLuint shaders[] = {shaderProgram, outlineShaderProgram};
		
		drawALL (chunk, shaders, VAOID, r);

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

ray initializeRay (vector origin, vector direction) {
	ray aux;
	aux.origin = origin;
	aux.direction = direction;
	aux.invDirection = (vector){1/direction.x, 1/direction.y, 1/direction.z};
	aux.sign[0] = aux.invDirection.x < 0;
	aux.sign[1] = aux.invDirection.y < 0;
	aux.sign[2] = aux.invDirection.z < 0;
	return aux;
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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.9f)
        pitch = 89.9f;
    if(pitch < -89.9f)
        pitch = -89.9f;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

int closestToZero (vector first, vector second) {
	
	int aux = abs(first.x) + abs(first.y) + abs(first.z);
	int aux2 = abs(second.x) + abs(second.y) + abs(second.z);
	
	return abs(aux) < abs(aux2);
}

block firstBlock (node *T, ray r) {
	vector base = (vector){round(r.origin.x), round(r.origin.y), round(r.origin.z)};
	vector aux = base;
	block b;

	for (int i = 0; i < 1331; i++, aux = base) {
		aux = addVectors(aux, blocksInRange[i]);
		b = getElement (T, vecToBlock(aux));
		
		if (intersect(aux, r, 5) && b.id != -1) {
			return b;
		}
	}
	
	return (block){-1, -1, -1, -1};
}

void drawBlockOutline (block T, GLuint shader, GLuint VAOID[]);

void drawALL (node *blocks, GLuint *shaders, GLuint *VAOID, ray r) {
	
	int viewLoc = glGetUniformLocation(shaders[0], "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projLoc = glGetUniformLocation(shaders[0], "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
	
	drawAllBlocks (blocks, shaders[0], VAOID);
	
	block first = firstBlock (blocks, r);
	if (first.id != -1)
		drawBlockOutline (first, shaders[1], VAOID);
	
	
}

void drawBlockOutline (block T, GLuint shader, GLuint VAOID[]) {
	int modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	model = glm::mat4 (1.0f);
	model = glm::translate (model, glm::vec3 (T.x, T.y, T.z));
	
	glBindVertexArray(outlineVAO[0]);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	glClear(GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 144, GL_UNSIGNED_INT, 0);
	//glEnable(GL_DEPTH_TEST);
}

void drawAllBlocks (node *blocks, GLuint shader, GLuint VAOID[]){
	if (!blocks)
		return;
	
	int modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	model = glm::mat4 (1.0f);
	model = glm::translate (model, glm::vec3 (blocks->data.x, blocks->data.y, blocks->data.z));
	
	glBindVertexArray(VAOID[blocks->data.id]);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	glDrawElements(GL_TRIANGLES, sizeof(GLuint)*36/sizeof(int), GL_UNSIGNED_INT, 0);
	
	drawAllBlocks (blocks->left, shader, VAOID);
	drawAllBlocks (blocks->right, shader, VAOID);
}

int intersect (vector position, ray r, float range) {
	
	vector bounds[2] = {addVectors(position, (vector){-0.5, -0.5, -0.5}), addVectors(position, (vector){0.5, 0.5, 0.5})};
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	
	tmin = (bounds[r.sign[0]].x - r.origin.x) * r.invDirection.x;
	tmax = (bounds[1-r.sign[0]].x - r.origin.x) * r.invDirection.x;
	
	tymin = (bounds[r.sign[1]].y - r.origin.y) * r.invDirection.y;
	tymax = (bounds[1-r.sign[1]].y - r.origin.y) * r.invDirection.y;
	
	if ((tmin > tymax) || (tymin > tmax))
		return 0;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	
	tzmin = (bounds[r.sign[2]].z - r.origin.z) * r.invDirection.z;
	tzmax = (bounds[1-r.sign[2]].z - r.origin.z) * r.invDirection.z;
	
	if ( (tmin > tzmax) || (tzmin > tmax) )
		return 0;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return ((tmin < range) && (tmax > 0.0));
}

block vecToBlock (vector T) {
	return (block){round(T.x), round(T.y), round(T.z), -1};
}

int isBlock (node *T, vector pos) {
	if (getElement(T, vecToBlock(pos)).id == -1)
		return 0;
	return 1;
}

void fillSkyBlock (node **chunk) {
	insertBlock (chunk, (block){0, 0, 0, 1});
	insertBlock (chunk, (block){1, 0, 0, 1});
	insertBlock (chunk, (block){-1, 0, 0, 1});
	insertBlock (chunk, (block){0, 0, 1, 1});
	insertBlock (chunk, (block){1, 0, 1, 1});
	insertBlock (chunk, (block){-1, 0, 1, 1});
	insertBlock (chunk, (block){0, 0, -1, 1});
	insertBlock (chunk, (block){1, 0, -1, 1});
	insertBlock (chunk, (block){-1, 0, -1, 1});
	
	insertBlock (chunk, (block){-3, 0, 0, 1});
	insertBlock (chunk, (block){-2, 0, 0, 1});
	insertBlock (chunk, (block){-4, 0, 0, 1});
	insertBlock (chunk, (block){-3, 0, 1, 1});
	insertBlock (chunk, (block){-2, 0, 1, 1});
	insertBlock (chunk, (block){-4, 0, 1, 1});
	insertBlock (chunk, (block){-3, 0, -1, 1});
	insertBlock (chunk, (block){-2, 0, -1, 1});
	insertBlock (chunk, (block){-4, 0, -1, 1});
	
	insertBlock (chunk, (block){-3, 0, 3, 1});
	insertBlock (chunk, (block){-2, 0, 3, 1});
	insertBlock (chunk, (block){-4, 0, 3, 1});
	insertBlock (chunk, (block){-3, 0, 4, 1});
	insertBlock (chunk, (block){-2, 0, 4, 1});
	insertBlock (chunk, (block){-4, 0, 4, 1});
	insertBlock (chunk, (block){-3, 0, 2, 1});
	insertBlock (chunk, (block){-2, 0, 2, 1});
	insertBlock (chunk, (block){-4, 0, 2, 1});
	
	insertBlock (chunk, (block){0, -1, 0, 1});
	insertBlock (chunk, (block){1, -1, 0, 1});
	insertBlock (chunk, (block){-1, -1, 0, 1});
	insertBlock (chunk, (block){0, -1, 1, 1});
	insertBlock (chunk, (block){1, -1, 1, 1});
	insertBlock (chunk, (block){-1, -1, 1, 1});
	insertBlock (chunk, (block){0, -1, -1, 1});
	insertBlock (chunk, (block){1, -1, -1, 1});
	insertBlock (chunk, (block){-1, -1, -1, 1});
	
	insertBlock (chunk, (block){-3, -1, 0, 1});
	insertBlock (chunk, (block){-2, -1, 0, 1});
	insertBlock (chunk, (block){-4, -1, 0, 1});
	insertBlock (chunk, (block){-3, -1, 1, 1});
	insertBlock (chunk, (block){-2, -1, 1, 1});
	insertBlock (chunk, (block){-4, -1, 1, 1});
	insertBlock (chunk, (block){-3, -1, -1, 1});
	insertBlock (chunk, (block){-2, -1, -1, 1});
	insertBlock (chunk, (block){-4, -1, -1, 1});
	
	insertBlock (chunk, (block){-3, -1, 3, 1});
	insertBlock (chunk, (block){-2, -1, 3, 1});
	insertBlock (chunk, (block){-4, -1, 3, 1});
	insertBlock (chunk, (block){-3, -1, 4, 1});
	insertBlock (chunk, (block){-2, -1, 4, 1});
	insertBlock (chunk, (block){-4, -1, 4, 1});
	insertBlock (chunk, (block){-3, -1, 2, 1});
	insertBlock (chunk, (block){-2, -1, 2, 1});
	insertBlock (chunk, (block){-4, -1, 2, 1});
	
	insertBlock (chunk, (block){0, 1, 0, 2});
	insertBlock (chunk, (block){1, 1, 0, 2});
	insertBlock (chunk, (block){-1, 1, 0, 2});
	insertBlock (chunk, (block){0, 1, 1, 2});
	insertBlock (chunk, (block){1, 1, 1, 2});
	insertBlock (chunk, (block){-1, 1, 1, 2});
	insertBlock (chunk, (block){0, 1, -1, 2});
	insertBlock (chunk, (block){1, 1, -1, 2});
	insertBlock (chunk, (block){-1, 1, -1, 2});
	
	insertBlock (chunk, (block){-3, 1, 0, 2});
	insertBlock (chunk, (block){-2, 1, 0, 2});
	insertBlock (chunk, (block){-4, 1, 0, 2});
	insertBlock (chunk, (block){-3, 1, 1, 2});
	insertBlock (chunk, (block){-2, 1, 1, 2});
	insertBlock (chunk, (block){-4, 1, 1, 2});
	insertBlock (chunk, (block){-3, 1, -1, 2});
	insertBlock (chunk, (block){-2, 1, -1, 2});
	insertBlock (chunk, (block){-4, 1, -1, 2});
	
	insertBlock (chunk, (block){-3, 1, 3, 2});
	insertBlock (chunk, (block){-2, 1, 3, 2});
	insertBlock (chunk, (block){-4, 1, 3, 2});
	insertBlock (chunk, (block){-3, 1, 4, 2});
	insertBlock (chunk, (block){-2, 1, 4, 2});
	insertBlock (chunk, (block){-4, 1, 4, 2});
	insertBlock (chunk, (block){-3, 1, 2, 2});
	insertBlock (chunk, (block){-2, 1, 2, 2});
	insertBlock (chunk, (block){-4, 1, 2, 2});
}