#pragma once
#include <vector>
#include <map>
#include "geometry.h"

struct Face {
	std::vector<int> face;
	std::vector<int> texInx;
	std::string material;

	Face(std::vector<int> f, std::vector<int> t, std::string mat) : face(f), texInx(t), material(mat) {}

	int faceSize() {
		return face.size();
	}
};

struct MtlMaterial {
	std::string matName;
	Vec3f diffuse;

	MtlMaterial(std::string mat, Vec3f diff) : matName(mat), diffuse(diff) {}
	MtlMaterial() {
		matName = "NoMat";
		diffuse = Vec3f(0.0f, 0.0f, 0.0f);
	}
};

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Face> faces_;
	std::vector<Vec2f> vts_;
	std::map<std::string, MtlMaterial> mats_;

public:
	Model(const char* modelFilename, const char* matFilename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec2f vt(int i);
	Face& face(int idx);
	MtlMaterial& mat(std::string key);
};
