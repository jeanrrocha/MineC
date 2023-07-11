class Ray {
	public:
	Ray(Vector3 &o, Vector3 &d) {
		origin = o;
		direction = d;
		inv_direction = Vector3(1/d.x(), 1/d.y(), 1/d.z());
		sign[0] = (inv_direction.x() < 0);
		sign[1] = (inv_direction.y() < 0);
		sign[2] = (inv_direction.z() < 0);
	}
	Vector3 origin;
	Vector3 direction;
	Vector3 inv_direction;
	int sign[3];
};

class Box {
	public:
	Box(const Vector3 &min, const Vector3 &max) {
		assert(min < max);
		bounds[0] = min;
		bounds[1] = max;
	}
	bool intersect(const Ray &, float t0, float t1) const;
	Vector3 bounds[2];
};

// Optimized method
bool Box::intersect(const Ray &r, float t0, float t1) const {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	tmin = (bounds[r.sign[0]].x() - r.origin.x()) * r.inv_direction.x();
	tmax = (bounds[1-r.sign[0]].x() - r.origin.x()) * r.inv_direction.x();
	tymin = (bounds[r.sign[1]].y() - r.origin.y()) * r.inv_direction.y();
	tymax = (bounds[1-r.sign[1]].y() - r.origin.y()) * r.inv_direction.y();
	if ( (tmin > tymax) || (tymin > tmax) )
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (bounds[r.sign[2]].z() - r.origin.z()) * r.inv_direction.z();
	tzmax = (bounds[1-r.sign[2]].z() - r.origin.z()) * r.inv_direction.z();
	if ( (tmin > tzmax) || (tzmin > tmax) )
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return ( (tmin < t1) && (tmax > t0) );
}

	

//Duas urnas com n candidatos, imprimir a quantidade de votos de todos em ordem crescente.
