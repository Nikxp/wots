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
	Patroling,
	Returning,
	Fuelling,
	Count
};

enum TurnDecision {
	OnPatrolCircle,
	OnCourse,
	ToCenter,
	FromCenter
};

class Aircraft
{
public:
	Aircraft(Ship& mothership);
	void init();
	void deinit();
	bool Takeoff();
	void update(float dt);
	void setTarget(Vector2 target);
private:

	bool _isTakeOffFinished();
	TurnDecision _getTurnDecision(float patrolRadius);
	bool _isReturningTime();
	bool _isAircraftNearTheMothership();
	void _setReturnModeIfNeed();
	
	scene::Mesh* _mesh;
	Ship& _mothership;
	
	Vector2 _position;
	float _speed;
	float _angle;
	float _flightTime;

	Vector2 _target;
	
	AircraftStatus _status;

	//Distance from Ship center
	float _relativePosition;

};