#ifndef MAT_H
#define MAT_H

	typedef struct {
		float matrix[4][4];
	}mat4;
	
	typedef struct {
		float vector[3];
	} vec3;
	
	typedef struct {
		float vector[4];
	} vec4;
	
	/*Mat.h

	Normalize
	Cross
	Multiply4x4
	Multiplay4xCol
	Scale
	Rotate
	*/
	
	
	
	mat4 identity4x4 ();
	
	mat4 multiply4x4 (mat4, mat4);
	
	mat4 matMultiply4xCol (mat4, vec4);
	vec4 vecMultiply4xCol (mat4, vec4);
	
	mat4 transform (mat4, vec3);
	
	mat4 rotation (mat4, float, vec3);
	
	mat4 lookAt (vec3, vec3, vec3);
	
	vec3 normalize (vec3);
	
	vec3 cross (vec3, vec3);
	
	float dot (vec3, vec3);
	
	void printVec3 (vec3);
	void printMat4 (mat4);
	
	
	
#endif
