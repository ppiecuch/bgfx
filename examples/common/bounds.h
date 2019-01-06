/*
 * Copyright 2011-2018 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#ifndef BOUNDS_H_HEADER_GUARD
#define BOUNDS_H_HEADER_GUARD

#include <bx/math.h>

struct Aabb
{
	bx::Vec3 m_min;
	bx::Vec3 m_max;
};

struct Cylinder
{
	bx::Vec3 m_pos;
	bx::Vec3 m_end;
	float m_radius;
};

struct Capsule
{
	bx::Vec3 m_pos;
	bx::Vec3 m_end;
	float m_radius;
};

struct Cone
{
	bx::Vec3 m_pos;
	bx::Vec3 m_end;
	float m_radius;
};

struct Disk
{
	bx::Vec3 m_center;
	bx::Vec3 m_normal;
	float m_radius;
};

struct Obb
{
	float m_mtx[16];
};

struct Ray
{
	bx::Vec3 m_pos;
	bx::Vec3 m_dir;
};

struct Sphere
{
	bx::Vec3 m_center;
	float m_radius;
};

struct Tris
{
	bx::Vec3 m_v0;
	bx::Vec3 m_v1;
	bx::Vec3 m_v2;
};

struct Hit
{
	bx::Vec3 m_pos;
	bx::Vec3 m_normal;
	float m_dist;
};

/// Convert axis aligned bounding box to oriented bounding box.
void aabbToObb(Obb& _obb, const Aabb& _aabb);

/// Convert oriented bounding box to axis aligned bounding box.
void toAabb(Aabb& _aabb, const Obb& _obb);

/// Convert sphere to axis aligned bounding box.
void toAabb(Aabb& _aabb, const Sphere& _sphere);

/// Convert disk to axis aligned bounding box.
void toAabb(Aabb& _aabb, const Disk& _disk);

/// Convert cylinder to axis aligned bounding box.
void toAabb(Aabb& _aabb, const Cylinder& _cylinder);

/// Calculate axis aligned bounding box.
void toAabb(Aabb& _aabb, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

/// Transform vertices and calculate axis aligned bounding box.
void toAabb(Aabb& _aabb, const float* _mtx, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

/// Expand AABB.
void aabbExpand(Aabb& _aabb, float _factor);

/// Expand AABB with xyz.
void aabbExpand(Aabb& _aabb, const float* _pos);

/// Calculate surface area of axis aligned bounding box.
float calcAreaAabb(const Aabb& _aabb);

/// Returns 0 is two AABB don't overlap, otherwise returns flags of overlap
/// test.
uint32_t aabbOverlapTest(const Aabb& _aabb0, const Aabb& _aabb1);

/// Calculate oriented bounding box.
void calcObb(Obb& _obb, const void* _vertices, uint32_t _numVertices, uint32_t _stride, uint32_t _steps = 17);

/// Calculate maximum bounding sphere.
void calcMaxBoundingSphere(Sphere& _sphere, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

/// Calculate minimum bounding sphere.
void calcMinBoundingSphere(Sphere& _sphere, const void* _vertices, uint32_t _numVertices, uint32_t _stride, float _step = 0.01f);

/// Returns 6 (near, far, left, right, top, bottom) planes representing frustum planes.
void buildFrustumPlanes(bx::Plane* _planes, const float* _viewProj);

/// Returns point from 3 intersecting planes.
bx::Vec3 intersectPlanes(const bx::Plane& _pa, const bx::Plane& _pb, const bx::Plane& _pc);

/// Make screen space ray from x, y coordinate and inverse view-projection matrix.
Ray makeRay(float _x, float _y, const float* _invVp);

/// Intersect ray / AABB.
bool intersect(const Ray& _ray, const Aabb& _aabb, Hit* _hit = NULL);

/// Intersect ray / OBB.
bool intersect(const Ray& _ray, const Obb& _obb, Hit* _hit = NULL);

/// Intersect ray / cylinder.
bool intersect(const Ray& _ray, const Cylinder& _cylinder, Hit* _hit = NULL);

/// Intersect ray / capsule.
bool intersect(const Ray& _ray, const Capsule& _capsule, Hit* _hit = NULL);

/// Intersect ray / cone.
bool intersect(const Ray& _ray, const Cone& _cone, Hit* _hit = NULL);

/// Intersect ray / disk.
bool intersect(const Ray& _ray, const Disk& _disk, Hit* _hit = NULL);

/// Intersect ray / plane.
bool intersect(const Ray& _ray, const bx::Plane& _plane, Hit* _hit = NULL);

/// Intersect ray / sphere.
bool intersect(const Ray& _ray, const Sphere& _sphere, Hit* _hit = NULL);

/// Intersect ray / triangle.
bool intersect(const Ray& _ray, const Tris& _triangle, Hit* _hit = NULL);

#endif // BOUNDS_H_HEADER_GUARD
