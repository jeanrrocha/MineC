#include "mat.h"
#include <stdio.h>
#include <math.h>

mat4 identity4x4 (){
	mat4 identity;
	identity.matrix[0][0] = 1;
	identity.matrix[0][1] = 0;
	identity.matrix[0][2] = 0;
	identity.matrix[0][3] = 0;
	identity.matrix[1][0] = 0;
	identity.matrix[1][1] = 1;
	identity.matrix[1][2] = 0;
	identity.matrix[1][3] = 0;
	identity.matrix[2][0] = 0;
	identity.matrix[2][1] = 0;
	identity.matrix[2][2] = 1;
	identity.matrix[2][3] = 0;
	identity.matrix[3][0] = 0;
	identity.matrix[3][1] = 0;
	identity.matrix[3][2] = 0;
	identity.matrix[3][3] = 1;
	return identity;
}

mat4 multiply4x4 (mat4 src1, mat4 src2){
	mat4 dest;
	dest.matrix[0][0] = src1.matrix[0][0] * src2.matrix[0][0] + src1.matrix[0][1] * src2.matrix[1][0] + src1.matrix[0][2] * src2.matrix[2][0] + src1.matrix[0][3] * src2.matrix[3][0];
	dest.matrix[0][1] = src1.matrix[0][0] * src2.matrix[0][1] + src1.matrix[0][1] * src2.matrix[1][1] + src1.matrix[0][2] * src2.matrix[2][1] + src1.matrix[0][3] * src2.matrix[3][1];
	dest.matrix[0][2] = src1.matrix[0][0] * src2.matrix[0][2] + src1.matrix[0][1] * src2.matrix[1][2] + src1.matrix[0][2] * src2.matrix[2][2] + src1.matrix[0][3] * src2.matrix[3][2];
	dest.matrix[0][3] = src1.matrix[0][0] * src2.matrix[0][3] + src1.matrix[0][1] * src2.matrix[1][3] + src1.matrix[0][2] * src2.matrix[2][3] + src1.matrix[0][3] * src2.matrix[3][3];
	dest.matrix[1][0] = src1.matrix[1][0] * src2.matrix[0][0] + src1.matrix[1][1] * src2.matrix[1][0] + src1.matrix[1][2] * src2.matrix[2][0] + src1.matrix[1][3] * src2.matrix[3][0];
	dest.matrix[1][1] = src1.matrix[1][0] * src2.matrix[0][1] + src1.matrix[1][1] * src2.matrix[1][1] + src1.matrix[1][2] * src2.matrix[2][1] + src1.matrix[1][3] * src2.matrix[3][1];
	dest.matrix[1][2] = src1.matrix[1][0] * src2.matrix[0][2] + src1.matrix[1][1] * src2.matrix[1][2] + src1.matrix[1][2] * src2.matrix[2][2] + src1.matrix[1][3] * src2.matrix[3][2];
	dest.matrix[1][3] = src1.matrix[1][0] * src2.matrix[0][3] + src1.matrix[1][1] * src2.matrix[1][3] + src1.matrix[1][2] * src2.matrix[2][3] + src1.matrix[1][3] * src2.matrix[3][3];
	dest.matrix[2][0] = src1.matrix[2][0] * src2.matrix[0][0] + src1.matrix[2][1] * src2.matrix[1][0] + src1.matrix[2][2] * src2.matrix[2][0] + src1.matrix[2][3] * src2.matrix[3][0];
	dest.matrix[2][1] = src1.matrix[2][0] * src2.matrix[0][1] + src1.matrix[2][1] * src2.matrix[1][1] + src1.matrix[2][2] * src2.matrix[2][1] + src1.matrix[2][3] * src2.matrix[3][1];
	dest.matrix[2][2] = src1.matrix[2][0] * src2.matrix[0][2] + src1.matrix[2][1] * src2.matrix[1][2] + src1.matrix[2][2] * src2.matrix[2][2] + src1.matrix[2][3] * src2.matrix[3][2];
	dest.matrix[2][3] = src1.matrix[2][0] * src2.matrix[0][3] + src1.matrix[2][1] * src2.matrix[1][3] + src1.matrix[2][2] * src2.matrix[2][3] + src1.matrix[2][3] * src2.matrix[3][3];
	dest.matrix[3][0] = src1.matrix[3][0] * src2.matrix[0][0] + src1.matrix[3][1] * src2.matrix[1][0] + src1.matrix[3][2] * src2.matrix[2][0] + src1.matrix[3][3] * src2.matrix[3][0];
	dest.matrix[3][1] = src1.matrix[3][0] * src2.matrix[0][1] + src1.matrix[3][1] * src2.matrix[1][1] + src1.matrix[3][2] * src2.matrix[2][1] + src1.matrix[3][3] * src2.matrix[3][1];
	dest.matrix[3][2] = src1.matrix[3][0] * src2.matrix[0][2] + src1.matrix[3][1] * src2.matrix[1][2] + src1.matrix[3][2] * src2.matrix[2][2] + src1.matrix[3][3] * src2.matrix[3][2];
	dest.matrix[3][3] = src1.matrix[3][0] * src2.matrix[0][3] + src1.matrix[3][1] * src2.matrix[1][3] + src1.matrix[3][2] * src2.matrix[2][3] + src1.matrix[3][3] * src2.matrix[3][3];
	return dest;
}

mat4 matMultiply4xCol (mat4 srcMat, vec4 srcVec){
	mat4 m = identity4x4();
	m.matrix[0][0] = srcVec.vector[0];
	m.matrix[1][1] = srcVec.vector[1];
	m.matrix[2][2] = srcVec.vector[2];
	m.matrix[3][3] = srcVec.vector[3];
	
	return multiply4x4(srcMat, m);
}

vec4 vecMultiply4xCol (mat4 srcMat, vec4 srcVec){
	mat4 m = identity4x4();
	m.matrix[0][0] = srcVec.vector[0];
	m.matrix[1][1] = srcVec.vector[1];
	m.matrix[2][2] = srcVec.vector[2];
	m.matrix[3][3] = srcVec.vector[3];
	
	m = multiply4x4(srcMat, m);
	
	return (vec4){m.matrix[0][0], m.matrix[1][1], m.matrix[2][2], m.matrix[3][3]};
}

mat4 transform (mat4 srcMat, vec3 srcVec){
	return matMultiply4xCol (srcMat, (vec4){srcVec.vector[0], srcVec.vector[1], srcVec.vector[2], 1});
}

mat4 rotation (mat4 srcMat, float radians, vec3 srcVec){
	
	if (srcVec.vector[2] != 0){
	
	} else if (srcVec.vector[1] != 0){
	
	} else if (srcVec.vector[0] != 0){
	
	}
}

mat4 lookAt (vec3 eye, vec3 center, vec3 up){
	vec3 f = normalize((vec3){(center.vector[0] - eye.vector[0]), (center.vector[1] - eye.vector[1]), (center.vector[2] - eye.vector[2])});
	vec3 u = normalize(up);
	vec3 s = normalize(cross(f, u));
	u = cross(s, f);
	
	mat4 result = identity4x4();
	
	result.matrix[0][0] = s.vector[0];
	result.matrix[1][0] = s.vector[1];
	result.matrix[2][0] = s.vector[2];
	result.matrix[0][1] = u.vector[0];
	result.matrix[1][1] = u.vector[1];
	result.matrix[2][1] = u.vector[2];
	result.matrix[0][2] =-f.vector[0];
	result.matrix[1][2] =-f.vector[1];
	result.matrix[2][2] =-f.vector[2];
	result.matrix[3][0] =-dot(s, eye);
	result.matrix[3][1] =-dot(u, eye);
	result.matrix[3][2] = dot(f, eye);
	return result;
}

vec3 normalize (vec3 src){
	float w = sqrt(src.vector[0] * src.vector[0] + src.vector[1] * src.vector[1] + src.vector[2] * src.vector[2]);
	src.vector[0] /= w;
	src.vector[1] /= w;
	src.vector[2] /= w;
	return src;
}

vec3 cross (vec3 src1, vec3 src2){
	return (vec3){(src1.vector[1] * src2.vector[2] - src1.vector[2] * src2.vector[1]), (src1.vector[2] * src2.vector[0] - src1.vector[0] * src2.vector[2]), (src1.vector[0] * src2.vector[1] - src1.vector[1] * src2.vector[0])};
}

float dot (vec3 src1, vec3 src2){
	return (src1.vector[0] * src2.vector[0] + src1.vector[1] * src2.vector[1] + src1.vector[2] * src2.vector[2]);
}

void printVec3 (vec3 src){
	printf ("%.2f %.2f %.2f\n", src.vector[0], src.vector[1], src.vector[2]);
}
void printMat4 (mat4 src){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++)
			printf ("%.2f ", src.matrix[i][j]);
		printf ("\n");
	}
}
