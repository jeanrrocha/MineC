#include <stdio.h>
#include <avl.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

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



#define CUBE 192

const int windowWidth = 800;
const int windowHeight = 800;
const char* windowTitle = "MineC";

typedef struct {
	union {
		GLfloat col[3];
		struct {
			GLfloat x;
			GLfloat y;
			GLfloat z;
		};
	};
} vec3;

typedef struct {
	union {
		GLfloat col[4];
		struct {
			GLfloat w;
			GLfloat x;
			GLfloat y;
			GLfloat z;
		};
	};
} vec4;

typedef struct {
	
	union {
		vec4 row[4];
		struct {
			GLfloat ww;
			GLfloat wx;
			GLfloat wy;
			GLfloat wz;
			GLfloat xw;
			GLfloat xx;
			GLfloat xy;
			GLfloat xz;
			GLfloat yw;
			GLfloat yx;
			GLfloat yy;
			GLfloat yz;
			GLfloat zw;
			GLfloat zx;
			GLfloat zy;
			GLfloat zz;
		};
		struct {
			vec4 w;
			vec4 x;
			vec4 y;
			vec4 z;
		};
	};
} mat4;

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
	Camera camera;
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
void closeWindow (GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void drawAllBlocks (node *blocks, GLuint shader, GLuint VAOID[]);
void drawALL (node *blocks, GLuint shaders[], GLuint VAOID[], ray r);
void fillSkyBlock (node **chunk);


vec3 scale_vec3 (vec3 vector, double x) {
	
	vector.x *= x;
	vector.y *= x;
	vector.z *= x;
	
	return vector;
}

vec4 scale_vec4 (vec4 vector, double x) {
	
	vector.w *= x;
	vector.x *= x;
	vector.y *= x;
	vector.z *= x;
	
	return vector;
}

vec3 sum_vec3 (vec3 vector1, vec3 vector2) {
	
	vector1.x += vector2.x;
	vector1.y += vector2.y;
	vector1.z += vector2.z;
	
	return vector1;
}

vec4 sum_vec4 (vec4 vector1, vec4 vector2) {
	
	vector1.w += vector2.w;
	vector1.x += vector2.x;
	vector1.y += vector2.y;
	vector1.z += vector2.z;
	
	return vector1;
}

vec3 sub_vec3 (vec3 vector1, vec3 vector2) {
	
	vector1.x -= vector2.x;
	vector1.y -= vector2.y;
	vector1.z -= vector2.z;
	
	return vector1;
}

vec4 sub_vec4 (vec4 vector1, vec4 vector2) {
	
	vector1.w -= vector2.w;
	vector1.x -= vector2.x;
	vector1.y -= vector2.y;
	vector1.z -= vector2.z;
	
	return vector1;
}

vec3 multiply_vec3 (vec3 vector1, vec3 vector2) {
	
	vector1.x *= vector2.x;
	vector1.y *= vector2.y;
	vector1.z *= vector2.z;
	
	return vector1;
}

vec4 multiply_vec4 (vec4 vector1, vec4 vector2) {
	
	vector1.w *= vector2.w;
	vector1.x *= vector2.x;
	vector1.y *= vector2.y;
	vector1.z *= vector2.z;
	
	return vector1;
}


double dot_vec3 (vec3 vector1, vec3 vector2) {
	return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

double dot_vec4 (vec4 vector1, vec4 vector2) {
	return vector1.w * vector2.w + vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

double norm_vec3 (vec3 vector) {
	return sqrt (dot_vec3 (vector, vector));
}

double norm_vec4 (vec4 vector) {
	return sqrt (dot_vec4 (vector, vector));
}

vec3 normalize_vec3 (vec3 vector) {
	return scale_vec3 (vector, 1 / norm_vec3 (vector));
}

vec4 normalize_vec4 (vec4 vector) {
	return scale_vec4 (vector, 1 / norm_vec4 (vector));
}

vec3 cross_vec3 (vec3 vector1, vec3 vector2) {
	vec3 out;
	
	out.x = vector1.y*vector2.z - vector1.z*vector2.y;
	out.y = vector1.z*vector2.x - vector1.x*vector2.z;
	out.z = vector1.x*vector2.y - vector1.y*vector2.x;
	
	return out;
}

mat4 multiply_mat4 (mat4 matrix1, mat4 matrix2) {
	mat4 out;
	
	for(int i = 0; i < 4; i++) {    
		for(int j = 0; j < 4; j++) {    
			out.row[i].col[j] = 0;
			for(int k=0; k < 4 ; k++) {    
				out.row[i].col[j] += matrix1.row[i].col[k] * matrix2.row[k].col[j];    
			}
		}
	}

	return out;
}

mat4 lookAt_mat4 (vec3 from, vec3 to, vec3 up) {
	
	mat4 out = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	
	vec3 forward = sub_vec3 (from, to);
	forward = normalize_vec3 (forward);
	
	vec3 right = cross_vec3 (up, forward);
	right = normalize_vec3 (right);
	
	vec3 newUp = cross_vec3 (forward, right);
	
	out.ww = right.x;
	out.wx = newUp.x;
	out.wy = forward.x;
	
	out.xw = right.y;
	out.xx = newUp.y;
	out.xy = forward.y;
	
	out.yw = right.z;
	out.yx = newUp.z;
	out.yy = forward.z;
	
	out.zw = dot_vec3 (right, scale_vec3 (from, -1));
	out.zx = dot_vec3 (newUp, scale_vec3 (from, -1));
	out.zy = dot_vec3 (forward, scale_vec3 (from, -1));
	
	return out;
}

mat4 perspective_mat4 (float angle, float ratio, float distNear, float distFar) {
	mat4 out = (mat4){(vec4){0.0, 0.0, 0.0, 0.0}, (vec4){0.0, 0.0, 0.0, 0.0}, (vec4){0.0, 0.0, 0.0, 0.0}, (vec4){0.0, 0.0, 0.0, 0.0}};
	
	float tan_half_angle;
	
	tan_half_angle = tan(angle / 2);
	
	out.ww = (1 / (ratio * tan_half_angle));
	out.xx = (1 / tan_half_angle);
	out.yy = ((-(distFar + distNear)) / (distFar - distNear));
	out.yz = -1;
	out.zy = ((-(2 * distFar * distNear)) / (distFar - distNear));
	
	return out;
}

mat4 translate_mat4_vec3 (mat4 matrix, vec3 vector) {
	
	vec4 aux = (vec4){vector.x, vector.y, vector.z, 1};
	vec4 a = (vec4){matrix.ww, matrix.xw, matrix.yw, matrix.zw};
	matrix.zw = dot_vec4 (a, aux);
	
	a = (vec4){matrix.wx, matrix.xx, matrix.yx, matrix.zx};
	matrix.zx = dot_vec4 (a, aux);
	
	a = (vec4){matrix.wy, matrix.xy, matrix.yy, matrix.zy};
	matrix.zy = dot_vec4 (a, aux);
	
	a = (vec4){matrix.wz, matrix.xz, matrix.yz, matrix.zz};
	matrix.zz = dot_vec4 (a, aux);
	
	return matrix;
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
	
	player.camera.cameraPos = (vec3){0.0, 3.0, 0.0};
	player.camera.cameraFront = (vec3){0.0, 0.0, -1.0};
	player.camera.cameraUp = (vec3){0.0, 1.0, 0.0};
	player.camera.direction = (vec3){0.0, 0.0, 0.0};
	player.camera.view = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.proj = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};

	GLenum result = glbInit(NULL, NULL);
	
	/****************************************************/
	/* Inicialianzo o GLFW e a janela */
	
	GLFWwindow* window;
	
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
	glViewport(0, 0, windowWidth, windowHeight);
	
	/* FIM */
	/****************************************************/
	
	
	
	/****************************************************/
	/* Inicializando as Shaders */
	
	const char *vertexShaderSource = getShader("default.shader", VERTEX);
	const char *vertexShaderOutline = getShader("outline.shader", VERTEX);
	const char *fragmentShaderSource = getShader("default.shader", FRAGMENT);
	const char *fragmentShaderOutline = getShader("outline.shader", FRAGMENT);
	
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
	glfwSetKeyCallback(window, closeWindow);
	
	float cameraSpeed = 0.01f;
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.5059f, 0.8314f, 0.9804f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		player.camera.view = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
		player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
		player.camera.proj = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		    cameraSpeed = 0.1f;
		
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			vec3 aux = player.camera.cameraFront;
			
			aux.y = 0;
			
			aux = normalize_vec3 (aux);
			
			
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
			
			aux = scale_vec3 (aux, cameraSpeed);
			
			vec3 sumDirectionX = (vec3){aux.x, aux.y, 0};
			vec3 sumDirectionZ = (vec3){0, aux.y, aux.z};
			vec3 aux2 = player.camera.cameraPos;
			
			if (isBlock (chunk, (vec3){player.camera.cameraPos.x, player.camera.cameraPos.y-0.5, player.camera.cameraPos.z}) ||
				isBlock (chunk, (vec3){player.camera.cameraPos.x, player.camera.cameraPos.y-1.5, player.camera.cameraPos.z})) {
				aux.x = 0;
				aux.z = 0;
				}

			player.camera.cameraPos = sum_vec3 (player.camera.cameraPos, aux);
		}
		
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			vec3 aux = player.camera.cameraFront;
			
			aux.y = 0;
			
			aux = normalize_vec3 (aux);
			aux = scale_vec3 (aux, cameraSpeed);
			
			player.camera.cameraPos = sub_vec3 (player.camera.cameraPos, aux);
		}
		
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			
			vec3 aux;
			
			aux = cross_vec3 (player.camera.cameraFront, player.camera.cameraUp);
			aux = normalize_vec3 (aux);
			aux = scale_vec3 (aux, cameraSpeed);
			
			player.camera.cameraPos = sub_vec3 (player.camera.cameraPos, aux);
			
		}
		
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			vec3 aux;
			
			aux = cross_vec3 (player.camera.cameraFront, player.camera.cameraUp);
			aux = normalize_vec3 (aux);
			aux = scale_vec3 (aux, cameraSpeed);
			
			player.camera.cameraPos = sum_vec3 (player.camera.cameraPos, aux);
		}
		
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			player.camera.cameraPos = sum_vec3 (player.camera.cameraPos, scale_vec3 (player.camera.cameraUp, cameraSpeed));
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		    if (!isBlock (chunk, (vec3){player.camera.cameraPos.x, player.camera.cameraPos.y-1.81, player.camera.cameraPos.z}))
				player.camera.cameraPos = sub_vec3 (player.camera.cameraPos, scale_vec3 (player.camera.cameraUp, cameraSpeed));
		}
		
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		    cameraSpeed += 0.001f;
		
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		    cameraSpeed -= 0.001f;
		
		ray r = initializeRay (player.camera.cameraPos, player.camera.cameraFront);
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && help == 0) {
		    block aux = firstBlock(chunk, r);
		    if (aux.id != -1)
		    	removeBlock (&chunk, aux);
			help = 20;
		}
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && help == 0) {
		    
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
				posBlock.id = 1;
				
			} else if (!isBlock (chunk, sum_vec3 (aux, down)) && intersect(sum_vec3 (aux, down), r, 5) && player.camera.cameraPos.y < aux.y-0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, down));
				posBlock.id = 1;
				
			} else if (!isBlock (chunk, sum_vec3 (aux, right)) &&  intersect(sum_vec3 (aux, right), r, 5) && player.camera.cameraPos.x > aux.x+0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, right));
				posBlock.id = 1;
				
			} else if (!isBlock (chunk, sum_vec3 (aux, left)) &&  intersect(sum_vec3 (aux, left), r, 5) && player.camera.cameraPos.x < aux.x-0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, left));
				posBlock.id = 1;
			 
			} else if (!isBlock (chunk, sum_vec3 (aux, front)) &&  intersect(sum_vec3 (aux, front), r, 5) && player.camera.cameraPos.z > aux.z+0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, front));
				posBlock.id = 1;
			
			} else if (!isBlock (chunk, sum_vec3 (aux, back)) &&  intersect(sum_vec3 (aux, back), r, 5) && player.camera.cameraPos.z < aux.z-0.5) {
				
				posBlock = vecToBlock (sum_vec3 (aux, back));
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
			player.camera.cameraPos = (vec3){0.0f, 3.5f, 0.0f};
			player.camera.cameraFront = (vec3){0.0f, 0.0f, -1.0f};
			player.camera.cameraUp = (vec3){0.0f, 1.0f, 0.0f};
			lastX = 400;
			lastY = 400;
			yaw = -90.0f;
			pitch = 0.0f;
			clearChunk (&chunk);
			fillSkyBlock (&chunk);
		}
		cameraSpeed = 0.05f;
		
		player.camera.view = lookAt_mat4 (player.camera.cameraPos, sum_vec3 (player.camera.cameraPos, player.camera.cameraFront), player.camera.cameraUp);
		
		player.camera.proj = perspective_mat4 ((float)degToRad(90.f), (float)(windowWidth/windowHeight), 0.2f, 600.0f);
		
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

void drawBlockOutline (block T, GLuint shader, GLuint VAOID[]);
void drawBlocks (node *blocks, GLuint shader, GLuint VAOID[]);

void drawALL (node *blocks, GLuint *shaders, GLuint *VAOID, ray r) {

	drawBlocks (blocks, shaders[0], VAOID);
	
	block first = firstBlock (blocks, r);
	if (first.id != -1)
		drawBlockOutline (first, shaders[1], VAOID);
	
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
	
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 144, GL_UNSIGNED_INT, 0);
}

void drawAllBlocks (node *blocks, GLuint shader, GLuint VAOID[]){
	if (!blocks)
		return;
	
	player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.model = translate_mat4_vec3 (player.camera.model, (vec3) {blocks->data.x, blocks->data.y, blocks->data.z});
	
	int modelLoc = glGetUniformLocation(shader, "model");
	glBindVertexArray(VAOID[blocks->data.id]);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &player.camera.model.ww);

	glDrawElements(GL_TRIANGLES, sizeof(GLuint)*36/sizeof(int), GL_UNSIGNED_INT, 0);
	
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

int intersect (vec3 position, ray r, float range) {
	
	vec3 bounds[2] = {sum_vec3(position, (vec3){-0.5, -0.5, -0.5}), sum_vec3(position, (vec3){0.5, 0.5, 0.5})};
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
	return (block){round(T.x), round(T.y), round(T.z), -1};
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