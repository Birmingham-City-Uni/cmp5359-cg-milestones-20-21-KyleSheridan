#include "TriangleRenderer.h"

void TriangleRenderer::Triangle(Vec2i points[3], std::vector<float>& depthBuffer, float z, TGAImage& image, TGAColor color)
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

void TriangleRenderer::Triangle(Vec3f points[3], std::vector<float>& depthBuffer, float z, TGAImage& image, TGAColor color)
{
	Vec2i screenPoints[3];

	for (int i = 0; i < 3; i++)
		screenPoints[i] = Vec2i(points[i].x, points[i].y);

	Vec2i* bbox = FindBoundingBox(screenPoints);

	float area = edgeFunction(points[0], points[1], points[2]);

	for (int y = bbox[0].y; y <= bbox[1].y; y++) {
		for (int x = bbox[0].x; x <= bbox[1].x; x++) {
			if (y >= 0 && y < image.get_height() && x >= 0 && x < image.get_width()) {
				Vec2i point = Vec2i(x, y);

				Vec3f pixelSample(x + 0.5, y + 0.5, 0); // You could use multiple pixel samples for antialiasing!!
				   // Calculate the area of the subtriangles for barycentric coordinates
				float w0 = edgeFunction(points[1], points[2], pixelSample);
				float w1 = edgeFunction(points[1], points[0], pixelSample);
				float w2 = edgeFunction(points[0], points[1], pixelSample);

				w0 /= area;
				w1 /= area;
				w2 /= area;
				float oneOverZ = points[0].z * w0 + points[1].z * w1 + points[2].z * w2; // reciprocal for depth testing
				float a = 1 / oneOverZ;

				if (PointInTriangle(point, screenPoints[0], screenPoints[1], screenPoints[2])) {

					//std::cout << y << " " << x << " " << y * 640 + x << "\n";
					

					if (z < depthBuffer[(y * image.get_width()) + x]) {
						depthBuffer[(y * image.get_width()) + x] = z;

						image.set(x, y, color);
					}
				}
			}
		}
	}
}

bool TriangleRenderer::PointInTriangle(Vec2i pt, Vec2i v1, Vec2i v2, Vec2i v3)
{
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
