#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char* modelFilename, const char* matFilename) : verts_(), faces_() {
    std::ifstream modelFile;
    modelFile.open(modelFilename, std::ifstream::in);
    if (modelFile.fail()) return;
    std::string line;
    std::string material;
    while (!modelFile.eof()) {
        std::getline(modelFile, line);
        std::istringstream iss(line.c_str());
        char trash;
        std::string strash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash;
            iss >> trash;
            Vec2f vt;
            for (int i = 0; i < 2; i++) iss >> vt[i];
            vts_.push_back(vt);
        }
        else if (!line.compare(0, 7, "usemtl ")) {
            material.clear();
            
            iss >> strash;

            iss >> material;
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            std::vector<int> t;
            int itrash, idx, tex;

            iss >> trash;
            while (iss >> idx >> trash >> tex >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                tex--;
                f.push_back(idx);
                t.push_back(tex);
            }
            faces_.emplace_back(f, t, material);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;

    std::ifstream matFile;
    matFile.open(matFilename, std::ifstream::in);
    if (matFile.fail()) return;
    while (!matFile.eof()) {
        std::getline(matFile, line);
        std::istringstream iss(line.c_str());

        std::string strash;

        if (!line.compare(0, 7, "newmtl ")) {
            iss >> strash;

            iss >> material;
        }
        else if (!line.compare(0, 3, "Kd ")) {
            Vec3f colour;
            
            iss >> strash;

            for(int i = 0; i < 3; i++)
            {
                iss >> colour[i];
            }

            mats_.emplace(material, MtlMaterial(material, colour));
        }
    }
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

Face& Model::face(int idx) {
    return faces_[idx];
}

MtlMaterial& Model::mat(std::string key)
{
    return mats_[key];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::vt(int i) {
    return vts_[i];
}