#pragma once
#include "../framework/scene.hpp"
#include "supporting_function.h"

#include <memory>

class Ship;

enum AircraftStatus {
	ReadyToFlight,
	TakeOff,
	FlyForward,
	Fuelling,
	Count
};

class Aircraft
{
public:
	Aircraft(Ship& mothership);
	void init();
	void deinit();
	void Takeoff();
	void update(float dt);
private:
	scene::Mesh* mesh;
	Ship& _mothership;

	Vector2 position;
	float angle;
	AircraftStatus status;
};