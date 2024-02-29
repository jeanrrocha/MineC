#include <stdio.h>
#include <math.h>
#include <x86intrin.h>
#include <stdbool.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

typedef struct {
	union {
		double col[3];
		struct {
			double x;
			double y;
			double z;
		};
	};
} vec3;

typedef struct {
	union {
		double col[4];
		struct {
			double w;
			double x;
			double y;
			double z;
		};
	};
} vec4;

typedef struct {
	
	union {
		vec4 row[4];
		struct {
			double ww;
			double wx;
			double wy;
			double wz;
			double xw;
			double xx;
			double xy;
			double xz;
			double yw;
			double yx;
			double yy;
			double yz;
			double zw;
			double zx;
			double zy;
			double zz;
		};
		struct {
			vec4 w;
			vec4 x;
			vec4 y;
			vec4 z;
		};
	};
} mat4;

bool compDouble (double a, double b, uint64_t decimal) {
	if (roundf(a * 10 * decimal) / (10 * decimal) == roundf (b * 10 * decimal) / (10 * decimal))
		return true;
	return false;
}


vec3 scale_vec3 (vec3 vector, double x) {
	
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

vec3 sub_vec3 (vec3 vector1, vec3 vector2) {
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


double dot_vec3 (vec3 vector1, vec3 vector2) {
	return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

double norm_vec3 (vec3 vector) {
	return sqrt (dot_vec3 (vector, vector));
}

vec3 normalize_vec3 (vec3 vector) {
	return scale_vec3 (vector, 1 / norm_vec3 (vector));
}

vec3 cross_vec3 (vec3 vector1, vec3 vector2) {
	vec3 out;
	
	out.x = vector1.y*vector2.z - vector1.z*vector2.y;
	out.y = vector1.z*vector2.x - vector1.x*vector2.z;
	out.z = vector1.x*vector2.y - vector1.y*vector2.x;
	
	return out;
}

mat4 lookAt_mat4 (vec3 a, vec3 b) {
	mat4 out;
	
	out.row[2] = a;
	
}

void benchmarkSCALE(uint64_t t);
void benchmarkSUM(uint64_t t);
//multiply
void benchmarkDOT(uint64_t t);
void benchmarkNORM(uint64_t t);
void benchmarkNORMALIZE(uint64_t t);
void benchmarkCROSS(uint64_t t);
void benchmarkLOOKAT(uint64_t t);


int main () {
	
	/*
	
	benchmarkSCALE (30000000);
	benchmarkSUM (30000000);
	benchmarkDOT (30000000);
	benchmarkNORM (30000000);
	benchmarkNORMALIZE (30000000);
	
	//*/
	
	benchmarkCROSS (30000000);

	return 0;
}





void benchmarkSCALE(uint64_t t) {
	vec3 a = (vec3){25, 50, 75};
	glm::vec3 b = glm::vec3(25, 50, 75);
	
	uint64_t avg = 0, min = -1, max = 0, it = t;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		a = scale_vec3 (a, 1);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\nscale_vec3\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	avg = 0, min = -1, max = 0;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		b *= 1;
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\nscale_glm\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	a = scale_vec3 (a, 5);
	b *= 5;
	
	if (a.x == b.x && a.y == b.y && a.z == b.z)
		printf ("OK\n\n\n");
	else
		printf ("ERROR - DIFFER\n\n\n");
	
	
}





void benchmarkSUM(uint64_t t) {
	vec3 a = (vec3){25, 50, 75};
	glm::vec3 b = glm::vec3(25, 50, 75);
	
	vec3 aux1 = (vec3){0, 0, 0};
	glm::vec3 aux2 = glm::vec3(0, 0, 0);
	
	uint64_t avg = 0, min = -1, max = 0, it = t;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		a = sum_vec3 (a, aux1);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\nsum_vec3\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	avg = 0, min = -1, max = 0;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		b += aux2;
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\nsum_glm\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	a = (vec3){25, 50, 75};
	b = glm::vec3(25, 50, 75);
	
	aux1.x = 100;
	aux1.y = 1000;
	aux1.z = 10000;
	
	aux2.x = 100;
	aux2.y = 1000;
	aux2.z = 10000;
	
	a = sum_vec3 (a, aux1);
	b += aux2;
	
	if (a.x == b.x && a.y == b.y && a.z == b.z)
		printf ("OK\n\n\n");
	else
		printf ("ERROR - DIFFER\n\n\n");
}





void benchmarkDOT(uint64_t t) {
	vec3 a = (vec3){25, 50, 75};
	glm::vec3 b = glm::vec3(25, 50, 75);
	
	vec3 aux1 = (vec3){0, 0, 0};
	glm::vec3 aux2 = glm::vec3(0, 0, 0);
	
	uint64_t avg = 0, min = -1, max = 0, it = t;
	double result;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		result = dot_vec3 (a, aux1);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\ndot_vec3\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	avg = 0, min = -1, max = 0;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		result = glm::dot(b, aux2);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\ndot_glm\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	aux1.x = 100;
	aux1.y = 1000;
	aux1.z = 10000;
	
	aux2.x = 100;
	aux2.y = 1000;
	aux2.z = 10000;

	if (dot_vec3 (a, aux1) == glm::dot(b, aux2))
		printf ("%lf - OK\n\n\n", dot_vec3 (a, aux1));
	else
		printf ("ERROR - DIFFER\n\n\n");
}





void benchmarkNORM(uint64_t t) {
	vec3 a = (vec3){25, 50, 75};
	glm::vec3 b = glm::vec3(25, 50, 75);
	
	uint64_t avg = 0, min = -1, max = 0, it = t;
	double result;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		result = norm_vec3 (a);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\nnorm_vec3\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	avg = 0, min = -1, max = 0;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		result = glm::length(b);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\nnorm_glm\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	
	
	if (compDouble (norm_vec3 (a), glm::length(b), 6))
		printf ("%lf - OK\n\n\n", norm_vec3 (a));
	else
		printf ("%lf, %lf - ERROR - DIFFER\n\n\n", norm_vec3 (a), glm::length(b));
}





void benchmarkNORMALIZE(uint64_t t) {
	vec3 a = (vec3){25, 50, 75};
	glm::vec3 b = glm::vec3(25, 50, 75);
	
	uint64_t avg = 0, min = -1, max = 0, it = t;
	double result;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		a = normalize_vec3 (a);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\nnormalize_vec3\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	avg = 0, min = -1, max = 0;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		b = glm::normalize(b);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\nnormalize_glm\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);

	if (compDouble (a.x, b.x, 6) && compDouble (a.y, b.y, 6) && compDouble (a.z, b.z, 6))
		printf ("%lf, %lf, %lf - OK\n\n\n", a.x, a.y, a.z, norm_vec3 (a));
	else
		printf ("%lf, %lf, %lf - %lf, %lf, %lf - ERROR - DIFFER\n\n\n", a.x, a.y, a.z, b.x, b.y, b.z, norm_vec3 (a), glm::length(b));
}





void benchmarkCROSS(uint64_t t) {
	vec3 a = (vec3){25, 50, 75};
	glm::vec3 b = glm::vec3(25, 50, 75);
	
	vec3 aux1 = (vec3){0, 0, 0};
	glm::vec3 aux2 = glm::vec3(0, 0, 0);
	
	uint64_t avg = 0, min = -1, max = 0, it = t;
	vec3 result;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		result = cross_vec3 (a, aux1);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\ncross_vec3\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	glm::vec3 result2;
	
	avg = 0, min = -1, max = 0;
	for (int i = 0; i < it; i++) {
		const uint64_t st = __rdtsc();
		result2 = glm::cross(b, aux2);
		const uint64_t et = __rdtsc()-st;
		avg += et;
		if (et > max)
			max = et;
		if (et < min)
			min = et;
	}
	printf ("\ncross_glm\nAVG = %lu\nMIN=%lu\nMAX=%lu\n", avg / it, min, max);
	
	aux1.x = 100;
	aux1.y = 1000;
	aux1.z = 10000;
	
	aux2.x = 100;
	aux2.y = 1000;
	aux2.z = 10000;

	a = cross_vec3 (a, aux1);
	b = glm::cross (b, aux2);


	if (compDouble (a.x, b.x, 6) && compDouble (a.y, b.y, 6) && compDouble (a.z, b.z, 6))
		printf ("%lf, %lf, %lf - OK\n\n\n", a.x, a.y, a.z, norm_vec3 (a));
	else
		printf ("%lf, %lf, %lf - %lf, %lf, %lf - ERROR - DIFFER\n\n\n", a.x, a.y, a.z, b.x, b.y, b.z, norm_vec3 (a), glm::length(b));
}