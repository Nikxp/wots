#include "aircraft.h"
#include "ship.h"
#include <cassert>
#include <cmath>
#include <algorithm>

Aircraft::Aircraft(Ship& mothership) :
	_mesh(nullptr),
	_mothership(mothership),
	_status(AircraftStatus::ReadyToFlight),
	_flightTime(0.f)
{
}

void Aircraft::init() {
	_position = Vector2(0.f, 0.f);
	_angle = 0.f;
	_speed = 0.f;
}

void Aircraft::deinit()
{
	if (_mesh != NULL) {
		scene::destroyMesh(_mesh);
		_mesh = nullptr;
	}
	_status = ReadyToFlight;
}

bool Aircraft::Takeoff()
{
	if (_status == ReadyToFlight) {
		assert(!_mesh);
		_mesh = scene::createAircraftMesh();
		_position = _mothership.getPosition();
		_distanceToShip = 0.f;
		_angle = _mothership.getAngle();
		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
		_status = TakeOff;
		return true;
	}
	return false;
}

void Aircraft::update(float dt) {
	_flightTime += dt;
	float acceleration = 0.f;
	float deltaAngle = 0.f;

	switch (_status) {
	case ReadyToFlight: {
		_flightTime = 0;
		return;
		break;
	}
	case TakeOff: {
		acceleration = params::aircraft::LINEAR_ACCELERATION;
		float deltaSpeed = _getDeltaSpeed(params::aircraft::LINEAR_ACCELERATION, dt, params::aircraft::TAKEOFF_SPEED_COEFICIENT * params::aircraft::LINEAR_SPEED);
		
		_angle = _mothership.getAngle();
		_distanceToShip = _distanceToShip + _speed * dt + deltaSpeed * dt * 0.5f;
		_position = _mothership.getPosition() + _distanceToShip * Vector2(std::cos(_angle), std::sin(_angle));
		_speed += deltaSpeed;

		scene::placeMesh(_mesh, _position.x, _position.y, _angle);

		if (_isReturningTime()) {
			_status = Returning;
			break;
		}
		if (_isTakeOffFinished()) {
			_status = LayInACourse;
		}
		break;
	}
	case LayInACourse: {
		bool isSuccess; 

		if (_isOnCourse()) {
			float patrolSpeed = params::aircraft::LINEAR_SPEED * params::aircraft::PATROL_SPEED_COEFFICIENT;
			acceleration = _getAcceleration(_target, patrolSpeed, dt);
		}

		changeInternalState(acceleration, _getLayInACourseDeltaAngle(dt), dt);

		scene::placeMesh(_mesh, _position.x, _position.y, _angle);

		if (_isReturningTime()) {
			_status = Returning;
			break;
		}
		break; 
	}
	case Returning: {
		Vector2 vectorToMothership = _mothership.getPosition() - _position;
		Vector2 angleVector(cos(_angle), sin(_angle));

		float returnSpeed = std::min(params::ship::LINEAR_SPEED * params::aircraft::LANDING_SPEED_COEFFICIENT, params::aircraft::LINEAR_SPEED* 1.f);
		acceleration = _getAcceleration(_mothership.getPosition(), returnSpeed, dt);
		
		float relativePatrolAngle = _getVectorsAngleDistance(atan2(vectorToMothership.y, vectorToMothership.x), _angle);
		if (abs(relativePatrolAngle) <= params::aircraft::ANGULAR_SPEED * dt) {
			deltaAngle = relativePatrolAngle;
		}
		else {
			deltaAngle = sign(relativePatrolAngle) * (params::aircraft::ANGULAR_SPEED * dt);
		}

		changeInternalState(acceleration, deltaAngle, dt);

		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
		
		if (_isAircraftNearTheMothership()) {
			_status = Fuelling;
			scene::destroyMesh(_mesh);
			_mesh = nullptr;
		}
		break;
	}
	case Fuelling: {
		_flightTime -= dt * (params::ship::FUELLING_COEFFICIENT + 1);
		if (_flightTime < 0) {
			_flightTime = 0;
			_status = ReadyToFlight;
		}
		return;
		break;
	}
	}
}

void Aircraft::setTarget(Vector2 target) {
	if (_status != Returning) {
		_target = target;
		if ((_status != Fuelling) && (_status !=ReadyToFlight) && (_status != TakeOff)) {
			_status = LayInACourse;
		}
	}
}

void Aircraft::changeInternalState(float acceleration, float deltaAngle, float dt) {
	_angle = fmod(_angle + deltaAngle, 2 * params::precision::PI_CONST);
	_position = _position + (_speed * dt + acceleration * pow(dt, 2) * 0.5f) * Vector2(std::cos(_angle), std::sin(_angle));
	_speed = _speed + acceleration * dt;
}

bool Aircraft::_isTakeOffFinished() {
	return params::aircraft::TAKEOFF_RADIUS < _distanceToShip;
}

bool Aircraft::_isReturningTime() {
	assert(params::aircraft::LINEAR_SPEED > params::ship::LINEAR_SPEED);
	float timeForReturning = _timeForMakingCircle() + _timeForReturning() + _timeForLanding();
	return _flightTime + timeForReturning  > params::aircraft::MAXIMAL_FLIGHT_TIME;
}


float Aircraft::_timeForMakingCircle() {
	return 2 * params::precision::PI_CONST / params::aircraft::ANGULAR_SPEED;
}

float Aircraft::_timeForReturning() {
	return sqrt((_mothership.getPosition() - _position).lengthSquare()) / (params::aircraft::LINEAR_SPEED - params::ship::LINEAR_SPEED);
}

float Aircraft::_timeForLanding() {
	float timeForBraking = (params::aircraft::LINEAR_SPEED - params::aircraft::LANDING_SPEED_COEFFICIENT * params::ship::LINEAR_SPEED) / params::aircraft::LINEAR_ACCELERATION;
	float timeForChase = timeForBraking / (params::aircraft::LANDING_SPEED_COEFFICIENT - 1);
	return timeForBraking + timeForChase;
}


bool Aircraft::_isAircraftNearTheMothership() {
	if ((_mothership.getPosition() - _position).lengthSquare() <= std::pow(params::ship::LANDING_RADIUS, 2)) {
		return true;
	}
	return false;
}


float Aircraft::_getDeltaSpeed(float acceleration, float dt, float targetSpeed) {
	if (abs(targetSpeed - _speed) < dt * acceleration) {
		return targetSpeed - _speed;
	}
	return sign(targetSpeed - _speed) * dt * acceleration;
}

bool Aircraft::_isOnCourse() {
	bool isSuccess;
	float relativeCourseAngle = _getRelativePatrolAngle(params::aircraft::PATROL_RADIUS, &isSuccess);
	if (!isSuccess) {
		return false;
	}
	if (abs(relativeCourseAngle) <= params::precision::ON_COURSE_ANGLE_VARIATION) {
		return true;
	}
	return false;	
}


float Aircraft::_getRelativePatrolAngle(float patrolRadius, bool* isSuccess) {
	Vector2 pathToTarget = _target - _position;
	float pathLength = sqrt(pathToTarget.lengthSquare());
	if (pathLength <= patrolRadius) {
		if (isSuccess != NULL) {
			*isSuccess = false;
		}
		return false;
	}
	else {
		float radiusAngle = asin(patrolRadius / pathLength);
		float targetAngle = atan2(pathToTarget.y, pathToTarget.x);
		float patrolAngle = targetAngle - radiusAngle;
		float pi = params::precision::PI_CONST;
		float relativePatrolAngle = _getVectorsAngleDistance(patrolAngle, _angle);
		//float relativePatrolAngle = fmod((patrolAngle - _angle + (pi)+2 * (pi)), (pi * 2)) - pi;
		if (isSuccess != NULL) {
			*isSuccess = true;
		}
		return relativePatrolAngle;
	}
}

bool Aircraft::_isBrakeTime(float targetSpeed, float acceleration, Vector2 targetPosition) {
	if (targetSpeed >= _speed) {
		return false;
	}
	float timeToBrake = (_speed - targetSpeed) / acceleration;
	float brakingDistance = targetSpeed * timeToBrake + acceleration * pow(timeToBrake,2)* 0.5f;
	return sqrt((targetPosition - _position).lengthSquare()) - params::aircraft::PATROL_RADIUS < brakingDistance;
}

float Aircraft::_getAcceleration(Vector2 target, float targetSpeed, float dt) {
	if (_isBrakeTime(targetSpeed, params::aircraft::LINEAR_ACCELERATION, target)) {
		return _getAcceleration(targetSpeed, dt);
	}
	else {
		return _getAcceleration(params::aircraft::LINEAR_SPEED, dt);
	}
}

float Aircraft::_getAcceleration(float targetSpeed, float dt) {
	float oneTickAcceleration = (targetSpeed - _speed) / dt;
	if (abs(oneTickAcceleration) > params::aircraft::LINEAR_ACCELERATION) {
		return sign(oneTickAcceleration) * params::aircraft::LINEAR_ACCELERATION;
	}
	return oneTickAcceleration;
}


float Aircraft::_getLayInACourseDeltaAngle(float dt) {
	bool isSuccess;
	float relativePatrolAngle = _getRelativePatrolAngle(params::aircraft::PATROL_RADIUS, &isSuccess);
	if (abs(relativePatrolAngle) <= params::aircraft::ANGULAR_SPEED * dt) {
		return relativePatrolAngle;
	}
	else {
		return sign(relativePatrolAngle) * (params::aircraft::ANGULAR_SPEED * dt);
	}
}

float Aircraft::_getVectorsAngleDistance(float first, float second) {
	float pi = params::precision::PI_CONST;

	//We add PI before fmod and substract PI after fmod for making result in [-PI, PI];
	float difference = first - second + (pi);
	if (difference < 0) {
		difference = fmod(difference, 2 * pi);
		//We add 2PI for guarantee angle positivness;
		difference += 2 * pi;
	}
	else {
		difference = fmod(difference, 2 * pi);
	}
	difference -= pi;
	return difference;

	
}