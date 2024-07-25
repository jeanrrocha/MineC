#define GLBIND_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <glbind.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "avl.h"
#include "blocks.h"
#include "mat.h"
#include "blocks.h"

typedef struct {
	vec3 origin;
	vec3 direction;
	vec3 invDirection;
	int sign[3];
} ray;

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


block vecToBlock (vec3 T);
ray initializeRay (vec3 origin, vec3 direction);
int intersect (vec3 position, ray r, float range);
int closestToZero (vec3 first, vec3 second);
block firstBlock (CHUNK_MANAGER *chunks, ray r);
int isBlock (node *T, vec3 pos);
const char* getShader (char *filePath, char type);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void drawAllChunks (CHUNK_MANAGER chunks, GLuint shader, GLuint VAOID[]);
void drawChunk (CHUNK chunk, GLuint shader, GLuint VAOID[]);
void drawAllBlocks (node *blocks, CHUNK_MANAGER *chunks, GLuint shader, GLuint VAOID[]);
void drawALL (node *blocks, CHUNK_MANAGER *chunks, GLuint shaders[], GLuint VAOID[], ray r);
void fillSkyBlock ();
void saveWorld (node *chunk, FILE *p);
void loadWorld (CHUNK_MANAGER *chunks, node **chunk, FILE *p);
void saveChunks (CHUNK_MANAGER *chunks);
void saveChunk (CHUNK *chunk);
void loadChunks (CHUNK_MANAGER *chunks);
void loadChunk (CHUNK *chunk);
void processMovement (GLFWwindow* window, Player *player, float deltaTime, CHUNK_MANAGER *chunks);
bool detectCollisionPlayer (GLFWwindow* window, Player *player, vec3 movement, float deltaTime, float speed, CHUNK_MANAGER *chunks);

void moveChunkX (CHUNK_MANAGER *chunks, int x);
void moveChunkY (CHUNK_MANAGER *chunks, int y);
void moveChunkZ (CHUNK_MANAGER *chunks, int z);

vec3 cordToChunkCord (vec3 pos);
void insertBlockToChunk (CHUNK_MANAGER *chunks, BLOCK b);
void removeBlockToChunk (CHUNK_MANAGER *chunks, BLOCK b);

int searchElement (node *T, block data);
block getElement (node *T, block data);