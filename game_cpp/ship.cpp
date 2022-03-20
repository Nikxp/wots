
#include "ship.h"
#include <cassert>
#include <cmath>

Ship::Ship() :
	mesh(nullptr)
{
}

void Ship::init()
{
	assert(!mesh);
	mesh = scene::createShipMesh();
	position = Vector2(0.f, 0.f);
	angle = 0.f;
	for (bool& key : input)
		key = false;
	for (int index = 0; index < params::ship::AIRCRAFT_SHIP_CAPACITY; index++) {
		aircraftStorage.push_back(Aircraft(*this));
		aircraftStorage[index].init();
	}
}


void Ship::deinit()
{
	scene::destroyMesh(mesh);
	mesh = nullptr;
	for (auto& aircraft : aircraftStorage) {
		aircraft.deinit();
	}
}


void Ship::update(float dt)
{
	float linearSpeed = 0.f;
	float angularSpeed = 0.f;

	if (input[game::KEY_FORWARD])
	{
		linearSpeed = params::ship::LINEAR_SPEED;
	}
	else if (input[game::KEY_BACKWARD])
	{
		linearSpeed = -params::ship::LINEAR_SPEED;
	}

	if (input[game::KEY_LEFT] && linearSpeed != 0.f)
	{
		angularSpeed = params::ship::ANGULAR_SPEED;
	}
	else if (input[game::KEY_RIGHT] && linearSpeed != 0.f)
	{
		angularSpeed = -params::ship::ANGULAR_SPEED;
	}

	angle = angle + angularSpeed * dt;
	position = position + linearSpeed * dt * Vector2(std::cos(angle), std::sin(angle));
	scene::placeMesh(mesh, position.x, position.y, angle);
	for (auto& aircraft : aircraftStorage) {
		aircraft.update(dt);
	}
}


void Ship::keyPressed(int key)
{
	assert(key >= 0 && key < game::KEY_COUNT);
	input[key] = true;
}


void Ship::keyReleased(int key)
{
	assert(key >= 0 && key < game::KEY_COUNT);
	input[key] = false;
}


void Ship::mouseClicked(Vector2 worldPosition, bool isLeftButton)
{
	// TODO: placeholder, remove it and implement aircarfts logic
	if (isLeftButton)
	{
		scene::placeGoalMarker(worldPosition.x, worldPosition.y);
		target = worldPosition;
	}
	else
	{
		aircraftStorage[0].Takeoff();
	}
}

Vector2 Ship::getPosition() {
	return position;
}

float Ship::getAngle() {
	return angle;
}