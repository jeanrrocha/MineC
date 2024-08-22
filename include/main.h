
#ifndef GLBIND_IMPLEMENTATION
	#define GLBIND_IMPLEMENTATION
	#include <glbind.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "types.h"

BLOCK vecToBlock (vec3 T);
ray initializeRay (vec3 origin, vec3 direction);
int intersect (vec3 position, ray r, float range);
int closestToZero (vec3 first, vec3 second);
BLOCK firstBlock (CHUNK_MANAGER *chunks, ray r);
const char* getShader (char *filePath, char type);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void drawAllChunks (CHUNK_MANAGER chunks, GLuint shader, GLuint VAOID[]);
void drawChunk (CHUNK chunk, GLuint shader, GLuint VAOID[]);
void fillSkyBlock ();

int isBlock (TREE *tree, vec3 pos);

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

//void saveWorld (game_state *state);
void saveChunks (CHUNK_MANAGER *chunks);
void saveChunk (CHUNK *chunk);
void saveBlocks (TREE_NODE* node, FILE *p);

TREE *relativePos (CHUNK_MANAGER *chunks, vec3 pos);