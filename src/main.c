#include <stdio.h>
#include <avl.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#include "mat.h"
#include "stb_image.h"

#define GLBIND_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <glbind.h>
#include <GLFW/glfw3.h>

#define FRAGMENT 'f'
#define VERTEX 'v'

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

#ifndef M_PI
#    define M_PI 3.14159265358979323846264338327950288
#endif

#define PLAYER_WALKING_SPEED 4.317f


#define CUBE 96

const int windowWidth = 1920;
const int windowHeight = 1080;
//const int windowWidth = 800;
//const int windowHeight = 800;
const char* windowTitle = "MineC";

typedef enum {
	NONE = 0,
	Y0X0,
	Y0X1,
	Y0Z0,
	Y1Z1,
	Y1X0,
	Y1X1,
	Y1Z0,
	Y0Z1,
	X0Y0,
	X0Y1,
	X1Y0,
	X1Y1,
	Z0Y0,
	Z0Y1,
	Z1Y0,
	Z1Y1
} collisionType;

typedef struct {
	vec3 cameraPos;
	vec3 cameraFront;
	vec3 cameraUp;
	vec3 direction;
	mat4 view;
	mat4 model;
	mat4 proj;
} Camera;

typedef struct {
	vec3 vertex[8];
} Box;

typedef struct {
	Camera camera;
	Box collisionBox;
	int currentBlock;
	float HSpeed;
	float VSpeed;
	vec3 acceleration;
	vec3 motion;
	bool onGround;
	bool isFlying;
	bool isSprinting;
	bool isMoving;
	bool isJumping;
	float jumpTimer;
	float flyTimer;
} Player;

Player player;

GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

GLfloat lastX = windowWidth/2, lastY = windowHeight/2;
int firstMouse = true;

GLuint VAO, VBO, EBO;
GLuint shaderProgram;
GLuint outlineShaderProgram;

vec3 blocksInRange[1331] = {(vec3){0, 0, 0}};

typedef struct {
	vec3 origin;
	vec3 direction;
	vec3 invDirection;
	int sign[3];
} ray;

block vecToBlock (vec3 T);
ray initializeRay (vec3 origin, vec3 direction);
int intersect (vec3 position, ray r, float range);
int closestToZero (vec3 first, vec3 second);
block firstBlock (node *T, ray r);
int isBlock (node *T, vec3 pos);
const char* getShader (char *filePath, char type);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void drawAllBlocks (node *blocks, GLuint shader, GLuint VAOID[]);
void drawALL (node *blocks, GLuint shaders[], GLuint VAOID[], ray r);
void fillSkyBlock (node **chunk);
void saveWorld (node *chunk, FILE *p);
void loadWorld (node **chunk, FILE *p);



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

float help = 0;
GLuint outlineVAO[1] = {0};

float deltaTime = 0.f;

void processMovement (GLFWwindow* window, Player *player, float deltaTime, node *chunk);


int main(void)
{	
	
	player.camera.cameraPos = (vec3){0.5, 3.82, 0.5};
	player.camera.cameraFront = (vec3){0.0, 0.0, -1.0};
	player.camera.cameraUp = (vec3){0.0, 1.0, 0.0};
	player.camera.direction = (vec3){0.0, 0.0, 0.0};
	player.camera.view = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.proj = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	
	player.HSpeed = 4.317f;
	player.VSpeed = 0.f;
	player.onGround = true;
	player.isFlying = false;
	
	player.collisionBox.vertex[0] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, -0.3});
	player.collisionBox.vertex[1] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, -0.3});
	player.collisionBox.vertex[2] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, 0.3});
	player.collisionBox.vertex[3] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, 0.3});
	player.collisionBox.vertex[4] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, -0.3});
	player.collisionBox.vertex[5] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, -0.3});
	player.collisionBox.vertex[6] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, 0.3});
	player.collisionBox.vertex[7] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, 0.3});
	
	
	
	GLenum result = glbInit(NULL, NULL);
	
	/****************************************************/
	/* Inicialianzo o GLFW e a janela */
	
	GLFWwindow* window;
	
	if (!glfwInit()){
		printf ("Failed to initialize GLFW.\n");
		return -1;
	}
	
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, glfwGetPrimaryMonitor(), NULL);
	if (!window){
		printf ("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glViewport(0, 0, windowWidth, windowHeight);
	
	/* FIM */
	/****************************************************/
	
	
	
	/****************************************************/
	/* Inicializando as Shaders */
	
	const char *vertexShaderSource = getShader("./resources/shaders/default.shader", VERTEX);
	const char *vertexShaderOutline = getShader("./resources/shaders/outline.shader", VERTEX);
	const char *fragmentShaderSource = getShader("./resources/shaders/default.shader", FRAGMENT);
	const char *fragmentShaderOutline = getShader("./resources/shaders/outline.shader", FRAGMENT);
	
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
	
	/* FIM */
	/****************************************************/
	
	//glDeleteProgram(shaderProgram);
	//glDeleteProgram(outlineShaderProgram);
	
	GLfloat defaultBlock[] =
	{    
		0.f, 1.f, 1.f, 0.0f, 0.0f,
		0.f, 0.f, 1.f, 0.0f, 0.5f,
		1.f, 0.f, 1.f, 0.5f, 0.5f,
		1.f, 1.f, 1.f, 0.5f, 0.0f,
		0.f, 1.f, 0.f, 0.0f, 0.0f,
		1.f, 1.f, 0.f, 0.5f, 0.0f,
		0.f, 0.f, 0.f, 0.0f, 0.5f,
		1.f, 0.f, 0.f, 0.5f, 0.5f,

		0.f, 1.f, 0.f, 0.0f, 0.5f,
		1.f, 1.f, 0.f, 0.5f, 0.5f,
		0.f, 1.f, 1.f, 0.0f, 1.0f,
		1.f, 1.f, 1.f, 0.5f, 1.0f,

		1.f, 1.f, 1.f, 0.0f, 0.0f,
		1.f, 0.f, 1.f, 0.0f, 0.5f,

		0.f, 1.f, 1.f, 0.5f, 0.0f,
		0.f, 0.f, 1.f, 0.5f, 0.5f,

		0.f, 0.f, 0.f, 0.5f, 0.0f,
		1.f, 0.f, 0.f, 1.0f, 0.0f,
		0.f, 0.f, 1.f, 0.5f, 0.5f,
		1.f, 0.f, 1.f, 1.0f, 0.5f
	};

	GLuint indices[] =
	{
		0, 1, 3, 3, 1, 2,
		8, 10, 11, 9, 8, 11,
		12, 13, 7, 5, 12, 7,
		14, 15, 6, 4, 14, 6,
		16, 18, 19, 17, 16, 19,
		4, 6, 7, 5, 4, 7
	};
	
	
	
	GLfloat outline[] =
	{
		//FACE SUPERIOR
	1.01, 1.01, 1.01, //0
	-0.01, 1.01, 1.01, //1
	1.01, 1.01, -0.01, //2
	-0.01, 1.01, -0.01, //3
	
	0.99, 1.01, 0.99, //4
	0.01, 1.01, 0.99, //5
	0.99, 1.01, 0.01, //6
	0.01, 1.01, 0.01, //7
	
	//A
	0.99, 0.99, 1.01, //8
	1.01, 0.99, 0.99, //9
	0.01, 0.99, 1.01, //10
	-0.01, 0.99, 0.99, //11
	0.99, 0.99, -0.01, //12
	1.01, 0.99, 0.01, //13
	0.01, 0.99, -0.01, //14
	-0.01, 0.99, 0.01, //15
	
	0.99, 0.01, 1.01, //16
	1.01, 0.01, 0.99, //17
	0.01, 0.01, 1.01, //18
	-0.01, 0.01, 0.99, //19
	0.99, 0.01, -0.01, //20
	1.01, 0.01, 0.01, //21
	0.01, 0.01, -0.01, //22
	-0.01, 0.01, 0.01, //23
	
	1.01, -0.01, 1.01, //24
	-0.01, -0.01, 1.01, //25
	1.01, -0.01, -0.01, //26
	-0.01, -0.01, -0.01, //27
	
	0.99, -0.01, 0.99, //28
	0.01, -0.01, 0.99, //29
	0.99, -0.01, 0.01, //30
	0.01, -0.01, 0.01 //31
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
	
	1, 3, 15,
	1, 11, 15,
	1, 11, 25,
	11, 19, 25,
	19, 25, 27,
	19, 23, 27,
	3, 23, 27,
	3, 15, 23,

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
	
	//fillSkyBlock (&chunk);
    FILE *p = fopen("world.map", "r");
    if (!p)
        return -1;
    
    loadWorld (&chunk, p);
    fclose (p);
	
	GLuint VAOID[1] = {0};
	
	glGenVertexArrays(1, &VAOID[0]);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(defaultBlock), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(defaultBlock), defaultBlock);
	
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
	
	for (int i = 0; i < sizeof(VAOID) / sizeof(GLuint); i++){
		glBindVertexArray(VAOID[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(CUBE*i));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(CUBE*i + 3*sizeof(float)));
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
	
	for (int i = 0; i < 11 * 11 * 11; i++)
		for (int j = 0; j < 11 * 11 * 11; j++)
			if (closestToZero(blocksInRange[i], blocksInRange[j])) {
				vec3 aux = blocksInRange[i];
				blocksInRange[i] = blocksInRange[j];
				blocksInRange[j] = aux;
			}
	
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	
	glfwSetCursorPosCallback(window, mouseCallback);
	
	float cameraSpeed = 4.317f;
	
	int widthImg, heightImg, numColCh;

	
	
	char blockPaths[][100] = {	"dirt.png", 
								"grass.png", 
								"bricks.png", 
								"oak_log.png", 
								"leaves_oak.png"};
	char base[] = "./resources/textures/";
	
	GLuint texture1;
	for (int i = 0; i < 5; i++) {
		
		char block[125];
		strcpy (block, base);
		strcat (block, blockPaths[i]);
		
		glGenTextures (1, &texture1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		unsigned char* textureBytes = stbi_load (block, &widthImg, &heightImg, &numColCh, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(textureBytes);
	}
	
	GLuint tex0Uni = glGetUniformLocation(shaderProgram, "tex0");
	
	
	float currentFrame = 0.f;
	float lastFrame = 0.f;
	float FPS = 120.f;
	
	player.currentBlock = 0;
	
	while (!glfwWindowShouldClose(window))
	{

		do {
			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
		} while (deltaTime < 1.0 / FPS);
		lastFrame = currentFrame;

		glClearColor(0.5059f, 0.8314f, 0.9804f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		player.camera.view = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
		player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
		player.camera.proj = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
		
		processMovement (window, &player, deltaTime, chunk);
		
		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
			player.currentBlock = 0;
		
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			player.currentBlock = 1;
		
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			player.currentBlock = 2;
		
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			player.currentBlock = 3;
		
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
			player.currentBlock = 4;
		
		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
			player.currentBlock = 5;
		
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		    cameraSpeed += 0.001f;
		
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		    cameraSpeed -= 0.001f;
		
		ray r = initializeRay (player.camera.cameraPos, player.camera.cameraFront);
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && help <= 0) {
		    block aux = firstBlock(chunk, r);
		    if (aux.id != -1)
		    	removeBlock (&chunk, aux);
			help = 0.2f;
		}
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && help <= 0) {
		    
			vec3 up = {0, 1, 0};
			vec3 down = {0, -1, 0};
			vec3 right = {1, 0, 0};
			vec3 left = {-1, 0, 0};
			vec3 front = {0, 0, 1};
			vec3 back = {0, 0, -1};
			
			block bAux = firstBlock(chunk, r);
		    
			vec3 aux = {bAux.x, bAux.y, bAux.z};
			block posBlock = vecToBlock (aux);
			
			if (!isBlock (chunk, sum_vec3 (aux, up)) && intersect(sum_vec3 (aux, up), r, 5) && player.camera.cameraPos.y > aux.y+0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, up));
				posBlock.id = player.currentBlock;
				
			} else if (!isBlock (chunk, sum_vec3 (aux, down)) && intersect(sum_vec3 (aux, down), r, 5) && player.camera.cameraPos.y < aux.y-0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, down));
				posBlock.id = player.currentBlock;
				
			} else if (!isBlock (chunk, sum_vec3 (aux, right)) &&  intersect(sum_vec3 (aux, right), r, 5) && player.camera.cameraPos.x > aux.x+0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, right));
				posBlock.id = player.currentBlock;
				
			} else if (!isBlock (chunk, sum_vec3 (aux, left)) &&  intersect(sum_vec3 (aux, left), r, 5) && player.camera.cameraPos.x < aux.x-0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, left));
				posBlock.id = player.currentBlock;
			 
			} else if (!isBlock (chunk, sum_vec3 (aux, front)) &&  intersect(sum_vec3 (aux, front), r, 5) && player.camera.cameraPos.z > aux.z+0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, front));
				posBlock.id = player.currentBlock;
			
			} else if (!isBlock (chunk, sum_vec3 (aux, back)) &&  intersect(sum_vec3 (aux, back), r, 5) && player.camera.cameraPos.z < aux.z-0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, back));
				posBlock.id = player.currentBlock;
				
			}
			
			
			
			if (posBlock.id != -1) {
				insertBlock (&chunk, posBlock);
				help = 0.2f;
			}
		}
		
		if (help > 0)
			help -= deltaTime;
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
			player.camera.cameraPos = (vec3){0.5, 3.82, 0.5};
			player.camera.cameraFront = (vec3){0.0, 0.0, -1.0};
			player.camera.cameraUp = (vec3){0.0, 1.0, 0.0};
			player.camera.direction = (vec3){0.0, 0.0, 0.0};
			player.camera.view = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
			player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
			player.camera.proj = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
			
			player.HSpeed = 4.317f;
			player.VSpeed = 0.f;
			player.onGround = true;
			player.isFlying = false;
			
			player.collisionBox.vertex[0] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, -0.3});
			player.collisionBox.vertex[1] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, -0.3});
			player.collisionBox.vertex[2] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, 0.3});
			player.collisionBox.vertex[3] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, 0.3});
			player.collisionBox.vertex[4] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, -0.3});
			player.collisionBox.vertex[5] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, -0.3});
			player.collisionBox.vertex[6] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, 0.3});
			player.collisionBox.vertex[7] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, 0.3});
			lastX = 400;
			lastY = 400;
			yaw = -90.0f;
			pitch = 0.0f;
			clearChunk (&chunk);
			fillSkyBlock (&chunk);
		}
		cameraSpeed = 4.317f;
		
		player.camera.view = lookAt_mat4 (player.camera.cameraPos, sum_vec3 (player.camera.cameraPos, player.camera.cameraFront), player.camera.cameraUp);
		
		player.camera.proj = perspective_mat4 ((float)degToRad(90.f), (float)windowWidth/windowHeight, 0.1f, 1600.0f);
		
		GLuint shaders[] = {shaderProgram, outlineShaderProgram};
		
		drawALL (chunk, shaders, &VAOID[0], r);
		
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            
            FILE *p = fopen("world.map", "w");
            if (!p)
                return -1;
            
            saveWorld (chunk, p);
            fclose (p);
            
            glfwSetWindowShouldClose(window, 1);
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

void drawBlockOutline (block T, GLuint shader, GLuint VAOID[]);
void drawBlocks (node *blocks, GLuint shader, GLuint VAOID[]);

void drawALL (node *blocks, GLuint *shaders, GLuint *VAOID, ray r) {
	
	block first = firstBlock (blocks, r);
	if (first.id != -1)
		drawBlockOutline (first, shaders[1], VAOID);
	
	drawBlocks (blocks, shaders[0], VAOID);

}

void drawBlockOutline (block T, GLuint shader, GLuint VAOID[]) {
	
	glUseProgram(shader);
	
	int viewLoc = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &player.camera.view.ww);
	
	int projLoc = glGetUniformLocation(shader, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &player.camera.proj.ww);
	
	
	player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.model = translate_mat4_vec3 (player.camera.model, (vec3) {T.x, T.y, T.z});
	
	
	int modelLoc = glGetUniformLocation(shader, "model");
	glBindVertexArray(outlineVAO[0]);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &player.camera.model.ww);
	
	//glClear(GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 144, GL_UNSIGNED_INT, 0);
}

void drawAllBlocks (node *blocks, GLuint shader, GLuint VAOID[]){
	if (!blocks)
		return;
	
	player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.model = translate_mat4_vec3 (player.camera.model, (vec3) {blocks->data.x, blocks->data.y, blocks->data.z});
	
	int modelLoc = glGetUniformLocation(shader, "model");
	glBindVertexArray(VAOID[0]);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &player.camera.model.ww);
	
	glBindTexture(GL_TEXTURE_2D, blocks->data.id);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	
	drawAllBlocks (blocks->left, shader, VAOID);
	drawAllBlocks (blocks->right, shader, VAOID);
}

void drawBlocks (node *blocks, GLuint shader, GLuint VAOID[]){
	glUseProgram(shader);
	
	int viewLoc = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &player.camera.view.ww);
	int projLoc = glGetUniformLocation(shader, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &player.camera.proj.ww);
	
	drawAllBlocks (blocks, shader, VAOID);
}



collisionType detectCollisionPlayer (GLFWwindow* window, Player *player, vec3 movement, float deltaTime, float speed, node *chunk);

float jumpSpeed = 8.5;

bool isPressedSpace = false;


void processMovement (GLFWwindow* window, Player *player, float deltaTime, node *chunk) {
	
	float a = 8.5;
	
	if (player->flyTimer > 0)
		player->flyTimer -= deltaTime;
	
	
	if (player->isFlying) {
		
		player->VSpeed = 0;
		
		if (!player->onGround) {
			
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isPressedSpace) {
				isPressedSpace = true;
				player->VSpeed = PLAYER_WALKING_SPEED;
				
				if (player->flyTimer <= 0)
					player->flyTimer = 0.2;
				else if (player->flyTimer > 0) {
					player->isFlying = false;
					player->flyTimer = 0;
				}

			} else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && isPressedSpace)
				isPressedSpace = false;
			else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
				player->VSpeed = PLAYER_WALKING_SPEED;
			}
			
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				player->VSpeed = -PLAYER_WALKING_SPEED;
			}
			
			
			
			if (!detectCollisionPlayer (window, player, (vec3){0, 1, 0}, deltaTime, player->VSpeed, chunk)) {
				player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
				for (int i = 0; i < 8; i++)
					player->collisionBox.vertex[i] = sum_vec3 (player->collisionBox.vertex[i], scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
			} else {

				for (int i = 0; i < 4; i++) {
					player->collisionBox.vertex[i].y = round(player->collisionBox.vertex[i].y);
				}
				
				for (int i = 4; i < 8; i++) {
					player->collisionBox.vertex[i].y = player->collisionBox.vertex[0].y+1.8;
				}

				player->camera.cameraPos.y = player->collisionBox.vertex[0].y + 1.62;

				player->VSpeed = 0;
				player->onGround = true;
			}
			
			
			
		} else {
			player->isFlying = false;
		}
		
		
	} else {
		
		if (!player->onGround) {
			player->VSpeed -= ((jumpSpeed*jumpSpeed)/2/1.251)*deltaTime;
			
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isPressedSpace) {
				isPressedSpace = true;
				if (player->flyTimer <= 0)
					player->flyTimer = 0.2;
				else if (player->flyTimer > 0) {
					player->isFlying = true;
					player->flyTimer = 0;
				}
			} else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && isPressedSpace)
				isPressedSpace = false;

			
			
			
			if (!detectCollisionPlayer (window, player, (vec3){0, 1, 0}, deltaTime, player->VSpeed, chunk)) {
				player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
				for (int i = 0; i < 8; i++)
					player->collisionBox.vertex[i] = sum_vec3 (player->collisionBox.vertex[i], scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
			} else {

				for (int i = 0; i < 4; i++) {
					player->collisionBox.vertex[i].y = round(player->collisionBox.vertex[i].y);
				}
				
				for (int i = 4; i < 8; i++) {
					player->collisionBox.vertex[i].y = player->collisionBox.vertex[0].y+1.8;
				}

				player->camera.cameraPos.y = player->collisionBox.vertex[0].y + 1.62;

				player->VSpeed = 0;
				player->onGround = true;
			}
			
		} else {
			
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
				isPressedSpace = true;
				player->VSpeed = jumpSpeed;
				player->onGround = false;
				
				if (player->flyTimer <= 0)
					player->flyTimer = 0.2;
				
				if (!detectCollisionPlayer (window, player, (vec3){0, 1, 0}, deltaTime, player->VSpeed, chunk)) {
					player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
					for (int i = 0; i < 8; i++)
						player->collisionBox.vertex[i] = sum_vec3 (player->collisionBox.vertex[i], scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
				} else
					player->VSpeed = 0;
			} else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && isPressedSpace)
				isPressedSpace = false;
			
			if (!detectCollisionPlayer (window, player, (vec3){0, 1, 0}, deltaTime, -1, chunk)) {
				player->onGround = false;
			}
		
		
		}
		
		
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	vec3 direction = (vec3){0, 0, 0};
	vec3 current = player->camera.cameraFront;
	
	current.y = 0;
	
	player->isMoving = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		direction = sum_vec3 (direction, normalize_vec3 (current));
		player->isMoving = true;
	}
	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		direction = sub_vec3 (direction, normalize_vec3 (current));
		player->isMoving = true;
	}
	
	
	current.y = player->camera.cameraFront.y;
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		direction = sub_vec3 (direction, normalize_vec3 (cross_vec3(current, player->camera.cameraUp)));
		player->isMoving = true;
	}
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		direction = sum_vec3 (direction, normalize_vec3 (cross_vec3(current, player->camera.cameraUp)));
		player->isMoving = true;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || (player->isSprinting && player->isMoving)) {
		player->HSpeed = PLAYER_WALKING_SPEED*1.3f;
		player->isSprinting = true;
	} else {
		player->HSpeed = PLAYER_WALKING_SPEED;
		player->isSprinting = false;
	}
	
	
	
	
	/*
	player.collisionBox.vertex[0] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, -0.3});
	player.collisionBox.vertex[1] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, -0.3});
	player.collisionBox.vertex[2] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, 0.3});
	player.collisionBox.vertex[3] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, 0.3});
	player.collisionBox.vertex[4] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, -0.3});
	player.collisionBox.vertex[5] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, -0.3});
	player.collisionBox.vertex[6] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, 0.3});
	player.collisionBox.vertex[7] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, 0.3});
	*/
	
	
	
	if (player->isMoving) {
		
		if (!player->HSpeed)
			player->HSpeed = PLAYER_WALKING_SPEED;
		
		//printf ("%f %f\n", direction.x, direction.z);
		direction = scale_vec3 (normalize_vec3 (direction), player->HSpeed * deltaTime);
		//printf ("%f %f\n", direction.x, direction.z);
		
		
		
		if (!detectCollisionPlayer (window, player, (vec3){direction.x, 0, 0}, deltaTime, 1, chunk)) {
			player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, (vec3){direction.x, 0, 0});
			for (int i = 0; i < 8; i++)
				player->collisionBox.vertex[i] = sum_vec3 (player->collisionBox.vertex[i], (vec3){direction.x, 0, 0});
		} /*else {
			if (isBlock (chunk, sum_vec3 (player->collisionBox.vertex[0], (vec3){direction.x, 0, 0})) ||
				isBlock (chunk, sum_vec3 (player->collisionBox.vertex[3], (vec3){direction.x, 0, 0})) ||
				isBlock (chunk, sum_vec3 (player->collisionBox.vertex[4], (vec3){direction.x, 0, 0})) ||
				isBlock (chunk, sum_vec3 (player->collisionBox.vertex[7], (vec3){direction.x, 0, 0}))) 
			{
				player->collisionBox.vertex[0].x = round (player->collisionBox.vertex[0].x);
				player->collisionBox.vertex[1].x = player->collisionBox.vertex[0].x + 0.6;
				player->collisionBox.vertex[2].x = player->collisionBox.vertex[0].x + 0.6;
				player->collisionBox.vertex[3].x = player->collisionBox.vertex[0].x;
				player->collisionBox.vertex[4].x = player->collisionBox.vertex[0].x;
				player->collisionBox.vertex[5].x = player->collisionBox.vertex[0].x + 0.6;
				player->collisionBox.vertex[6].x = player->collisionBox.vertex[0].x + 0.6;
				player->collisionBox.vertex[7].x = player->collisionBox.vertex[0].x;
				
				player->camera.cameraPos.x = player->collisionBox.vertex[0].x + 0.3;
				
				
			} else if (	isBlock (chunk, sum_vec3 (player->collisionBox.vertex[1], (vec3){direction.x, 0, 0})) ||
						isBlock (chunk, sum_vec3 (player->collisionBox.vertex[2], (vec3){direction.x, 0, 0})) ||
						isBlock (chunk, sum_vec3 (player->collisionBox.vertex[5], (vec3){direction.x, 0, 0})) ||
						isBlock (chunk, sum_vec3 (player->collisionBox.vertex[6], (vec3){direction.x, 0, 0})))
			{

				player->collisionBox.vertex[1].x = round (player->collisionBox.vertex[1].x);
				
				player->collisionBox.vertex[0].x = player->collisionBox.vertex[1].x - 0.6;
				player->collisionBox.vertex[2].x = player->collisionBox.vertex[1].x;
				player->collisionBox.vertex[3].x = player->collisionBox.vertex[1].x - 0.6;
				player->collisionBox.vertex[4].x = player->collisionBox.vertex[1].x - 0.6;
				player->collisionBox.vertex[5].x = player->collisionBox.vertex[1].x;
				player->collisionBox.vertex[6].x = player->collisionBox.vertex[1].x;
				player->collisionBox.vertex[7].x = player->collisionBox.vertex[1].x - 0.6;
				
				player->camera.cameraPos.x = player->collisionBox.vertex[0].x + 0.3;
				
			}
			
		}*/
		
		if (!detectCollisionPlayer (window, player, (vec3){0, 0, direction.z}, deltaTime, 1, chunk)) {
			player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, (vec3){0, 0, direction.z});
			for (int i = 0; i < 8; i++)
				player->collisionBox.vertex[i] = sum_vec3 (player->collisionBox.vertex[i], (vec3){0, 0, direction.z});
		} /*else {
			
			if (isBlock (chunk, sum_vec3 (player->collisionBox.vertex[0], (vec3){0, 0, direction.z})) ||
				isBlock (chunk, sum_vec3 (player->collisionBox.vertex[1], (vec3){0, 0, direction.z})) ||
				isBlock (chunk, sum_vec3 (player->collisionBox.vertex[4], (vec3){0, 0, direction.z})) ||
				isBlock (chunk, sum_vec3 (player->collisionBox.vertex[5], (vec3){0, 0, direction.z}))) 
			{
				//printf ("%f\n", player->collisionBox.vertex[0].z);
				player->collisionBox.vertex[0].z = round (player->collisionBox.vertex[0].z);
				//printf ("%f\n", player->collisionBox.vertex[0].z);
				player->collisionBox.vertex[1].z = player->collisionBox.vertex[0].z;
				player->collisionBox.vertex[2].z = player->collisionBox.vertex[0].z + 0.6;
				player->collisionBox.vertex[3].z = player->collisionBox.vertex[0].z + 0.6;
				player->collisionBox.vertex[4].z = player->collisionBox.vertex[0].z;
				player->collisionBox.vertex[5].z = player->collisionBox.vertex[0].z;
				player->collisionBox.vertex[6].z = player->collisionBox.vertex[0].z + 0.6;
				player->collisionBox.vertex[7].z = player->collisionBox.vertex[0].z + 0.6;
				
				player->camera.cameraPos.z = player->collisionBox.vertex[0].z + 0.3;
				
				
			} else if (	isBlock (chunk, sum_vec3 (player->collisionBox.vertex[2], (vec3){0, 0, direction.z})) ||
						isBlock (chunk, sum_vec3 (player->collisionBox.vertex[3], (vec3){0, 0, direction.z})) ||
						isBlock (chunk, sum_vec3 (player->collisionBox.vertex[6], (vec3){0, 0, direction.z})) ||
						isBlock (chunk, sum_vec3 (player->collisionBox.vertex[7], (vec3){0, 0, direction.z})))
			{
				//printf ("%f\n", player->collisionBox.vertex[2].z);
				/*player->collisionBox.vertex[2].z = round (player->collisionBox.vertex[2].z);
				//printf ("%f\n", player->collisionBox.vertex[2].z);
				
				player->collisionBox.vertex[0].z = player->collisionBox.vertex[2].z - 0.6;
				player->collisionBox.vertex[1].z = player->collisionBox.vertex[2].z - 0.6;
				player->collisionBox.vertex[3].z = player->collisionBox.vertex[2].z;
				player->collisionBox.vertex[4].z = player->collisionBox.vertex[2].z - 0.6;
				player->collisionBox.vertex[5].z = player->collisionBox.vertex[2].z - 0.6;
				player->collisionBox.vertex[6].z = player->collisionBox.vertex[2].z;
				player->collisionBox.vertex[7].z = player->collisionBox.vertex[2].z;
				
				player->camera.cameraPos.z = player->collisionBox.vertex[0].z + 0.3;
				
			}
			
		}*/
		
		
	} else {
		player->HSpeed = 0;
	}
}

collisionType detectCollisionPlayer (GLFWwindow* window, Player *player, vec3 movement, float deltaTime, float speed, node *chunk) {
	
	
	
	
	
	
	
	
	return isBlock (chunk, sum_vec3 (player->collisionBox.vertex[0], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (chunk, sum_vec3 (player->collisionBox.vertex[1], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (chunk, sum_vec3 (player->collisionBox.vertex[2], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (chunk, sum_vec3 (player->collisionBox.vertex[3], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (chunk, sum_vec3 (player->collisionBox.vertex[4], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (chunk, sum_vec3 (player->collisionBox.vertex[5], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (chunk, sum_vec3 (player->collisionBox.vertex[6], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (chunk, sum_vec3 (player->collisionBox.vertex[7], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)));
}






ray initializeRay (vec3 origin, vec3 direction) {
	ray aux;
	aux.origin = origin;
	aux.direction = direction;
	aux.invDirection = (vec3){1/direction.x, 1/direction.y, 1/direction.z};
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
	
	player.camera.cameraFront.x = cos(degToRad(yaw)) * cos(degToRad(pitch));
	player.camera.cameraFront.y = sin(degToRad(pitch));
	player.camera.cameraFront.z = sin(degToRad(yaw)) * cos(degToRad(pitch));
	player.camera.cameraFront = normalize_vec3 (player.camera.cameraFront);
}

int closestToZero (vec3 first, vec3 second) {
	
	int aux = abs(first.x) + abs(first.y) + abs(first.z);
	int aux2 = abs(second.x) + abs(second.y) + abs(second.z);
	
	return abs(aux) < abs(aux2);
}

block firstBlock (node *T, ray r) {
	vec3 base = (vec3){round(r.origin.x), round(r.origin.y), round(r.origin.z)};
	vec3 aux = base;
	block b;

	for (int i = 0; i < 1331; i++, aux = base) {
		aux = sum_vec3(aux, blocksInRange[i]);
		b = getElement (T, vecToBlock(aux));
		
		if (intersect(aux, r, 5) && b.id != -1) {
			return b;
		}
	}
	
	return (block){-1, -1, -1, -1};
}

int intersect (vec3 position, ray r, float range) {
	
	vec3 bounds[2] = {position, sum_vec3(position, (vec3){1, 1, 1})};
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

block vecToBlock (vec3 T) {
	return (block){(int)floor(T.x), (int)floor(T.y), (int)floor(T.z), -1};
}

int isBlock (node *T, vec3 pos) {
	
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

void saveWorld (node *chunk, FILE *p) {
    
    if (!chunk)
        return;
    
    fprintf(p, "%d %d %d %d\n", chunk->data.x, chunk->data.y, chunk->data.z, chunk->data.id);
    
    
    saveWorld (chunk->left, p);
    saveWorld (chunk->right, p);
}


void loadWorld (node **chunk, FILE *p) {
    
    vector pos;
    int id;
    
    while (fscanf (p, "%lf %lf %lf %d%*c", &pos.x, &pos.y, &pos.z, &id) != EOF)
        insertBlock (chunk, (block){(int)pos.x, (int)pos.y, (int)pos.z, id});
    
}
