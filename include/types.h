#ifndef TYPES_H
#define TYPES_H
	
	#define GLFW_INCLUDE_NONE
	#include <GLFW/glfw3.h>
	
	#include "tree.h"
	#define GLFW_INCLUDE_NONE
	#include <GLFW/glfw3.h>

	typedef float GLfloat;
	typedef struct ray ray;
	typedef struct camera camera;
	typedef struct box box;
	typedef struct Player Player;
	typedef struct vec2 vec2;
	typedef struct vec3 vec3;
	typedef struct vec4 vec4;
	typedef struct mat4 mat4;
	typedef struct game_state game_state;
	
	typedef struct TREE TREE;
	
	/*//
	*
	*
	*
	*	ENUMS
	*
	*
	*
	*
	//*/
	
	typedef enum block_id {
		dirt = 1,
		grass,
		bricks,
		oak_log,
		leaves_oak,
		redstone_block,
		redstone_lamp,
		redstone_lamp_lit,
		redstone_ore,
		redstone_ore_lit,
		piston,
		piston_lit,
		block_id_end
	} block_id;

	typedef enum game_states {
		main_menu,
		gameplay,
		game_states_end
	} game_states;

	typedef enum main_menu_buttons {
		main_menu_single_player,
		main_menu_end
	} main_menu_buttons;
	
	
	
	
	/*//
	*
	*
	*
	*	STRUCTS FOR MATRIX OPERATIONS
	*
	*
	*
	//*/
	
	struct vec2 {
		union {
			GLfloat col[2];
			struct {
				GLfloat x;
				GLfloat y;
			};
		};
	};

	struct vec3 {
		union {
			GLfloat col[3];
			struct {
				GLfloat x;
				GLfloat y;
				GLfloat z;
			};
		};
	};

	struct vec4 {
		union {
			GLfloat col[4];
			struct {
				GLfloat w;
				GLfloat x;
				GLfloat y;
				GLfloat z;
			};
		};
	};

	struct mat4 {
		
		union {
			vec4 row[4];
			GLfloat elements[16];
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
	};
	
	/*
	*
	*
	*
	*
	*
	*
	*
	//*/
	
	struct ray {
		vec3 origin;
		vec3 direction;
		vec3 invDirection;
		int sign[3];
	};

	struct camera {
		vec3 cameraPos;
		vec3 cameraFront;
		vec3 cameraUp;
		vec3 direction;
		mat4 view;
		mat4 model;
		mat4 proj;
	};

	struct box {
		vec3 vertex[8];
	};

	struct Player {
		camera camera;
		box collisionbox;
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
	};
	
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
		bool tick;
	} BLOCK;

	typedef struct {
		vec3 pos;
		TREE *blocks;
	} CHUNK;
	
	/*
	*
	*
	*
	*
	*
	*
	*
	//*/
	
	typedef struct {
		GLfloat yaw;
		GLfloat pitch;
		GLfloat lastX;
		GLfloat lastY;
		bool is_mouse_button_left_pressed;
		bool is_mouse_button_right_pressed;
	} MOUSE_MANAGER;

	typedef struct {
		bool is_button_0_pressed;
		bool is_button_1_pressed;
		bool is_button_2_pressed;
		bool is_button_3_pressed;
		bool is_button_4_pressed;
		bool is_button_5_pressed;
		bool is_button_6_pressed;
		bool is_button_7_pressed;
		bool is_button_8_pressed;
		bool is_button_9_pressed;
		
		bool is_button_Q_pressed;
		bool is_button_W_pressed;
		bool is_button_E_pressed;
		bool is_button_R_pressed;
		bool is_button_T_pressed;
		bool is_button_Y_pressed;
		bool is_button_U_pressed;
		bool is_button_I_pressed;
		bool is_button_O_pressed;
		bool is_button_P_pressed;
		
		bool is_button_A_pressed;
		bool is_button_S_pressed;
		bool is_button_D_pressed;
		bool is_button_F_pressed;
		bool is_button_G_pressed;
		bool is_button_H_pressed;
		bool is_button_J_pressed;
		bool is_button_K_pressed;
		bool is_button_L_pressed;
		
		bool is_button_Z_pressed;
		bool is_button_X_pressed;
		bool is_button_C_pressed;
		bool is_button_V_pressed;
		bool is_button_B_pressed;
		bool is_button_N_pressed;
		bool is_button_M_pressed;
		
		bool is_button_shift_pressed;
	} KEYBOARD_MANAGER;
	
	typedef struct {
		vec3 currentPos;
		CHUNK chunks[3][3][3];
	} CHUNK_MANAGER;

	
	typedef struct {
		int VAO;
		int VBO;
		int EBO;
		vec2 size;
		vec2 normalized_size;
		vec4 pos;
		vec2 absolute_pos;
		vec2 vertex[2];
		int texture;
		void (*exec)(game_state*);
	} BUTTON;
	
	struct game_state {
		GLFWwindow* window;
		int window_width;
		int window_height;
		char *current_world;
		char *path_to_world;
		char *path_to_world_chunks;
		int state;
		CHUNK_MANAGER *chunks_info;
		MOUSE_MANAGER *mouse_info;
		KEYBOARD_MANAGER *keyboard_info;
		Player *player;
		float deltaTime;
		void (*block_update_functions[block_id_end])(BLOCK*, CHUNK_MANAGER*);
		BUTTON **buttons_info;
	};
	
#endif
