#pragma once
#include "common.h"
#include "hittable.h"
#include "hittable_list.h"

class BVH_Node : public Hittable {
public:
	BVH_Node();
	BVH_Node(const Hittable_List& list) : BVH_Node(list.objects, 0, list.objects.size()) {}
	BVH_Node(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end);

	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const override;
	virtual bool Bounding_Box(AABB& output_box) const override;

public:
	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	AABB box;
};

inline int Random_Int(int min, int max) {
	return static_cast<int>(Random_Double(min, max + 1));
}

inline bool Box_Compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis) {
	AABB box_a;
	AABB box_b;

	if (!a->Bounding_Box(box_a) || !b->Bounding_Box(box_b))
		std::cerr << "No bounding box in BVH_Node constructor.\n";

	return box_a.Min()[axis] < box_b.Min()[axis];
}

bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return Box_Compare(a, b, 0);
}

bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return Box_Compare(a, b, 1);
}

bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return Box_Compare(a, b, 2);
}

BVH_Node::BVH_Node(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end) {
	auto objects = src_objects;

	int axis = Random_Int(0, 2);

	auto comparator = (axis == 0) ? box_x_compare
		: (axis == 1) ? box_y_compare
		: box_z_compare;

	size_t object_span = end - start;

	if (object_span == 1) {
		left = right = objects[start];
	}
	else if (object_span == 2) {
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + object_span / 2;
		left = std::make_shared<BVH_Node>(objects, start, mid);

		right = std::make_shared<BVH_Node>(objects, mid, end);
	}

	AABB box_left, box_right;

	if (!left->Bounding_Box(box_left) || !right->Bounding_Box(box_right))
		std::cerr << "No bounding box in BVH_Node constructor.\n";

	box = Surrounding_Box(box_left, box_right);
}

bool BVH_Node::Bounding_Box(AABB& output_box) const {
	output_box = box;
	return true;
}

bool BVH_Node::Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const {
	if (!box.Hit(r, t_min, t_max))
		return false;

	bool hit_left = left->Hit(r, t_min, t_max, rec);
	bool hit_right = right->Hit(r, t_min, hit_left ? rec.t : t_max, rec);

	return hit_left || hit_right;
}
