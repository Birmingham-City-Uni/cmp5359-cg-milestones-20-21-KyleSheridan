﻿#pragma once
#include "tgaimage.h"
#include "geometry.h"
#include "ray.h"
#include "hittable.h"
#include "hittable_list.h"
#include "common.h"
#include "camera.h"
#include "material.h"
#include "Sphere.h"
#include "model.h"
#include "bvh.h"
#include "triangle.h"

// method to ensure colours don’t go out of 8 bit range in RGB​
void clamp255(Vec3f& col) {
	col.x = (col.x > 255) ? 255 : (col.x < 0) ? 0 : col.x;
	col.y = (col.y > 255) ? 255 : (col.y < 0) ? 0 : col.y;
	col.z = (col.z > 255) ? 255 : (col.z < 0) ? 0 : col.z;
}

double Hit_Sphere(const Point3f& centre, double radius, const Ray& r) {
	Vec3f oc = r.Origin() - centre;
	auto a = r.Direction().dotProduct(r.Direction());
	auto b = 2.0 * oc.dotProduct(r.Direction());
	auto c = oc.dotProduct(oc) - radius * radius;
	auto discriminant = b * b - 4 * a * c;
	if (discriminant < 0) return -1.0;
	else return (-b - sqrt(discriminant)) / (2.0 * a);
}

Colour Ray_Colour(const Ray& r, const Colour& background, const Hittable& world, int depth) {
	Hit_Record rec;
	if (depth <= 0) return Colour(0, 0, 0);
	if (!world.Hit(r, 0.001, infinity, rec))
		return background;

	Ray scattered;
	Colour attentuation;
	Colour emitted = rec.mat_ptr->Emitted();

	if (!rec.mat_ptr->Scatter(r, rec, attentuation, scattered))
		return emitted;

	return emitted + attentuation * Ray_Colour(scattered, background, world, depth - 1);
}

void FullRender(TGAImage& image, Hittable_List world, Camera cam, int spp, int max_depth, int width, int height) {
	const float aspect_ratio = 16.0 / 9;
	const int image_width = width;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	//Camera cam;
	const Colour black(0, 0, 0);
	Colour pix_col(black);

	Colour background(0, 0, 0);

	for (int y = height - 1; y >= 0; --y) {
		for (int x = 0; x < width; ++x) {
			pix_col = black;
			for (int s = 0; s < spp; s++) {
				auto u = double(x + Random_Double()) / (image_width - 1);
				auto v = double(y + Random_Double()) / (image_height - 1);
				Ray ray = cam.Get_Ray(u, v);

				Vec3f unit_direction = ray.Direction().normalize();
				auto t = 0.5 * (unit_direction.y + 1.0);
				background =  (1.0 - t) * Colour(1.0, 1.0, 1.0) + t * Colour(0.5, 0.7, 1.0) * 255;

				pix_col = pix_col + Ray_Colour(ray, background, world, max_depth);
			}
			pix_col /= 255.f * spp;
			pix_col.x = sqrt(pix_col.x);
			pix_col.y = sqrt(pix_col.y);
			pix_col.z = sqrt(pix_col.z);
			pix_col *= 255;
			TGAColor colour = TGAColor(pix_col.x, pix_col.y, pix_col.z, 255);
			image.set(x, y, colour);
		}
	}
}

Hittable_List Random_Scene() {
	Hittable_List world;

	auto t_start = std::chrono::high_resolution_clock::now();

	auto ground_material = std::make_shared<Lambertian>(Colour(0.5, 0.5, 0.5));
	world.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, ground_material));
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = Random_Double();
			Point3f centre(a + 0.9 * Random_Double(), 0.2, b + 0.9 * Random_Double());
			if ((centre - Point3f(4, 0.2, 0)).length() > 0.9) {
				std::shared_ptr<Material> sphere_material;
				if (choose_mat < 0.75) {
					auto albedo = Colour::Random() * Colour::Random();
					sphere_material = std::make_shared<Lambertian>(albedo);
					world.Add(std::make_shared<Sphere>(centre, 0.2, sphere_material));
				}
				else if (choose_mat < 0.90) {
					auto albedo = Colour::Random(0.5, 1);
					auto fuzz = Random_Double(0, 0.5);
					sphere_material = std::make_shared<Metal>(albedo, fuzz);
					world.Add(std::make_shared<Sphere>(centre, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					auto albedo = Colour::Random(0.5, 1);
					sphere_material = std::make_shared<Diffuse_Light>(Colour(255, 255, 255));
					world.Add(std::make_shared<Sphere>(centre, 0.2, sphere_material));
				}
				else {
					sphere_material = std::make_shared<Dielectric>(1.5);
					world.Add(std::make_shared<Sphere>(centre, 0.2, sphere_material));
				}
			}
		}
	}
	auto material1 = std::make_shared<Dielectric>(1.5);
	world.Add(std::make_shared<Sphere>(Point3f(0, 1, 0), 1.0, material1));
	auto material2 = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(Point3f(-4, 1, 0), 1.0, material2));
	auto material3 = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
	world.Add(std::make_shared<Sphere>(Point3f(0, 1, 0), 1.0, material3));
	auto material4 = std::make_shared<Diffuse_Light>(Colour(255, 255, 255));
	world.Add(std::make_shared<Sphere>(Point3f(0, 4, 0), 1.0, material4));

	auto t_end = std::chrono::high_resolution_clock::now();
	auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
	std::cerr << "BVH Generated:  " << passedTime << " ms\n";

	return Hittable_List(std::make_shared<BVH_Node>(world));
}

Hittable_List LoadScene(Model* model) {
	Hittable_List scene;

	int percent = 1;

	for (int i = 0; i < model->nfaces(); i++) {
		Face face = model->face(i);

		Vec3f world_coords[3];

		if (face.faceSize() > 0) {
			for (int j = 0; j < 3; j++) {
				Vec3f v = model->vert(face.face[j]);

				world_coords[j] = v;
			}
			MtlMaterial mat = model->mat(face.material);
			auto material = std::make_shared<Lambertian>(Colour(mat.diffuse.x, mat.diffuse.y, mat.diffuse.z));
			scene.Add(std::make_shared<Triangle>(world_coords[0], world_coords[1], world_coords[2], material));
		}

		if (i % (model->nfaces() / 100) < 1) {
			std::cout << percent << "%\n";
			percent++;
		}
	}
	std::cout << "Scene Loaded" << "\n";

	Hittable_List world = Hittable_List(std::make_shared<BVH_Node>(scene));

	std::cout << "BVH Generated" << "\n";
	
	return world;
}

void RenderRayTracer(TGAImage& image, Model* model, const int spp, const int max_depth, int width, int height) {
	const float aspect_ratio = 16.0 / 9;
	const int image_width = width;
	const int image_height = height;

	auto scene = LoadScene(model);

	//Camera
	Point3f lookfrom(2.119f, 62.945f, 126.19f);
	Point3f lookat(3, 19, 0);

	Vec3f vup(0, 1, 0);
	auto dist_to_focus = 17;
	auto aperture = 0.15;
	Camera cam(lookfrom, lookat, vup, 33, aspect_ratio, aperture, dist_to_focus);

	const Colour white(255, 255, 255);
	const Colour black(0, 0, 0);
	const Colour red(255, 0, 0);

	double t;
	Colour pix_col(black);

	auto t_start = std::chrono::high_resolution_clock::now();

	FullRender(image, scene, cam, spp, max_depth, width, height);

	auto t_end = std::chrono::high_resolution_clock::now();
	auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
	std::cerr << "Ray traced render time:  " << passedTime << " ms\n";

	image.flip_vertically(); // we want to have the origin at the left bottom corner of the image
}