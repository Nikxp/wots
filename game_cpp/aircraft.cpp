#include "aircraft.h"
#include "ship.h"
#include <cassert>
#include <cmath>

Aircraft::Aircraft(Ship& mothership) :
	mesh(nullptr),
	_mothership(mothership),
	status(AircraftStatus::ReadyToFlight)
{
}

void Aircraft::init() {
	position = Vector2(0.f, 0.f);
	angle = 0.f;

}

void Aircraft::deinit()
{
	if (mesh != NULL) {
		scene::destroyMesh(mesh);
		mesh = nullptr;
	}
}

void Aircraft::Takeoff()
{
	assert(!mesh);
	mesh = scene::createAircraftMesh();
	position = _mothership.getPosition();
	angle = _mothership.getAngle();
	scene::placeMesh(mesh, position.x, position.y, angle);
	status = FlyForward;
}


void Aircraft::update(float dt) {
	float linearSpeed = 0.f;
	float angularSpeed = 0.f;
	switch (status) {
	case ReadyToFlight:
		return;
		break;
	case TakeOff:

		break;
	case FlyForward:
		linearSpeed = params::aircraft::LINEAR_SPEED;

		break;
	case Fuelling:
		return;
		break;
	}
	angle = angle;
	position = position + linearSpeed * dt * Vector2(std::cos(angle), std::sin(angle));
	scene::placeMesh(mesh, position.x, position.y, angle);
};