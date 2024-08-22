#ifndef MAT_H
#define MAT_H

#include "types.h"

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