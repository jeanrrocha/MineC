#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "types.h"

#include "main.h"
#include "stb_image.h"
#include "blocks.h"
#include "mat.h"
#include "blocks.h"
#include "tree.h"

#define FRAGMENT 'f'
#define VERTEX 'v'

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

#ifndef M_PI
#    define M_PI 3.14159265358979323846264338327950288
#endif

#define PLAYER_WALKING_SPEED 4.317f
#define CUBE 96


//const int windowWidth = 1760;
const int windowWidth = 1366;
//const int windowHeight = 990;
const int windowHeight = 768;

const char* windowTitle = "MineC";

Player player;
float help = 0;
GLuint outlineVAO[1] = {0};
float deltaTime = 0.f;
GLuint VAO, VBO, EBO;
GLuint shaderProgram;
GLuint outlineShaderProgram;
GLuint shaderProgram_2D;

vec3 blocksInRange[1331] = {(vec3){0, 0, 0}};
CHUNK_MANAGER test_chunks;

void print_block (BLOCK b) {
	printf ("XYZ: %f %f %f\n", b.pos.x, b.pos.y, b.pos.z);
}



void test (){
	
}

void generate_shader (GLuint *shader, char *source) {
	const char *vertexSource = getShader(source, VERTEX);
	const char *fragmentSource = getShader(source, FRAGMENT);
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	
	*shader = glCreateProgram();
	
	glAttachShader (*shader, vertexShader);
	glAttachShader (*shader, fragmentShader);
	
	glLinkProgram (*shader);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}



typedef struct {
	int VAO;
	int VBO;
	int EBO;
	vec2 size;
	vec2 normalized_size;
	vec4 pos;
	vec2 absolute_pos;
	int texture;
} element_2d;

void loadWorld (game_state *state);
void saveWorld (game_state *state);

game_state ESTADO;
MOUSE_MANAGER mouse;

int comp_blocks (const void *a, const void *b) {
	BLOCK first = *(BLOCK*)a;
	BLOCK second = *(BLOCK*)b;
	
	if (first.pos.x != second.pos.x)
		return first.pos.x < second.pos.x ? 1 : -1;
	if (first.pos.y != second.pos.y)
		return first.pos.y < second.pos.y ? 1 : -1;
	if (first.pos.z != second.pos.z)
		return first.pos.z < second.pos.z ? 1 : -1;
	return 0;
	
}

void drawALL (game_state *state, GLuint *shaders, GLuint *VAOID, ray r);


BUTTON button_init (game_state *state, vec2 size, vec2 pos, float* vertexs, int* indexes, char* texture_path, void (*exec)(game_state*)) {
	BUTTON b;
	b.exec = exec;
	b.size = size;
	b.normalized_size = (vec2){size.x / state->window_width, size.y / state->window_height};
	b.absolute_pos = pos;
	b.pos = (vec4){pos.x - b.normalized_size.x / 2, pos.y - b.normalized_size.y / 2};
	b.vertex[0] = (vec2){b.pos.w * state->window_width, b.pos.x * state->window_height};
	b.vertex[1] = (vec2){b.vertex[0].x + size.x, b.vertex[0].y + size.y};
	
	printf ("%f %f\n", b.vertex[0].x, b.vertex[0].y);
	printf ("%f %f\n", b.vertex[1].x, b.vertex[1].y);
	
	glGenTextures (1, &b.texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, b.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	int widht_img, height_img, num_col_ch;
	unsigned char* textureBytes = stbi_load (texture_path, &widht_img, &height_img, &num_col_ch, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widht_img, height_img, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(textureBytes);
	
	if (vertexs && indexes) {
		glGenVertexArrays(1, &b.VAO);
		glGenBuffers(1, &b.VBO);
		glGenBuffers(1, &b.EBO);
		
		glBindVertexArray(b.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, b.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b.EBO);
		
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertexs, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), indexes, GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	} else {
		glGenVertexArrays(1, &b.VAO);
		glGenBuffers(1, &b.VBO);
		glGenBuffers(1, &b.EBO);
		
		glBindVertexArray(b.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, b.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b.EBO);
		
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), (float[]){0., 0., 0., 0., 0., size.y / state->window_height, 0., 1., size.x / state->window_width, size.y / state->window_height, 1., 1., size.x / state->window_width, 0., 1., 0.}, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), (int[]){0, 1, 2, 0, 2, 3}, GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	return b;
}

void draw_button (game_state *state, BUTTON b) {
	glBindVertexArray(b.VAO);
	glBindTexture(GL_TEXTURE_2D, b.texture);

	int pos = glGetUniformLocation(shaderProgram_2D, "p");
	glUniform4fv(pos, 1, &b.pos.w);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



/*
bool is_button_1_pressed;
bool is_button_2_pressed;
bool is_button_3_pressed;
bool is_button_4_pressed;
bool is_button_5_pressed;
bool is_button_6_pressed;
bool is_button_7_pressed;
bool is_button_8_pressed;
bool is_button_9_pressed;

bool is_button_q_pressed;
bool is_button_w_pressed;
bool is_button_e_pressed;
bool is_button_r_pressed;
bool is_button_t_pressed;
bool is_button_y_pressed;
bool is_button_u_pressed;
bool is_button_i_pressed;
bool is_button_o_pressed;
bool is_button_p_pressed;

bool is_button_a_pressed;
bool is_button_s_pressed;
bool is_button_d_pressed;
bool is_button_f_pressed;
bool is_button_g_pressed;
bool is_button_h_pressed;
bool is_button_j_pressed;
bool is_button_k_pressed;
bool is_button_l_pressed;

bool is_button_z_pressed;
bool is_button_x_pressed;
bool is_button_c_pressed;
bool is_button_v_pressed;
bool is_button_b_pressed;
bool is_button_n_pressed;
bool is_button_m_pressed;
*/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	game_state *state = (game_state*)glfwGetWindowUserPointer(window);
	
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS && !state->keyboard_info->is_button_shift_pressed) {
		state->keyboard_info->is_button_shift_pressed = true;
	} else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE && state->keyboard_info->is_button_shift_pressed) {
		state->keyboard_info->is_button_shift_pressed = false;
	}
	
	
	
	if (key == GLFW_KEY_0 && action == GLFW_PRESS && !state->keyboard_info->is_button_0_pressed) {
		state->keyboard_info->is_button_0_pressed = true;
		state->current_world[strlen(state->current_world)] = '0';
	} else if (key == GLFW_KEY_0 && action == GLFW_RELEASE && state->keyboard_info->is_button_0_pressed) {
		state->keyboard_info->is_button_0_pressed = false;
	}
	
	if (key == GLFW_KEY_1 && action == GLFW_PRESS && !state->keyboard_info->is_button_1_pressed) {
		state->keyboard_info->is_button_1_pressed = true;
		state->current_world[strlen(state->current_world)] = '1';
	} else if (key == GLFW_KEY_1 && action == GLFW_RELEASE && state->keyboard_info->is_button_1_pressed) {
		state->keyboard_info->is_button_1_pressed = false;
	}
	
	if (key == GLFW_KEY_2 && action == GLFW_PRESS && !state->keyboard_info->is_button_2_pressed) {
		state->keyboard_info->is_button_2_pressed = true;
		state->current_world[strlen(state->current_world)] = '2';
	} else if (key == GLFW_KEY_2 && action == GLFW_RELEASE && state->keyboard_info->is_button_2_pressed) {
		state->keyboard_info->is_button_2_pressed = false;
	}
	
	if (key == GLFW_KEY_3 && action == GLFW_PRESS && !state->keyboard_info->is_button_3_pressed) {
		state->keyboard_info->is_button_3_pressed = true;
		state->current_world[strlen(state->current_world)] = '3';
	} else if (key == GLFW_KEY_3 && action == GLFW_RELEASE && state->keyboard_info->is_button_3_pressed) {
		state->keyboard_info->is_button_3_pressed = false;
	}
	
	if (key == GLFW_KEY_4 && action == GLFW_PRESS && !state->keyboard_info->is_button_4_pressed) {
		state->keyboard_info->is_button_4_pressed = true;
		state->current_world[strlen(state->current_world)] = '4';
	} else if (key == GLFW_KEY_4 && action == GLFW_RELEASE && state->keyboard_info->is_button_4_pressed) {
		state->keyboard_info->is_button_4_pressed = false;
	}
	
	if (key == GLFW_KEY_5 && action == GLFW_PRESS && !state->keyboard_info->is_button_5_pressed) {
		state->keyboard_info->is_button_5_pressed = true;
		state->current_world[strlen(state->current_world)] = '5';
	} else if (key == GLFW_KEY_5 && action == GLFW_RELEASE && state->keyboard_info->is_button_5_pressed) {
		state->keyboard_info->is_button_5_pressed = false;
	}
	
	if (key == GLFW_KEY_6 && action == GLFW_PRESS && !state->keyboard_info->is_button_6_pressed) {
		state->keyboard_info->is_button_6_pressed = true;
		state->current_world[strlen(state->current_world)] = '6';
	} else if (key == GLFW_KEY_6 && action == GLFW_RELEASE && state->keyboard_info->is_button_6_pressed) {
		state->keyboard_info->is_button_6_pressed = false;
	}
	
	if (key == GLFW_KEY_7 && action == GLFW_PRESS && !state->keyboard_info->is_button_7_pressed) {
		state->keyboard_info->is_button_7_pressed = true;
		state->current_world[strlen(state->current_world)] = '7';
	} else if (key == GLFW_KEY_7 && action == GLFW_RELEASE && state->keyboard_info->is_button_7_pressed) {
		state->keyboard_info->is_button_7_pressed = false;
	}
	
	if (key == GLFW_KEY_8 && action == GLFW_PRESS && !state->keyboard_info->is_button_8_pressed) {
		state->keyboard_info->is_button_8_pressed = true;
		state->current_world[strlen(state->current_world)] = '8';
	} else if (key == GLFW_KEY_8 && action == GLFW_RELEASE && state->keyboard_info->is_button_8_pressed) {
		state->keyboard_info->is_button_8_pressed = false;
	}
	
	if (key == GLFW_KEY_9 && action == GLFW_PRESS && !state->keyboard_info->is_button_9_pressed) {
		state->keyboard_info->is_button_9_pressed = true;
		state->current_world[strlen(state->current_world)] = '9';
	} else if (key == GLFW_KEY_9 && action == GLFW_RELEASE && state->keyboard_info->is_button_9_pressed) {
		state->keyboard_info->is_button_9_pressed = false;
	}
	
	if (key == GLFW_KEY_Q && action == GLFW_PRESS && !state->keyboard_info->is_button_Q_pressed) {
		state->keyboard_info->is_button_Q_pressed = true;
		state->current_world[strlen(state->current_world)] = 'q' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_Q && action == GLFW_RELEASE && state->keyboard_info->is_button_Q_pressed) {
		state->keyboard_info->is_button_Q_pressed = false;
	}
	
	if (key == GLFW_KEY_W && action == GLFW_PRESS && !state->keyboard_info->is_button_W_pressed) {
		state->keyboard_info->is_button_W_pressed = true;
		state->current_world[strlen(state->current_world)] = 'w' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_W && action == GLFW_RELEASE && state->keyboard_info->is_button_W_pressed) {
		state->keyboard_info->is_button_W_pressed = false;
	}
	
	if (key == GLFW_KEY_E && action == GLFW_PRESS && !state->keyboard_info->is_button_E_pressed) {
		state->keyboard_info->is_button_E_pressed = true;
		state->current_world[strlen(state->current_world)] = 'e' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_E && action == GLFW_RELEASE && state->keyboard_info->is_button_E_pressed) {
		state->keyboard_info->is_button_E_pressed = false;
	}
	
	if (key == GLFW_KEY_R && action == GLFW_PRESS && !state->keyboard_info->is_button_R_pressed) {
		state->keyboard_info->is_button_R_pressed = true;
		state->current_world[strlen(state->current_world)] = 'r' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_R && action == GLFW_RELEASE && state->keyboard_info->is_button_R_pressed) {
		state->keyboard_info->is_button_R_pressed = false;
	}
	
	if (key == GLFW_KEY_T && action == GLFW_PRESS && !state->keyboard_info->is_button_T_pressed) {
		state->keyboard_info->is_button_T_pressed = true;
		state->current_world[strlen(state->current_world)] = 't' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_T && action == GLFW_RELEASE && state->keyboard_info->is_button_T_pressed) {
		state->keyboard_info->is_button_T_pressed = false;
	}
	
	if (key == GLFW_KEY_Y && action == GLFW_PRESS && !state->keyboard_info->is_button_Y_pressed) {
		state->keyboard_info->is_button_Y_pressed = true;
		state->current_world[strlen(state->current_world)] = 'y' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_Y && action == GLFW_RELEASE && state->keyboard_info->is_button_Y_pressed) {
		state->keyboard_info->is_button_Y_pressed = false;
	}
	
	if (key == GLFW_KEY_U && action == GLFW_PRESS && !state->keyboard_info->is_button_U_pressed) {
		state->keyboard_info->is_button_U_pressed = true;
		state->current_world[strlen(state->current_world)] = 'u' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_U && action == GLFW_RELEASE && state->keyboard_info->is_button_U_pressed) {
		state->keyboard_info->is_button_U_pressed = false;
	}
	
	if (key == GLFW_KEY_I && action == GLFW_PRESS && !state->keyboard_info->is_button_I_pressed) {
		state->keyboard_info->is_button_I_pressed = true;
		state->current_world[strlen(state->current_world)] = 'i' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_I && action == GLFW_RELEASE && state->keyboard_info->is_button_I_pressed) {
		state->keyboard_info->is_button_I_pressed = false;
	}
	
	if (key == GLFW_KEY_O && action == GLFW_PRESS && !state->keyboard_info->is_button_O_pressed) {
		state->keyboard_info->is_button_O_pressed = true;
		state->current_world[strlen(state->current_world)] = 'o' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_O && action == GLFW_RELEASE && state->keyboard_info->is_button_O_pressed) {
		state->keyboard_info->is_button_O_pressed = false;
	}
	
	if (key == GLFW_KEY_P && action == GLFW_PRESS && !state->keyboard_info->is_button_P_pressed) {
		state->keyboard_info->is_button_P_pressed = true;
		state->current_world[strlen(state->current_world)] = 'p' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_P && action == GLFW_RELEASE && state->keyboard_info->is_button_P_pressed) {
		state->keyboard_info->is_button_P_pressed = false;
	}
	
	if (key == GLFW_KEY_A && action == GLFW_PRESS && !state->keyboard_info->is_button_A_pressed) {
		state->keyboard_info->is_button_A_pressed = true;
		state->current_world[strlen(state->current_world)] = 'a' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_A && action == GLFW_RELEASE && state->keyboard_info->is_button_A_pressed) {
		state->keyboard_info->is_button_A_pressed = false;
	}
	
	if (key == GLFW_KEY_S && action == GLFW_PRESS && !state->keyboard_info->is_button_S_pressed) {
		state->keyboard_info->is_button_S_pressed = true;
		state->current_world[strlen(state->current_world)] = 's' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_S && action == GLFW_RELEASE && state->keyboard_info->is_button_S_pressed) {
		state->keyboard_info->is_button_S_pressed = false;
	}
	
	if (key == GLFW_KEY_D && action == GLFW_PRESS && !state->keyboard_info->is_button_D_pressed) {
		state->keyboard_info->is_button_D_pressed = true;
		state->current_world[strlen(state->current_world)] = 'd' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_D && action == GLFW_RELEASE && state->keyboard_info->is_button_D_pressed) {
		state->keyboard_info->is_button_D_pressed = false;
	}
	
	if (key == GLFW_KEY_F && action == GLFW_PRESS && !state->keyboard_info->is_button_F_pressed) {
		state->keyboard_info->is_button_F_pressed = true;
		state->current_world[strlen(state->current_world)] = 'f' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_F && action == GLFW_RELEASE && state->keyboard_info->is_button_F_pressed) {
		state->keyboard_info->is_button_F_pressed = false;
	}
	
	if (key == GLFW_KEY_G && action == GLFW_PRESS && !state->keyboard_info->is_button_G_pressed) {
		state->keyboard_info->is_button_G_pressed = true;
		state->current_world[strlen(state->current_world)] = 'g' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_G && action == GLFW_RELEASE && state->keyboard_info->is_button_G_pressed) {
		state->keyboard_info->is_button_G_pressed = false;
	}
	
	if (key == GLFW_KEY_H && action == GLFW_PRESS && !state->keyboard_info->is_button_H_pressed) {
		state->keyboard_info->is_button_H_pressed = true;
		state->current_world[strlen(state->current_world)] = 'h' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_H && action == GLFW_RELEASE && state->keyboard_info->is_button_H_pressed) {
		state->keyboard_info->is_button_H_pressed = false;
	}
	
	if (key == GLFW_KEY_J && action == GLFW_PRESS && !state->keyboard_info->is_button_J_pressed) {
		state->keyboard_info->is_button_J_pressed = true;
		state->current_world[strlen(state->current_world)] = 'j' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_J && action == GLFW_RELEASE && state->keyboard_info->is_button_J_pressed) {
		state->keyboard_info->is_button_J_pressed = false;
	}
	
	if (key == GLFW_KEY_K && action == GLFW_PRESS && !state->keyboard_info->is_button_K_pressed) {
		state->keyboard_info->is_button_K_pressed = true;
		state->current_world[strlen(state->current_world)] = 'k' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_K && action == GLFW_RELEASE && state->keyboard_info->is_button_K_pressed) {
		state->keyboard_info->is_button_K_pressed = false;
	}
	
	if (key == GLFW_KEY_L && action == GLFW_PRESS && !state->keyboard_info->is_button_L_pressed) {
		state->keyboard_info->is_button_L_pressed = true;
		state->current_world[strlen(state->current_world)] = 'l' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_L && action == GLFW_RELEASE && state->keyboard_info->is_button_L_pressed) {
		state->keyboard_info->is_button_L_pressed = false;
	}
	
	if (key == GLFW_KEY_Z && action == GLFW_PRESS && !state->keyboard_info->is_button_Z_pressed) {
		state->keyboard_info->is_button_Z_pressed = true;
		state->current_world[strlen(state->current_world)] = 'z' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_Z && action == GLFW_RELEASE && state->keyboard_info->is_button_Z_pressed) {
		state->keyboard_info->is_button_Z_pressed = false;
	}
	
	if (key == GLFW_KEY_X && action == GLFW_PRESS && !state->keyboard_info->is_button_X_pressed) {
		state->keyboard_info->is_button_X_pressed = true;
		state->current_world[strlen(state->current_world)] = 'x' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_X && action == GLFW_RELEASE && state->keyboard_info->is_button_X_pressed) {
		state->keyboard_info->is_button_X_pressed = false;
	}
	
	if (key == GLFW_KEY_C && action == GLFW_PRESS && !state->keyboard_info->is_button_C_pressed) {
		state->keyboard_info->is_button_C_pressed = true;
		state->current_world[strlen(state->current_world)] = 'c' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_C && action == GLFW_RELEASE && state->keyboard_info->is_button_C_pressed) {
		state->keyboard_info->is_button_C_pressed = false;
	}
	
	if (key == GLFW_KEY_V && action == GLFW_PRESS && !state->keyboard_info->is_button_V_pressed) {
		state->keyboard_info->is_button_V_pressed = true;
		state->current_world[strlen(state->current_world)] = 'v' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_V && action == GLFW_RELEASE && state->keyboard_info->is_button_V_pressed) {
		state->keyboard_info->is_button_V_pressed = false;
	}
	
	if (key == GLFW_KEY_B && action == GLFW_PRESS && !state->keyboard_info->is_button_B_pressed) {
		state->keyboard_info->is_button_B_pressed = true;
		state->current_world[strlen(state->current_world)] = 'b' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_B && action == GLFW_RELEASE && state->keyboard_info->is_button_B_pressed) {
		state->keyboard_info->is_button_B_pressed = false;
	}
	
	if (key == GLFW_KEY_N && action == GLFW_PRESS && !state->keyboard_info->is_button_N_pressed) {
		state->keyboard_info->is_button_N_pressed = true;
		state->current_world[strlen(state->current_world)] = 'n' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_N && action == GLFW_RELEASE && state->keyboard_info->is_button_N_pressed) {
		state->keyboard_info->is_button_N_pressed = false;
	}
	
	if (key == GLFW_KEY_M && action == GLFW_PRESS && !state->keyboard_info->is_button_M_pressed) {
		state->keyboard_info->is_button_M_pressed = true;
		state->current_world[strlen(state->current_world)] = 'm' - 32*state->keyboard_info->is_button_shift_pressed;
	} else if (key == GLFW_KEY_M && action == GLFW_RELEASE && state->keyboard_info->is_button_M_pressed) {
		state->keyboard_info->is_button_M_pressed = false;
	}

}

bool check_button_click (game_state *state, BUTTON b) {
	return 	state->mouse_info->lastX >= b.vertex[0].x &&
			state->mouse_info->lastX <  b.vertex[1].x &&
			state->mouse_info->lastY >= b.vertex[0].y &&
			state->mouse_info->lastY <  b.vertex[1].y &&
			glfwGetMouseButton(state->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}



void main_menu_single_player_button (game_state* state) {
	state->state = 1;
	glfwSetKeyCallback(state->window, key_callback);
	memset (state->current_world, 0, 128);
}

int main(void)
{	

	KEYBOARD_MANAGER teclado;
	ESTADO.current_world = malloc(128);
	ESTADO.path_to_world = malloc(140);
	ESTADO.path_to_world_chunks = malloc(150);
	ESTADO.chunks_info = &test_chunks;
	ESTADO.player = &player;
	ESTADO.state = main_menu;
	ESTADO.keyboard_info = &teclado;
	ESTADO.window_width = windowWidth;
	ESTADO.window_height = windowHeight;
	
	ESTADO.keyboard_info->is_button_shift_pressed = false;
	
	for (int i = 0; i < 12; i++)
		ESTADO.block_update_functions[i] = NULL;
	
	ESTADO.block_update_functions[redstone_lamp] = redstone_lamp_update;
	ESTADO.block_update_functions[redstone_ore] = redstone_dust_update;
	ESTADO.block_update_functions[piston] = piston_update;
	
	ESTADO.buttons_info = (BUTTON**) malloc(sizeof(BUTTON*)*1);
	ESTADO.buttons_info[main_menu] = (BUTTON*) malloc(sizeof(BUTTON)*1);
	
	mouse.yaw = -90.0f;
	mouse.pitch = 0.0f;
	mouse.lastX = windowWidth/2;
	mouse.lastY = windowHeight/2;
	
	ESTADO.mouse_info = &mouse;
	
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++) {
				ESTADO.chunks_info->chunks[i][j][k].blocks = tree_init(sizeof(BLOCK), comp_blocks);
			}
	
	
	
	GLenum result = glbInit(NULL, NULL);
	
	/****************************************************/
	/* Inicialianzo o GLFW e a janela */
	
	GLFWwindow* window;

	
	if (!glfwInit()){
		printf ("Failed to initialize GLFW.\n");
		return -1;
	}
	
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, glfwGetPrimaryMonitor(), NULL);
	glfwSetWindowUserPointer(window, &ESTADO);
	ESTADO.window = window;
	
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
	
	generate_shader(&shaderProgram, "./resources/shaders/default.shader");
	generate_shader(&outlineShaderProgram, "./resources/shaders/outline.shader");
	generate_shader(&shaderProgram_2D, "./resources/shaders/2D.shader");
	
	//glDeleteProgram(shaderProgram);
	//glDeleteProgram(outlineShaderProgram);
	
	/* FIM */
	/****************************************************/
	
	

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
	
	printf ("%f\n", ((2.0 * 400 + 1.0) / windowWidth - 1.0));
	
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
	
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  
	glfwSetCursorPosCallback(window, mouseCallback);
	
	float cameraSpeed = 4.317f;
	
	int widthImg, heightImg, numColCh;

	
	
	char blockPaths[][100] = {	"dirt.png", 
								"grass.png", 
								"bricks.png", 
								"oak_log.png", 
								"leaves_oak.png",
								"redstone_block.png",
								"redstone_lamp.png",
								"redstone_lamp_on.png",
								"stone.png",
								"redstone_ore.png",
								"piston.png", 
								"piston.png"};
	char base[] = "./resources/textures/";
	
	GLuint texture1;
	for (int i = 0; i < sizeof(blockPaths)/sizeof(blockPaths[0]); i++) {
		
		char block[125];
		strcpy (block, base);
		strcat (block, blockPaths[i]);
		
		glGenTextures (1, &texture1);
		printf ("%d\n", texture1);
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
	
	ESTADO.buttons_info[main_menu][main_menu_single_player] = button_init (&ESTADO, (vec2){400., 40.}, (vec2){0.5, 0.365}, NULL, NULL, "./resources/textures/singleplayer.png", main_menu_single_player_button);
	
	BUTTON botao = button_init (&ESTADO, (vec2){400., 40.}, (vec2){0.5, 0.6}, (float[]){0., 0., 0., 0., 0., 40. / ESTADO.window_height, 0., 0.09, 400. / ESTADO.window_width, 40. / ESTADO.window_height, 0.7070, 0.09, 400. / ESTADO.window_width, 0., 0.7070, 0.}, (int[]){0, 1, 2, 0, 2, 3}, "./resources/textures/widgets.png", NULL);
	
	
	
	
	
	
	
	
	
	
	
	/*
	b.size = (vec2){400., 40.};
	b.normalized_size = (vec2){size.x / state->window_width, size.y / state->window_height};
	b.absolute_pos = (vec2){0.5, 0.7}
	b.pos = (vec4){pos.x - b.normalized_size.x / 2, pos.y - b.normalized_size.y / 2};
	b.vertex[0] = (vec2){b.pos.w * state->window_width, b.pos.x * state->window_height};
	b.vertex[1] = (vec2){b.vertex[0].x + size.x, b.vertex[0].y + size.y};
	
	glGenTextures (1, &b.texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, b.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	textureBytes = stbi_load ("./resources/textures/widgets.png", &widht_img, &height_img, &num_col_ch, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widht_img, height_img, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(textureBytes);
	*/
	
	
	/*
		BUTTON b;
	b.exec = exec;
	b.size = size;
	b.normalized_size = (vec2){size.x / state->window_width, size.y / state->window_height};
	b.absolute_pos = pos;
	b.pos = (vec4){pos.x - b.normalized_size.x / 2, pos.y - b.normalized_size.y / 2};
	b.vertex[0] = (vec2){b.pos.w * state->window_width, b.pos.x * state->window_height};
	b.vertex[1] = (vec2){b.vertex[0].x + size.x, b.vertex[0].y + size.y};
	
	printf ("%f %f\n", b.vertex[0].x, b.vertex[0].y);
	printf ("%f %f\n", b.vertex[1].x, b.vertex[1].y);
	
	glGenTextures (1, &b.texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, b.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	int widht_img, height_img, num_col_ch;
	unsigned char* textureBytes = stbi_load (texture_path, &widht_img, &height_img, &num_col_ch, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widht_img, height_img, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(textureBytes);
	
	if (vertexs && indexes) {
		
	} else {
		glGenVertexArrays(1, &b.VAO);
		glGenBuffers(1, &b.VBO);
		glGenBuffers(1, &b.EBO);
		
		glBindVertexArray(b.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, b.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b.EBO);
		
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), (float[]){0., 0., 0., 0., 0., size.y / state->window_height, 0., 1., size.x / state->window_width, size.y / state->window_height, 1., 1., size.x / state->window_width, 0., 1., 0.}, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), (int[]){0, 1, 2, 0, 2, 3}, GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	*/
	
	
	
	/*
	glBindVertexArray(b.VAO);
	glBindTexture(GL_TEXTURE_2D, b.texture);

	int pos = glGetUniformLocation(shaderProgram_2D, "p");
	glUniform4fv(pos, 1, &b.pos.w);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	*/
	
	
	
	//ESTADO.buttons_info[main_menu][main_menu_single_player] = button_init (&ESTADO, (vec2){400., 40.}, (vec2){0.5, 0.6}, NULL, NULL, "./resources/textures/widgets.png", main_menu_single_player_button);
	
	float fov = 90.f;
	
	while (!glfwWindowShouldClose(window))
	{
		do {
			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
		} while (deltaTime < 1.0 / FPS);
		lastFrame = currentFrame;

		glClearColor(0.5059f, 0.8314f, 0.9804f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if (ESTADO.state == main_menu) {
			
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  

			glUseProgram(shaderProgram_2D);
			
			draw_button (&ESTADO, ESTADO.buttons_info[main_menu][main_menu_single_player]);
			draw_button (&ESTADO, botao);
			
			if (check_button_click(&ESTADO, ESTADO.buttons_info[main_menu][main_menu_single_player])) {
				 ESTADO.buttons_info[main_menu][main_menu_single_player].exec (&ESTADO);
			}
			
			if (help > 0)
				help -= deltaTime;
			
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && help <= 0)
				glfwSetWindowShouldClose(window, 1);
			
		} else if (ESTADO.state == 1) {
			if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
				loadWorld (&ESTADO);
				glfwSetKeyCallback(window, NULL);
				ESTADO.state = 2;
			}
		} else if (ESTADO.state == 2) {
			
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
			
			player.camera.view = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
			player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
			player.camera.proj = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
			
			processMovement (window, &player, deltaTime, &test_chunks);
			
			if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
				player.currentBlock = dirt;
			
			if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
				player.currentBlock = grass;
			
			if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
				player.currentBlock = bricks;
			
			if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
				player.currentBlock = oak_log;
			
			if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
				player.currentBlock = leaves_oak;
			
			if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
				player.currentBlock = redstone_block;
			
			if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
				player.currentBlock = redstone_lamp;
			
			if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
				player.currentBlock = redstone_ore;
			
			if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
				player.currentBlock = piston;
			
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				fov += 0.1f;
			
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				fov -= 0.1f;
			
			ray r = initializeRay (player.camera.cameraPos, player.camera.cameraFront);
			
			
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && help <= 0) {
				BLOCK aux = firstBlock(&test_chunks, r);
				if (aux.id != -1) {
					printf ("%f %f %f %d %d %d %d %d %d %d %d %d %d\n", aux.pos.x, aux.pos.y, aux.pos.z, aux.id, aux.north, aux.south, aux.east, aux.west, aux.power, aux.powered, aux.power_source, aux.lit, aux.tick);
					erase_block_from_chunk (&test_chunks, aux);
				}
				help = 0.2f;
			}
			
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && help <= 0) {
				
				vec3 up = {0, 1, 0};
				vec3 down = {0, -1, 0};
				vec3 right = {1, 0, 0};
				vec3 left = {-1, 0, 0};
				vec3 front = {0, 0, 1};
				vec3 back = {0, 0, -1};
				
				BLOCK bAux = firstBlock(&test_chunks, r);
				
				vec3 aux = bAux.pos;
				BLOCK posBlock = vecToBlock (aux);
				
				if (!isBlock (relativePos(&test_chunks, sum_vec3 (aux, up)), sum_vec3 (aux, up)) && intersect(sum_vec3 (aux, up), r, 5) && player.camera.cameraPos.y > aux.y+0.5) {
					
					posBlock = vecToBlock (sum_vec3 (aux, up));
					posBlock.id = player.currentBlock;
					
				} else if (!isBlock (relativePos(&test_chunks, sum_vec3 (aux, up)), sum_vec3 (aux, down)) && intersect(sum_vec3 (aux, down), r, 5) && player.camera.cameraPos.y < aux.y-0.5) {
					
					posBlock = vecToBlock (sum_vec3 (aux, down));
					posBlock.id = player.currentBlock;
					
				} else if (!isBlock (relativePos(&test_chunks, sum_vec3 (aux, up)), sum_vec3 (aux, right)) &&  intersect(sum_vec3 (aux, right), r, 5) && player.camera.cameraPos.x > aux.x+0.5) {
					
					posBlock = vecToBlock (sum_vec3 (aux, right));
					posBlock.id = player.currentBlock;
					
				} else if (!isBlock (relativePos(&test_chunks, sum_vec3 (aux, up)), sum_vec3 (aux, left)) &&  intersect(sum_vec3 (aux, left), r, 5) && player.camera.cameraPos.x < aux.x-0.5) {
					
					posBlock = vecToBlock (sum_vec3 (aux, left));
					posBlock.id = player.currentBlock;
				 
				} else if (!isBlock (relativePos(&test_chunks, sum_vec3 (aux, up)), sum_vec3 (aux, front)) &&  intersect(sum_vec3 (aux, front), r, 5) && player.camera.cameraPos.z > aux.z+0.5) {
					
					posBlock = vecToBlock (sum_vec3 (aux, front));
					posBlock.id = player.currentBlock;
				
				} else if (!isBlock (relativePos(&test_chunks, sum_vec3 (aux, up)), sum_vec3 (aux, back)) &&  intersect(sum_vec3 (aux, back), r, 5) && player.camera.cameraPos.z < aux.z-0.5) {
					
					posBlock = vecToBlock (sum_vec3 (aux, back));
					posBlock.id = player.currentBlock;
					
				}
				
				
				if (posBlock.id == 6) {
					posBlock.power = 15;
					posBlock.powered = true;
					posBlock.power_source = true;
				}
				
				if (posBlock.id == 9) {
					if (!block_down(test_chunks.chunks[1][1][1].blocks, posBlock)) 
						posBlock.id = -1;
				}
				
				if (posBlock.id != -1) {
					insert_block_from_chunk (&test_chunks, posBlock);
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
				
				player.collisionbox.vertex[0] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, -0.3});
				player.collisionbox.vertex[1] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, -0.3});
				player.collisionbox.vertex[2] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, 0.3});
				player.collisionbox.vertex[3] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, 0.3});
				player.collisionbox.vertex[4] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, -0.3});
				player.collisionbox.vertex[5] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, -0.3});
				player.collisionbox.vertex[6] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, 0.3});
				player.collisionbox.vertex[7] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, 0.3});
			}
			cameraSpeed = 4.317f;
			
			player.camera.view = lookAt_mat4 (player.camera.cameraPos, sum_vec3 (player.camera.cameraPos, player.camera.cameraFront), player.camera.cameraUp);
			
			player.camera.proj = perspective_mat4 ((float)degToRad(fov), (float)windowWidth/windowHeight, 0.1f, 1600.0f);
			
			GLuint shaders[] = {shaderProgram, outlineShaderProgram};
			
			drawALL (&ESTADO, shaders, &VAOID[0], r);
			
			
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				saveWorld (&ESTADO);
				ESTADO.state = 0;
				help = 0.2f;
			}
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

TREE *relativePos (CHUNK_MANAGER *chunks, vec3 pos) {
	vec3 aux;
	aux.x = pos.x>=0?(int)pos.x/64:(int)pos.x/64-1;
	aux.y = pos.y>=0?(int)pos.y/64:(int)pos.y/64-1;
	aux.z = pos.z>=0?(int)pos.z/64:(int)pos.z/64-1;
	
	if (aux.x < chunks->currentPos.x-1 || aux.x > chunks->currentPos.x+1)
		return NULL;
	if (aux.y < chunks->currentPos.y-1 || aux.y > chunks->currentPos.y+1)
		return NULL;
	if (aux.z < chunks->currentPos.z-1 || aux.z > chunks->currentPos.z+1)
		return NULL;
	
	return chunks->chunks[(int)(aux.x+1 - chunks->currentPos.x)][(int)(aux.y+1 - chunks->currentPos.y)][(int)(aux.z+1 - chunks->currentPos.z)].blocks;
}


void drawBlockOutline (BLOCK T, GLuint shader, GLuint VAOID[]);
void drawBlocks (CHUNK_MANAGER *chunks, GLuint shader, GLuint VAOID[]);

void drawALL (game_state *state, GLuint *shaders, GLuint *VAOID, ray r) {
	BLOCK first = firstBlock (state->chunks_info, initializeRay(state->player->camera.cameraPos, state->player->camera.cameraFront));
	if (first.id != -1)
		drawBlockOutline (first, shaders[1], VAOID);
	
	drawBlocks (state->chunks_info, shaders[0], VAOID);
}

void drawAllBlocks (TREE_NODE* node, CHUNK_MANAGER *chunks, GLuint shader, GLuint VAOID[]){
	if (!node)
		return;
	
	BLOCK data = *(BLOCK*)node->data;
	
	player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.model = translate_mat4_vec3 (player.camera.model, data.pos);
	
	int modelLoc = glGetUniformLocation(shader, "model");
	glBindVertexArray(VAOID[0]);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &player.camera.model.ww);
	
	if (ESTADO.block_update_functions[data.id])
		ESTADO.block_update_functions[data.id]((BLOCK*)node->data, chunks);
	
	glBindTexture(GL_TEXTURE_2D, data.id+data.lit);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	
	drawAllBlocks (node->left, chunks, shader, VAOID);
	drawAllBlocks (node->right, chunks, shader, VAOID);
}

void drawBlocks (CHUNK_MANAGER *chunks, GLuint shader, GLuint VAOID[]){
	glUseProgram(shader);
	
	int viewLoc = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &player.camera.view.ww);
	int projLoc = glGetUniformLocation(shader, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &player.camera.proj.ww);
	
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				drawAllBlocks (chunks->chunks[i][j][k].blocks->root, chunks, shader, VAOID);
			}
		}
	}
}

void drawBlockOutline (BLOCK T, GLuint shader, GLuint VAOID[]) {
	
	glUseProgram(shader);
	
	int viewLoc = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &player.camera.view.ww);
	
	int projLoc = glGetUniformLocation(shader, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &player.camera.proj.ww);
	
	
	player.camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	player.camera.model = translate_mat4_vec3 (player.camera.model, T.pos);
	
	
	int modelLoc = glGetUniformLocation(shader, "model");
	glBindVertexArray(outlineVAO[0]);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &player.camera.model.ww);
	
	//glClear(GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 144, GL_UNSIGNED_INT, 0);
}

BLOCK firstBlock (CHUNK_MANAGER *chunks, ray r) {
	vec3 base = (vec3){round(r.origin.x), round(r.origin.y), round(r.origin.z)};
	vec3 aux = base;
	BLOCK *b;

	for (int i = 0; i < 1331; i++, aux = base) {
		aux = sum_vec3(aux, blocksInRange[i]);
		b = get_block_from_chunk (chunks, vecToBlock(aux));
		if (intersect(aux, r, 5) && b && b->id != -1) {
			return *b;
		}
	}
	
	return (BLOCK){-1, -1, -1, -1};
}

float jumpSpeed = 8.5;

bool isPressedSpace = false;


void processMovement (GLFWwindow* window, Player *player, float deltaTime, CHUNK_MANAGER *chunks) {
	
	float a = 8.5;
	
	vec3 beforeChunk = cordToChunkCord (player->camera.cameraPos);
	
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
			
			
			
			if (!detectCollisionPlayer (window, player, (vec3){0, 1, 0}, deltaTime, player->VSpeed, chunks)) {
				player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
				for (int i = 0; i < 8; i++)
					player->collisionbox.vertex[i] = sum_vec3 (player->collisionbox.vertex[i], scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
			} else {

				for (int i = 0; i < 4; i++) {
					player->collisionbox.vertex[i].y = round(player->collisionbox.vertex[i].y);
				}
				
				for (int i = 4; i < 8; i++) {
					player->collisionbox.vertex[i].y = player->collisionbox.vertex[0].y+1.8;
				}

				player->camera.cameraPos.y = player->collisionbox.vertex[0].y + 1.62;

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

			
			
			
			if (!detectCollisionPlayer (window, player, (vec3){0, 1, 0}, deltaTime, player->VSpeed, chunks)) {
				player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
				for (int i = 0; i < 8; i++)
					player->collisionbox.vertex[i] = sum_vec3 (player->collisionbox.vertex[i], scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
			} else {

				for (int i = 0; i < 4; i++) {
					player->collisionbox.vertex[i].y = round(player->collisionbox.vertex[i].y);
				}
				
				for (int i = 4; i < 8; i++) {
					player->collisionbox.vertex[i].y = player->collisionbox.vertex[0].y+1.8;
				}

				player->camera.cameraPos.y = player->collisionbox.vertex[0].y + 1.62;

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
				
				if (!detectCollisionPlayer (window, player, (vec3){0, 1, 0}, deltaTime, player->VSpeed, chunks)) {
					player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
					for (int i = 0; i < 8; i++)
						player->collisionbox.vertex[i] = sum_vec3 (player->collisionbox.vertex[i], scale_vec3 (player->camera.cameraUp, player->VSpeed * deltaTime));
				} else
					player->VSpeed = 0;
			} else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && isPressedSpace)
				isPressedSpace = false;
			
			if (!detectCollisionPlayer (window, player, (vec3){0, 1, 0}, deltaTime, -1, chunks)) {
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
	

	
	
	
	if (player->isMoving) {
		
		if (!player->HSpeed)
			player->HSpeed = PLAYER_WALKING_SPEED;

		direction = scale_vec3 (normalize_vec3 (direction), player->HSpeed * deltaTime);

		if (!detectCollisionPlayer (window, player, (vec3){direction.x, 0, 0}, deltaTime, 1, chunks)) {
			player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, (vec3){direction.x, 0, 0});
			for (int i = 0; i < 8; i++)
				player->collisionbox.vertex[i] = sum_vec3 (player->collisionbox.vertex[i], (vec3){direction.x, 0, 0});
		}
		
		if (!detectCollisionPlayer (window, player, (vec3){0, 0, direction.z}, deltaTime, 1, chunks)) {
			player->camera.cameraPos = sum_vec3 (player->camera.cameraPos, (vec3){0, 0, direction.z});
			for (int i = 0; i < 8; i++)
				player->collisionbox.vertex[i] = sum_vec3 (player->collisionbox.vertex[i], (vec3){0, 0, direction.z});
		}
		
		
	} else {
		player->HSpeed = 0;
	}
	
	
	vec3 afterChunk = cordToChunkCord (player->camera.cameraPos);
	
	
	beforeChunk.x = round(beforeChunk.x);
	beforeChunk.y = round(beforeChunk.y);
	beforeChunk.z = round(beforeChunk.z);
	
	afterChunk.x = round(afterChunk.x);
	afterChunk.y = round(afterChunk.y);
	afterChunk.z = round(afterChunk.z);
	
	
	if (afterChunk.x < beforeChunk.x)
		moveChunkX (chunks, -1);
	
	if (afterChunk.x > beforeChunk.x)
		moveChunkX (chunks, 1);
	
	if (afterChunk.y < beforeChunk.y)
		moveChunkY (chunks, 1);
	
	if (afterChunk.y > beforeChunk.y)
		moveChunkY (chunks, -1);
	
	if (afterChunk.z < beforeChunk.z)
		moveChunkZ (chunks, 1);
	
	if (afterChunk.z > beforeChunk.z)
		moveChunkZ (chunks, -1);
}




bool detectCollisionPlayer (GLFWwindow* window, Player *player, vec3 movement, float deltaTime, float speed, CHUNK_MANAGER *chunks) {
	
	return isBlock (relativePos(chunks, player->collisionbox.vertex[0]), sum_vec3 (player->collisionbox.vertex[0], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (relativePos(chunks, player->collisionbox.vertex[1]), sum_vec3 (player->collisionbox.vertex[1], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (relativePos(chunks, player->collisionbox.vertex[2]), sum_vec3 (player->collisionbox.vertex[2], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (relativePos(chunks, player->collisionbox.vertex[3]), sum_vec3 (player->collisionbox.vertex[3], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (relativePos(chunks, player->collisionbox.vertex[4]), sum_vec3 (player->collisionbox.vertex[4], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (relativePos(chunks, player->collisionbox.vertex[5]), sum_vec3 (player->collisionbox.vertex[5], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (relativePos(chunks, player->collisionbox.vertex[6]), sum_vec3 (player->collisionbox.vertex[6], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)))
		|| isBlock (relativePos(chunks, player->collisionbox.vertex[7]), sum_vec3 (player->collisionbox.vertex[7], scale_vec3 (normalize_vec3 (movement), speed * deltaTime)));
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
	game_state *state = (game_state*)glfwGetWindowUserPointer(window); 
  
    float xoffset = xpos - state->mouse_info->lastX;
    float yoffset = state->mouse_info->lastY - ypos; 
    state->mouse_info->lastX = xpos;
    state->mouse_info->lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    state->mouse_info->yaw   += xoffset;
    state->mouse_info->pitch += yoffset;

    if(state->mouse_info->pitch > 89.9f)
        state->mouse_info->pitch = 89.9f;
    if(state->mouse_info->pitch < -89.9f)
        state->mouse_info->pitch = -89.9f;
	
	state->player->camera.cameraFront.x = cos(degToRad(state->mouse_info->yaw)) * cos(degToRad(state->mouse_info->pitch));
	state->player->camera.cameraFront.y = sin(degToRad(state->mouse_info->pitch));
	state->player->camera.cameraFront.z = sin(degToRad(state->mouse_info->yaw)) * cos(degToRad(state->mouse_info->pitch));
	state->player->camera.cameraFront = normalize_vec3 (player.camera.cameraFront);
}

int closestToZero (vec3 first, vec3 second) {
	
	int aux = abs(first.x) + abs(first.y) + abs(first.z);
	int aux2 = abs(second.x) + abs(second.y) + abs(second.z);
	
	return abs(aux) < abs(aux2);
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

BLOCK vecToBlock (vec3 T) {
	return (BLOCK){(int)floor(T.x), (int)floor(T.y), (int)floor(T.z), -1};
}

int isBlock (TREE *tree, vec3 pos) {
	return (int)get_block(tree, vecToBlock(pos));
}

void fillSkyBlock () {
	
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){0, 0, 0}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){1, 0, 0}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){-1, 0, 0}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){0, 0, 1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){1, 0, 1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){-1, 0, 1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){0, 0, -1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){1, 0, -1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){-1, 0, -1}, 1});
	
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){0, -1, 0}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){1, -1, 0}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){-1, -1, 0}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){0, -1, 1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){1, -1, 1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){-1, -1, 1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){0, -1, -1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){1, -1, -1}, 1});
	insert_block_from_chunk (&test_chunks, (BLOCK){(vec3){-1, -1, -1}, 1});
	
}

void saveWorld (game_state *state) {
	saveChunks (state->chunks_info);
}

void loadWorld (game_state *state) {
	sprintf (state->path_to_world, "saves/%s", state->current_world);
	sprintf (state->path_to_world_chunks, "saves/%s/chunks", state->current_world);
	
	
	state->chunks_info->currentPos = (vec3){0, 0, 0};
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++) {
				state->chunks_info->chunks[i][j][k].pos.x = i-1;
				state->chunks_info->chunks[i][j][k].pos.y = j-1;
				state->chunks_info->chunks[i][j][k].pos.z = k-1;
				clear_chunk (state->chunks_info->chunks[i][j][k].blocks);
			}
	
	
	
	state->player->camera.cameraPos = (vec3){0.5, 3.82, 0.5};
	state->player->camera.cameraFront = (vec3){0.0, 0.0, -1.0};
	state->player->camera.cameraUp = (vec3){0.0, 1.0, 0.0};
	state->player->camera.direction = (vec3){0.0, 0.0, 0.0};
	state->player->camera.view = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	state->player->camera.model = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	state->player->camera.proj = (mat4){(vec4){1.0, 0.0, 0.0, 0.0}, (vec4){0.0, 1.0, 0.0, 0.0}, (vec4){0.0, 0.0, 1.0, 0.0}, (vec4){0.0, 0.0, 0.0, 1.0}};
	
	state->player->HSpeed = 4.317f;
	state->player->VSpeed = 0.f;
	state->player->onGround = true;
	state->player->isFlying = false;
	
	state->player->collisionbox.vertex[0] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, -0.3});
	state->player->collisionbox.vertex[1] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, -0.3});
	state->player->collisionbox.vertex[2] = sum_vec3(player.camera.cameraPos, (vec3){0.3, -1.62, 0.3});
	state->player->collisionbox.vertex[3] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, -1.62, 0.3});
	state->player->collisionbox.vertex[4] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, -0.3});
	state->player->collisionbox.vertex[5] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, -0.3});
	state->player->collisionbox.vertex[6] = sum_vec3(player.camera.cameraPos, (vec3){0.3, 0.18, 0.3});
	state->player->collisionbox.vertex[7] = sum_vec3(player.camera.cameraPos, (vec3){-0.3, 0.18, 0.3});
	
	struct stat st = {0};
	if (stat(state->path_to_world, &st) == -1) {
		mkdir(state->path_to_world, 0777);
		mkdir(state->path_to_world_chunks, 0777	);
		fillSkyBlock();
	}
	
	loadChunks (state->chunks_info);
}

void saveChunks (CHUNK_MANAGER *chunks) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				saveChunk(&chunks->chunks[i][j][k]);
			}
		}
	}
}



void saveChunk (CHUNK *chunk) {
	
	char *filePath = malloc(200);

	sprintf (filePath, "saves/%s/chunks/chunk%d%d%d.map", ESTADO.current_world, (int)chunk->pos.x, (int)chunk->pos.y, (int)chunk->pos.z);
	
	printf ("%s\n", filePath);
	
	FILE *p = fopen (filePath, "w");
	if (!p)
		return;
	
	saveBlocks (chunk->blocks->root, p);
	fclose(p);
	
	free (filePath);
}

void saveBlocks (TREE_NODE* node, FILE *p) {
    if (!node)
        return;

	BLOCK data = *(BLOCK*)node->data;
	
    fprintf(p, "%f %f %f %d %hd %hd %hd %hd %hd %d %d %d %d \n", data.pos.x, data.pos.y, data.pos.z, data.id, data.north, data.south, data.east, data.west, data.power, data.powered, data.power_source, data.lit, data.tick);
    
    print_block(data);
    
    saveBlocks (node->left, p);
    saveBlocks (node->right, p);
}

void loadChunks (CHUNK_MANAGER *chunks) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				loadChunk(&chunks->chunks[i][j][k]);
			}
		}
	}
}

void loadChunk (CHUNK *chunk) {
	char *filePath = malloc(200);

	sprintf (filePath, "saves/%s/chunks/chunk%d%d%d.map", ESTADO.current_world, (int)chunk->pos.x, (int)chunk->pos.y, (int)chunk->pos.z);
	
	//printf ("%s\n", filePath);
	
	FILE *p = fopen (filePath, "r");
	if (!p)
		return;
	
	vec3 pos;
    int id;

	BLOCK b;
    while (fscanf (p, "%f %f %f %d %hd %hd %hd %hd %hd %d %d %d %d%*c", &b.pos.x, &b.pos.y, &b.pos.z, &b.id, &b.north, &b.south, &b.east, &b.west, &b.power, &b.powered, &b.power_source, &b.lit, &b.tick) != EOF) {		
        insert_block (chunk->blocks, b);
	}
}


void moveChunkX (CHUNK_MANAGER *chunks, int x) {
	
	if (x < 0) {
		printf ("a\n");
		chunks->currentPos.x--;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				TREE *aux = chunks->chunks[2][i][j].blocks;
				clear_chunk (chunks->chunks[2][i][j].blocks);
				chunks->chunks[2][i][j] = chunks->chunks[1][i][j];
				chunks->chunks[1][i][j] = chunks->chunks[0][i][j];
				chunks->chunks[0][i][j].blocks = aux;
				chunks->chunks[0][i][j].pos.x--;
				loadChunk (&chunks->chunks[0][i][j]);
			}
		}
	} else {
		printf ("b\n");
		chunks->currentPos.x++;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				TREE *aux = chunks->chunks[0][i][j].blocks;
				clear_chunk (chunks->chunks[0][i][j].blocks);
				chunks->chunks[0][i][j] = chunks->chunks[1][i][j];
				chunks->chunks[1][i][j] = chunks->chunks[2][i][j];
				chunks->chunks[2][i][j].blocks = aux;
				chunks->chunks[2][i][j].pos.x++;
				loadChunk (&chunks->chunks[2][i][j]);
			}
		}
	}

}

void moveChunkY (CHUNK_MANAGER *chunks, int y) {
	if (y >= 0) {
		chunks->currentPos.y--;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				TREE *aux = chunks->chunks[i][2][j].blocks;
				clear_chunk (chunks->chunks[i][2][j].blocks);
				chunks->chunks[i][2][j] = chunks->chunks[i][1][j];
				chunks->chunks[i][1][j] = chunks->chunks[i][0][j];
				chunks->chunks[i][0][j].blocks = aux;
				chunks->chunks[i][0][j].pos.y--;
				loadChunk (&chunks->chunks[i][0][j]);
			}
		}
	} else {
		chunks->currentPos.y++;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				TREE *aux = chunks->chunks[i][0][j].blocks;
				clear_chunk (chunks->chunks[i][0][j].blocks);
				chunks->chunks[i][0][j] = chunks->chunks[i][1][j];
				chunks->chunks[i][1][j] = chunks->chunks[i][2][j];
				chunks->chunks[i][2][j].blocks = aux;
				chunks->chunks[i][2][j].pos.y++;
				loadChunk (&chunks->chunks[i][2][j]);
			}
		}
	}
}

void moveChunkZ (CHUNK_MANAGER *chunks, int z) {
	if (z >= 0) {
		chunks->currentPos.z--;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				TREE *aux = chunks->chunks[i][j][2].blocks;
				clear_chunk (chunks->chunks[i][j][2].blocks);
				chunks->chunks[i][j][2] = chunks->chunks[i][j][1];
				chunks->chunks[i][j][1] = chunks->chunks[i][j][0];
				chunks->chunks[i][j][0].blocks = aux;
				chunks->chunks[i][j][0].pos.z--;
				loadChunk (&chunks->chunks[i][j][0]);
			}
		}
	} else {
		chunks->currentPos.z++;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				TREE *aux = chunks->chunks[i][j][0].blocks;
				clear_chunk (chunks->chunks[i][j][0].blocks);
				chunks->chunks[i][j][0] = chunks->chunks[i][j][1];
				chunks->chunks[i][j][1] = chunks->chunks[i][j][2];
				chunks->chunks[i][j][2].pos.z++;
				chunks->chunks[i][j][2].blocks = aux;
				loadChunk (&chunks->chunks[i][j][2]);
			}
		}
	}
}


vec3 cordToChunkCord (vec3 pos) {
	vec3 r;
	r.x = pos.x>=0?(int)pos.x/64:(int)pos.x/64-1;
	r.y = pos.y>=0?(int)pos.y/64:(int)pos.y/64-1;
	r.z = pos.z>=0?(int)pos.z/64:(int)pos.z/64-1;
	return r;
}
