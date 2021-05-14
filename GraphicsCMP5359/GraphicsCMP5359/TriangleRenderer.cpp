#include "TriangleRenderer.h"

void TriangleRenderer::Triangle(Vec2i points[3], float* depthBuffer, float z, TGAImage& image, TGAColor color)
{
	Vec2i* bbox = FindBoundingBox(points);

	for (int y = bbox[0].y; y <= bbox[1].y; y++) {
		for (int x = bbox[0].x; x <= bbox[1].x; x++) {
			if (y >= 0 && y < image.get_height() && x >= 0 && x < image.get_width()) {
				Vec2i point = Vec2i(x, y);
				if (PointInTriangle(point, points[0], points[1], points[2])) {

					//std::cout << y << " " << x << " " << y * 640 + x << "\n";

					if (z < depthBuffer[y * 640 + x]) {
						depthBuffer[y * 640 + x] = z;
					
						image.set(x, y, color);
					}
				}
			}
		}
	}
}

bool TriangleRenderer::PointInTriangle(Vec2i pt, Vec2i v1, Vec2i v2, Vec2i v3)
{
	/*float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);*/

	float denominator = ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
	float a = ((v2.y - v3.y) * (pt.x - v3.x) + (v3.x - v2.x) * (pt.y - v3.y)) / denominator;
	float b = ((v3.y - v1.y) * (pt.x - v3.x) + (v1.x - v3.x) * (pt.y - v3.y)) / denominator;
	float c = 1 - a - b;

	return 0 <= a && a <= 1 && 0 <= b && b <= 1 && 0 <= c && c <= 1;
}

Vec2i* TriangleRenderer::FindBoundingBox(Vec2i points[3])
{
	int maxX = std::max(points[0].x, std::max(points[1].x, points[2].x));
	int maxY = std::max(points[0].y, std::max(points[1].y, points[2].y));
	int minX = std::min(points[0].x, std::min(points[1].x, points[2].x));
	int minY = std::min(points[0].y, std::min(points[1].y, points[2].y));

	Vec2i* max = new Vec2i(maxX, maxY);
	Vec2i* min = new Vec2i(minX, minY);

	Vec2i* arr = new Vec2i[2];

	arr[0] = *min;
	arr[1] = *max;

	return arr;
}
