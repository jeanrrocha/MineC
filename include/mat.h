#ifndef MAT_H
#define MAT_H

typedef float GLfloat;

typedef struct {
	union {
		GLfloat col[2];
		struct {
			GLfloat x;
			GLfloat y;
		};
	};
} vec2;

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
		int col[3];
		struct {
			int x;
			int y;
			int z;
		};
	};
} vec3Int;

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

vec3 scale_vec3 (vec3 vector, double x);
vec4 scale_vec4 (vec4 vector, double x);
vec3 sum_vec3 (vec3 vector1, vec3 vector2);
vec4 sum_vec4 (vec4 vector1, vec4 vector2);
vec3 sub_vec3 (vec3 vector1, vec3 vector2);
vec4 sub_vec4 (vec4 vector1, vec4 vector2);
vec3 multiply_vec3 (vec3 vector1, vec3 vector2);
vec4 multiply_vec4 (vec4 vector1, vec4 vector2);
double dot_vec3 (vec3 vector1, vec3 vector2);
double dot_vec4 (vec4 vector1, vec4 vector2);
double norm_vec3 (vec3 vector);
double norm_vec4 (vec4 vector);
vec3 normalize_vec3 (vec3 vector);
vec4 normalize_vec4 (vec4 vector);
vec3 cross_vec3 (vec3 vector1, vec3 vector2);
mat4 multiply_mat4 (mat4 matrix1, mat4 matrix2);
mat4 lookAt_mat4 (vec3 from, vec3 to, vec3 up);
mat4 perspective_mat4 (float angle, float ratio, float distNear, float distFar);
mat4 translate_mat4_vec3 (mat4 matrix, vec3 vector);


#endif