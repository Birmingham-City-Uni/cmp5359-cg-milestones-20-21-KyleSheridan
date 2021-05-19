#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class TriangleRenderer {
public:
	TriangleRenderer() {}

	void Triangle(Vec2i points[3], std::vector<float>& depthBuffer, float z, TGAImage& image, TGAColor color);
	void Triangle(Vec3f points[3], std::vector<float>& depthBuffer, float z, TGAImage& image, TGAColor color);

private:
	float sign(Vec2i p1, Vec2i p2, Vec2i p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	float edgeFunction(const Vec3f& a, const Vec3f& b, const Vec3f& c)
	{
		return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
	}

	bool PointInTriangle(Vec2i pt, Vec2i v1, Vec2i v2, Vec2i v3);

	Vec2i* FindBoundingBox(Vec2i points[3]);
};