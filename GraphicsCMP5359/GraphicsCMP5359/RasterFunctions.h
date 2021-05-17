#pragma once
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include "TriangleRenderer.h"

void Line(int x0, int y0, int x1, int y1, TGAImage* image, TGAColor colour)
{
    int newX = std::min(x0, x1);

    int tempY = y0;

    bool steep = false;

    if (abs(x0 - x1) < abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);

        steep = true;
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (float x = x0; x <= x1; x++)
    {
        float t = (x - x0) / (x1 - x0);
        int y = y0 * (1 - t) + (y1 * t);

        steep ? image->set(y, x, colour) : image->set(x, y, colour);
    }
}

int clamp(int val1, int val2) {
    if (val1 > val2) {
        return val2;
    }
    else {
        return val1;
    }
}

void RenderRasteriser(TGAImage& image, Model* model, int width, int height) {
    TriangleRenderer* tr = new TriangleRenderer();
    Vec3f light_dir(0, 0, -1);

    float* depthBuffer = new float[image.get_width() * image.get_height()];
    for (int i = 0; i < image.get_width() * image.get_height(); i++)
        depthBuffer[i] = FLT_MAX;

    std::cout << image.get_width() << " " << image.get_height() << "\n";

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        std::vector<int> tex = model->tex(i);

        Face face2 = model->face2(i);
        
        /*for (auto& i : face)
        {
            std::cout << i << "\n";
        }

        std::cout << "------- face" << "\n";

        for (auto& i : tex)
        {
            std::cout << i << "\n";
        }

        std::cout << "------- texture" << "\n";*/

        Vec2i screen_coords[3];
        Vec3f world_coords[3];

        float z = 10000;

        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face2.face[j]);

            screen_coords[j] = Vec2i(((v.x + 1.0) * width / 8.0) + 200, ((v.y + 1.0) * height / 8.0) + 200); //TO DO - CHANGE THIS LINE -- THIS IS SO PALM TREE FITS
            world_coords[j] = v;

            if (v.z < z) {
                z = v.z;
            }
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n.dotProduct(light_dir);

        if (intensity > 0) {
            MtlMaterial mat = model->mat(face2.material);
            tr->Triangle(screen_coords, depthBuffer, z, image, TGAColor(intensity * mat.diffuse.r * 255, intensity * mat.diffuse.g * 255, intensity * mat.diffuse.b * 255, 255));
        }
    }

    image.flip_vertically(); // we want to have the origin at the left bottom corner of the image
}