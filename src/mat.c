#include "mat.h"
#include <stdio.h>
#include <math.h>

#include "types.h"

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
