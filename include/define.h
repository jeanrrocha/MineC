#ifndef DEFINE_H
#define DEFINE_H
	
	#include "mat.h"
	#include "stb_image.h"
	#include "avl.h"
	#include "blocks.h"
	
	typedef struct {
		vec3 pos;
		node *blocks;
	} CHUNK;

	typedef struct {
		vec3 currentPos;
		CHUNK chunks[3][3][3];
	} CHUNK_MANAGER;

	typedef struct {
		vec3 pos;
		int id;
		short north;
		short south;
		short east;
		short west;
		short power;
		bool powered;
		bool power_source;
		bool lit;
		void (*update)(struct BLOCK *, struct CHUNK_MANAGER *);
	} BLOCK;

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
	
#endif