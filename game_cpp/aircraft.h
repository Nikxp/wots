#pragma once
#include "../framework/scene.hpp"
#include "supporting_function.h"

#include <memory>

class Ship;

enum AircraftStatus {
	ReadyToFlight,
	TakeOff,
	LayInACourse,
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

	bool _isTakeOffFinished();
	
	scene::Mesh* mesh;
	Ship& _mothership;
	
	Vector2 position;
	Vector2 speed;
	float angle;
	
	AircraftStatus status;

	//Distance from Ship center
	float relativePosition;

};