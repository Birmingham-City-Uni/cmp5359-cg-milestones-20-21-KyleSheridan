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
const int width = 1920;
const int height = 1080;

int main(int argc, char** argv) {

    if (3 == argc) {
        model = new Model(argv[1], argv[2]);
    }
    else {
        model = new Model("Scene.obj", "Scene.mtl");
    }

    //Rasterizer
    {
        TGAImage image(width, height, TGAImage::RGB);

        RenderRasteriser(image, model, width, height);

        image.write_tga_file("Rasteriser.tga");
    }

    //Ray Tracer
    {
		/*TGAImage image(width, height, TGAImage::RGB);

		RenderRayTracer(image, model, 1, 50, width, height);

		image.write_tga_file("RayTracer.tga");*/
    }


    delete model;
    return 0;
}

