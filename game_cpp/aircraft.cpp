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
	relativePosition = 0.f;
	angle = _mothership.getAngle();
	scene::placeMesh(mesh, position.x, position.y, angle);
	status = TakeOff;
}

void Aircraft::update(float dt) {
	float linearSpeed = 0.f;
	float angularSpeed = 0.f;
	switch (status) {
	case ReadyToFlight:
		return;
		break;
	case TakeOff:
		linearSpeed = params::aircraft::LINEAR_SPEED * 0.1;
		angle = _mothership.getAngle();
		relativePosition = relativePosition + linearSpeed * dt;
		position = _mothership.getPosition() + relativePosition * Vector2(std::cos(angle), std::sin(angle));
		scene::placeMesh(mesh, position.x, position.y, angle);
		if (_isTakeOffFinished()) {
			status = LayInACourse;
		}
		break;
	case LayInACourse:
		linearSpeed = params::aircraft::LINEAR_SPEED;
		angle = angle + 2 * dt;
		position = position + linearSpeed * 2 * dt * Vector2(std::cos(angle), std::sin(angle));
		scene::placeMesh(mesh, position.x, position.y, angle);
		//PlaceHolder
		if ((angle > 3.95) && (angle< 4.05)) {
			status = FlyForward;
			}
		//
		break;
	case FlyForward:
		linearSpeed = params::aircraft::LINEAR_SPEED;
		angle = angle;
		position = position + linearSpeed * dt * Vector2(std::cos(angle), std::sin(angle));
		scene::placeMesh(mesh, position.x, position.y, angle);

		break;
	case Fuelling:
		return;
		break;
	}
}

bool Aircraft::_isTakeOffFinished() {
	return params::aircraft::TAKEOFF_RADIUS < relativePosition;
}