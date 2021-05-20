#pragma once

#include "hittable.h"
#include "geometry.h"

class Triangle : public Hittable {
public:
	Triangle() {}
	Triangle(Point3f vert0, Point3f vert1, Point3f vert2, std::shared_ptr<Material> m) : 
		v0(vert0), v1(vert1), v2(vert2), mat_ptr(m) {
		normal = (v1 - v0).crossProduct(v2 - v0);
	}
	Triangle(Point3f vert0, Point3f vert1, Point3f vert2, Vec3f n0, Vec3f n1, Vec3f n2, std::shared_ptr<Material> m) :
		v0(vert0), v1(vert1), v2(vert2), v0n(n0), v1n(n1), v2n(n2), mat_ptr(m) {}

	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const override;
	virtual bool Bounding_Box(AABB& output_box) const override;

private:
	Point3f v0, v1, v2;
	Point3f v0n, v1n, v2n;
	Vec3f normal;
	std::shared_ptr<Material>mat_ptr;
};

bool Triangle::Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const 
{
	float thit, t, u, v;

	Vec3f v0v1 = v1 - v0;
	Vec3f v0v2 = v2 - v0;
	Vec3f pvec = r.Direction().crossProduct(v0v2);

	float det = pvec.dotProduct(v0v1);
	float kEpsilon = 0.00001;

	if (det < kEpsilon) return false;

	float invDet = 1 / det;

	Vec3f tvec = r.Origin() - v0;
	u = tvec.dotProduct(pvec) * invDet;

	if (u < 0 || u > 1) return false;

	Vec3f gvec = tvec.crossProduct(v0v1);
	v = r.Direction().dotProduct(gvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	t = v0v2.dotProduct(gvec) * invDet;

	if (t < 0) return false;

	rec.p = r.At(t);
	rec.t = t;

	//Vec3f fnormal = (v0n + v1n + v2n / 3);
	//rec.normal = fnormal;

	rec.normal = normal;
	rec.mat_ptr = mat_ptr;

	return true;
}

inline bool Triangle::Bounding_Box(AABB& output_box) const
{
	float min[3];
	float max[3];
	for (int i = 0; i < 3; i++) {
		min[i] = std::min(v0[i], std::min(v1[i], v2[i]));
		max[i] = std::max(v0[i], std::max(v1[i], v2[i]));
	}
	output_box = AABB(Vec3f(min[0], min[1], min[2]), Vec3f(max[0], max[1], max[2]));
	return true;
}