#include <vector>
#include <cmath>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

// Write the line method here
// line (x0, y0, x1, y1, image, colour)
void Line(int x0, int y0, int x1, int y1, TGAImage* image, TGAColor colour)
{
    for (float t = 0; t < 1; t += 0.01f)
    {
        int x = x0 + (x1 - x0) * t;
        int y = y0 + (y1 - y0) * t;

        image->set(x, y, colour);
    }
}

void Line2(int x0, int y0, int x1, int y1, TGAImage* image, TGAColor colour)
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

int main(int argc, char** argv) {

    TGAImage image(width, height, TGAImage::RGB);

    /*Line2(13, 20, 80, 40, &image, TGAColor(10, 248, 136, 255));
    Line2(20, 13, 40, 80, &image, TGAColor(212, 126, 12, 255));
    Line2(80, 40, 13, 20, &image, TGAColor(255, 12, 136, 255));*/

    for (int i = 0; i < 80; i++)
    {
        Line2(0, i * 10, 800, i * 10, &image, TGAColor(12, 27, 60, 255));
        Line2(0, (i * 40) + 5, 800, (i * 20) + 5, &image, TGAColor(50, 20, 40, 255));
        Line2(0, (i * 20) + 5, 800, (i * 40) + 5, &image, TGAColor(10, 30, 10, 255));
    }

    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("PalmTree.obj");
    }

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 8;
            int y0 = (v0.y + 1.) * height / 8;
            int x1 = (v1.x + 1.) * width / 8;
            int y1 = (v0.y + 1.) * height / 8;

            if (y1 <= 190) {
                Line2(x0 + 200, y0 + 200, x1 + 200, y1 + 200, &image, TGAColor(70, 20, 10, 255));
                Line2(x0 + 200, clamp(y0 + 225, 450), x1 + 200, clamp(y1 + 225, 450), &image, TGAColor(90, 25, 15, 255));
                Line2(x0 + 200, clamp(y0 + 250, 450), x1 + 200, clamp(y1 + 250, 450), &image, TGAColor(90, 25, 15, 255));
                Line2(x0 + 200, clamp(y0 + 275, 450), x1 + 200, clamp(y1 + 275, 450), &image, TGAColor(90, 25, 15, 255));
                Line2(x0 + 200, clamp(y0 + 300, 450), x1 + 200, clamp(y1 + 300, 450), &image, TGAColor(90, 25, 15, 255));
                Line2(x0 + 200, clamp(y0 + 325, 450), x1 + 200, clamp(y1 + 325, 450), &image, TGAColor(90, 25, 15, 255));
                Line2(x0 + 200, clamp(y0 + 350, 450), x1 + 200, clamp(y1 + 350, 450), &image, TGAColor(90, 25, 15, 255));
                Line2(x0 + 200, clamp(y0 + 375, 450), x1 + 200, clamp(y1 + 375, 450), &image, TGAColor(90, 25, 15, 255));
            }
            else if (y1 > 200 && y1 <= 400) {
                Line2(x0 + 200, y0 + 200, x1 + 200, y1 + 200, &image, TGAColor(12, 255, 89, 255));
            }
            else if (y1 > 400 && y1 <= 600) {
                Line2(x0 + 200, y0 + 200, x1 + 200, y1 + 200, &image, TGAColor(12, 255, 89, 255));
            }
            else if (y1 > 600 && y1 <= 800) {
                Line2(x0 + 200, y0 + 200, x1 + 200, y1 + 200, &image, TGAColor(12, 255, 89, 255));
            }
        }
    }



    image.flip_vertically(); // we want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

