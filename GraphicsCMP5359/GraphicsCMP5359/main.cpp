#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>

#include "threadpool.h"
#include "RasterFunctions.h"
#include "RayTracerFunctions.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model* model = nullptr;
const int width = 800;
const int height = 800;

int main(int argc, char** argv) {

    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("PalmTree.obj");
    }

    //Rasterizer
    {
        TGAImage image(width, height, TGAImage::RGB);

        RenderRasteriser(image, model, width, height);

        image.write_tga_file("Rasteriser.tga");
    }

    //Ray Tracer
    {
		TGAImage image(width, height, TGAImage::RGB);

		RenderRayTracer(image, model, 1, 5, width, height);

		image.write_tga_file("RayTracer.tga");
    }

    delete model;
    return 0;
}

