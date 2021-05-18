#pragma once
#include <iostream>
#include <cmath>
#include "geometry.h"
#include "tgaimage.h"

class TriangleRenderer {
public:
	TriangleRenderer() {}

	void Triangle(Vec2i points[3], float* depthBuffer, float z, TGAImage& image, TGAColor color);
	void Triangle(Vec3f points[3], float* depthBuffer, float z, TGAImage& image, TGAColor color);

private:
	float sign(Vec2i p1, Vec2i p2, Vec2i p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	bool PointInTriangle(Vec2i pt, Vec2i v1, Vec2i v2, Vec2i v3);

	Vec2i* FindBoundingBox(Vec2i points[3]);
};