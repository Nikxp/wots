#pragma once
#include "../framework/scene.hpp"
#include "supporting_function.h"

#include <memory>

class Ship;

enum AircraftStatus {
	ReadyToFlight,
	TakeOff,
	LayInACourse,
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

	void changeInternalState(float acceleration, float deltaAngle, float dt);
	bool _isTakeOffFinished();

	bool _isReturningTime();
	float _timeForMakingCircle();
	float _timeForReturning();
	float _timeForLanding();

	bool _isAircraftNearTheMothership();
	float _getDeltaSpeed(float acceleration, float dt, float targetSpeed);
	bool _isOnCourse();
	float _getRelativePatrolAngle(float patrolRadius, bool* status);
	bool _isBrakeTime(float targetSpeed, float acceleration, Vector2 targetPosition);
	float _getAcceleration(Vector2 target, float targetSpeed, float dt);
	float _getAcceleration(float targetSpeed, float dt);
	float _getLayInACourseDeltaAngle(float dt);
	
	//return value is [-pi; pi]
	float _getVectorsAngleDistance(float first, float second);

	
	scene::Mesh* _mesh;
	Ship& _mothership;
	
	Vector2 _position;
	float _speed;
	float _angle;
	float _flightTime;

	Vector2 _target;
	
	AircraftStatus _status;

	//Distance from Ship center
	float _distanceToShip;

};