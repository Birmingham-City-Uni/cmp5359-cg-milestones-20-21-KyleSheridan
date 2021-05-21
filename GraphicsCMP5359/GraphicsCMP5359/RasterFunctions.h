#pragma once
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include "TriangleRenderer.h"

#define M_PI 3.14159265359

static const float inchToMm = 25.4;
enum FitResolutionGate { kFill = 0, kOverscan };

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

// Compute screen coordinates based on a physically-based camera model
// http://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera
void computeScreenCoordinates(
    const float& filmApertureWidth,
    const float& filmApertureHeight,
    const uint32_t& imageWidth,
    const uint32_t& imageHeight,
    const FitResolutionGate& fitFilm,
    const float& nearClippingPlane,
    const float& focalLength,
    float& top, float& bottom, float& left, float& right
)
{
    float filmAspectRatio = filmApertureWidth / filmApertureHeight;
    float deviceAspectRatio = imageWidth / (float)imageHeight;

    top = ((filmApertureHeight * inchToMm / 2) / focalLength) * nearClippingPlane;
    right = ((filmApertureWidth * inchToMm / 2) / focalLength) * nearClippingPlane;

    float fov = 54.43f;
    std::cerr << "Field of view " << fov << std::endl;

    float xscale = 1;
    float yscale = 1;

    switch (fitFilm) {
    default:
    case kFill:
        if (filmAspectRatio > deviceAspectRatio) {
            xscale = deviceAspectRatio / filmAspectRatio;
        }
        else {
            yscale = filmAspectRatio / deviceAspectRatio;
        }
        break;
    case kOverscan:
        if (filmAspectRatio > deviceAspectRatio) {
            yscale = filmAspectRatio / deviceAspectRatio;
        }
        else {
            xscale = deviceAspectRatio / filmAspectRatio;
        }
        break;
    }

    right *= xscale;
    top *= yscale;

    bottom = -top;
    left = -right;
}

// Compute vertex raster screen coordinates.
void convertToRaster(
    const Vec3f& vertexWorld,
    const Matrix44f& worldToCamera,
    const float& l,
    const float& r,
    const float& t,
    const float& b,
    const float& near,
    const uint32_t& imageWidth,
    const uint32_t& imageHeight,
    Vec3f& vertexRaster
)
{
    Vec3f vertexCamera;
    worldToCamera.multVecMatrix(vertexWorld, vertexCamera);

    Vec2f vertexScreen;
    vertexScreen.x = near * vertexCamera.x / -vertexCamera.z;
    vertexScreen.y = near * vertexCamera.y / -vertexCamera.z;

    Vec2f vertexNDC;
    vertexNDC.x = 2 * vertexScreen.x / (r - l) - (r + l) / (r - l);
    vertexNDC.y = 2 * vertexScreen.y / (t - b) - (t + b) / (t - b);

    vertexRaster.x = (vertexNDC.x + 1) / 2 * imageWidth;
    vertexRaster.y = (1 - vertexNDC.y) / 2 * imageHeight;
    vertexRaster.z = -vertexCamera.z;
}

Matrix44f worldToCamera;

const float nearClippingPlane = 1;
const float farClippingPlane = 1000;
float focalLength = 20; // in mm
// 35mm Full Aperture in inches
float filmApertureWidth = 0.980;
float filmApertureHeight = 0.735;

Matrix44f lookAt(const Vec3f from, const Vec3f to, const Vec3f _tmp = Vec3f(0, 1, 0))
{
    // Calculate forward, right and up vectors
    Vec3f forward = from - to;
    forward.normalize();
    Vec3f right = _tmp.crossProduct(forward);
    Vec3f up = forward.crossProduct(right);

    Matrix44f camToWorld;

    // Set the values of the camToWorld matrix
    camToWorld[0][0] = right.x;
    camToWorld[0][1] = right.y;
    camToWorld[0][2] = right.z;
    camToWorld[0][3] = 0;
    camToWorld[1][0] = up.x;
    camToWorld[1][1] = up.y;
    camToWorld[1][2] = up.z;
    camToWorld[1][3] = 0;
    camToWorld[2][0] = forward.x;
    camToWorld[2][1] = forward.y;
    camToWorld[2][2] = forward.z;
    camToWorld[2][3] = 0;

    camToWorld[3][0] = from.x;
    camToWorld[3][1] = from.y;
    camToWorld[3][2] = from.z;
    camToWorld[3][3] = 1;

    return camToWorld;
}

void RenderRasteriser(TGAImage& image, Model* model, int width, int height) {
    TriangleRenderer* tr = new TriangleRenderer();
    Vec3f light_dir(0, 0, 1);

    // compute screen coordinates
    float t, b, l, r;

    // Calculate screen coordinates and store in t, b, l, r
    computeScreenCoordinates(
        filmApertureWidth, filmApertureHeight,
        width, height,
        kOverscan,
        nearClippingPlane,
        focalLength,
        t, b, l, r);

    std::vector<float> depthBuffer(width * height);

    std::fill(begin(depthBuffer), end(depthBuffer), farClippingPlane);

    float camX, camY, camZ;
    float camAngleX = 0.0f;
    float camAngleY = 0.0f;

    float distance = 30.0f;

    camX = distance * -sinf(camAngleX * (M_PI / 180)) * cosf((camAngleY) * (M_PI / 180));
    camY = distance * -sinf(camAngleY * (M_PI / 180));
    camZ = distance * cosf(camAngleX * (M_PI / 180)) * sinf((camAngleY) * (M_PI / 180));


    Vec3f eye(2.119, 50.945f, 86.0f);
    Vec3f target(3.0f, 19.0f, 0.0f);
    Vec3f up(0.0f, 1.0f, 0.0f);
    worldToCamera = lookAt(eye, target, up).inverse();

    std::cout << image.get_width() << " " << image.get_height() << "\n";

    std::cout << model->nfaces() << "\n";

    for (auto& mat : model->matMap()) {
        std::cout << mat.second.matName << "\n";
        std::cout << mat.second.diffuse << "\n";
    }

    for (int i = 0; i < model->nfaces(); i++) {
        
        if (i == 32)
            std::cout << "help\n";
        
        Face face = model->face(i);

        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        Vec3f raster_coords[3];

        float z = -farClippingPlane;

        if (face.faceSize() > 0) {
            for (int j = 0; j < 3; j++) {
                Vec3f v = model->vert(face.face[j]);

                convertToRaster(v, worldToCamera, l, r, t, b, nearClippingPlane, width, height, raster_coords[j]);

                world_coords[j] = v;

                if (raster_coords[j].z > z) {
                    z = raster_coords[j].z;
                }
            }

            Vec3f v0Cam, v1Cam, v2Cam;
            worldToCamera.multVecMatrix(world_coords[0], v0Cam);
            worldToCamera.multVecMatrix(world_coords[1], v1Cam);
            worldToCamera.multVecMatrix(world_coords[2], v2Cam);

            Vec3f n = (v1Cam - v0Cam).crossProduct(v2Cam - v0Cam);
            n.normalize();
            float intensity = n.dotProduct(light_dir);

            if (intensity >= 0) {
                MtlMaterial mat = model->mat(face.material);

                //Back face culling
                if (n.dotProduct(v1Cam) < 0) {
                    tr->Triangle(raster_coords, depthBuffer, z, image, TGAColor(intensity * mat.diffuse.r * 255, intensity * mat.diffuse.g * 255, intensity * mat.diffuse.b * 255, 255));
                }
           }
        }
    }
}