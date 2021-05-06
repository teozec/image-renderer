#include "camera.h"
#include "shape.h"
#undef NDEBUG
#include <cassert>

using namespace std;

void testSphere()
{
	Ray ray1{Point{0.f, 0.f, 2.f}, Vec{0.f, 0.f, -1.f}};
	Sphere sphere1;
	HitRecord hit1 = sphere1.rayIntersection(ray1);
	assert(hit1.hit);
	assert((hit1.worldPoint == Point{0.f, 0.f, 1.f}));
	assert((hit1.normal == Normal{0.f, 0.f, 1.f}));
}

void testWorld() {
	World myWorld;
	Ray ray1{Point{0.f, 0.f, 2.f}, Vec{0.f, 0.f, -1.f}};
	Sphere sphere1;
	myWorld.add(sphere1);
	HitRecord hit1 = myWorld.rayIntersection(ray1);
	assert(hit1.hit);
	assert((hit1.worldPoint == Point{0.f, 0.f, 1.f}));
	assert((hit1.normal == Normal{0.f, 0.f, 1.f}));
}

int main()
{
	testSphere();
	testWorld();
	return 0;
}
