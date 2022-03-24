#include "aircraft.h"
#include "ship.h"
#include <cassert>
#include <cmath>

//Delete me
#include <iostream>

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
		_relativePosition = 0.f;
		_angle = _mothership.getAngle();
		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
		_status = TakeOff;
		return true;
	}
	return false;
}

void Aircraft::update(float dt) {
	// Placeholder::
	// Set speed more complex
	// Refactor code
	_flightTime += dt;
	float patrolRadius = 0.7f;
	switch (_status) {
	case ReadyToFlight: {
		_flightTime = 0;
		return;
		break;
	}
	case TakeOff: {
		float acceleration = params::aircraft::LINEAR_ACCELERATION;
		float deltaSpeed = _deltaSpeed(params::aircraft::LINEAR_ACCELERATION, dt, 0.25 * params::aircraft::LINEAR_SPEED);
		_angle = _mothership.getAngle();
		_relativePosition = _relativePosition + _speed * dt + deltaSpeed * dt * 0.5;
		_position = _mothership.getPosition() + _relativePosition * Vector2(std::cos(_angle), std::sin(_angle));
		_speed += deltaSpeed;
		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
		if (_isTakeOffFinished()) {
			_status = LayInACourse;
		}
		break;
	}
	case LayInACourse: {

//		std::cout << "Lay On Course" << std::endl;
//		TurnDecision turnDecision = _getTurnDecision(0.7f);
//		switch (turnDecision) {
//			case OnPatrolCircle: {
//				_status = Patroling;
//				break;
//			}
//			case OnCourse: {
//				_status = FlyForward;
//				break;
//			}
//			case ToCenter: {
//				_speed = params::aircraft::LINEAR_SPEED * 0.5;
//				_angle = _angle + 2 * dt;
//				_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));
//				break;
//			}
//			case FromCenter: {
//				_speed = params::aircraft::LINEAR_SPEED * 0.5;
//				_angle = _angle - 2 * dt;
//				_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));
//				break;
//			}
//		}
//		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
//		if (_isReturningTime()) {
//			_status = Returning;
//		}
//		break;
		bool isSuccess; 
		float acceleration = 0.f;
		if (_isOnCourse()) {
			float patrolSpeed = params::aircraft::LINEAR_SPEED * params::aircraft::PATROL_SPEED_COEFFICIENT;
			if (_isBrakeTime(patrolSpeed, params::aircraft::LINEAR_ACCELERATION)) {
				acceleration = _getAcceleration(patrolSpeed,dt);
			}
			else {
				acceleration = _getAcceleration(params::aircraft::LINEAR_SPEED, dt);
			}
		}

		float relativePatrolAngle = _getRelativePatrolAngle(params::aircraft::PATROL_RADIUS, &isSuccess);
		if (abs(relativePatrolAngle) <= params::aircraft::ANGULAR_SPEED * dt) {
			_angle += relativePatrolAngle;
		}
		else {
			_angle += sign(relativePatrolAngle) * (params::aircraft::ANGULAR_SPEED * dt);
		}
		_angle = fmod(_angle, 2 * params::precision::PI_CONST);

		if (abs(_angle) > params::precision::PI_CONST * 2) {
			std::cout << "WARNING!";
		}
		
		_position = _position + (_speed * dt + acceleration*pow(dt,2)) * Vector2(std::cos(_angle), std::sin(_angle));
		_speed = _speed + acceleration * dt;
		scene::placeMesh(_mesh, _position.x, _position.y, _angle);

		if (_isReturningTime()) {
			_status = Returning;
			break;
		}
		break; 
	}
	case FlyForward: {
		std::cout << "Fly forward" << std::endl;
		_speed = params::aircraft::LINEAR_SPEED;
		_angle = _angle;
		_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));
		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
		if (_isReturningTime()) {
			_status = Returning;
			break;
		}
	
		Vector2 pathToTarget = _target - _position;
		float pathLength = sqrt(pathToTarget.lengthSquare());
	
		if (pathLength <= patrolRadius + params::precision::PATROL_RADIUS_VARIATION) {
			_status = Patroling;
		}
		break;
	}
	case Patroling: {
		std::cout << "Patroling" << std::endl;
				TurnDecision turnDecision = _getTurnDecision(0.7f);
				switch (turnDecision) {
					case OnPatrolCircle: {
						//_status = Patroling;
						break;
					}
					case OnCourse: {
						//_status = FlyForward;
						break;
					}
					case ToCenter: {
						_speed = params::aircraft::LINEAR_SPEED * 0.5;
						_angle = _angle + 2 * dt;
						_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));
						break;
					}
					case FromCenter: {
						_speed = params::aircraft::LINEAR_SPEED * 0.5;
						_angle = _angle - 2 * dt;
						_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));
						break;
					}
				}
				scene::placeMesh(_mesh, _position.x, _position.y, _angle);
				if (_isReturningTime()) {
					_status = Returning;
				}
				break;
	}
	case Returning: {
		Vector2 vectorToMothership = _mothership.getPosition() - _position;
		Vector2 angleVector(cos(_angle), sin(_angle));
		_speed = params::aircraft::LINEAR_SPEED * 0.5;
		if (isVectorsClockviseOrder(vectorToMothership, angleVector)) {
			_angle = _angle - 2 * dt;
		}
		else {
			_angle = _angle + 2 * dt;
		}
		_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));
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

bool Aircraft::_isTakeOffFinished() {
	return params::aircraft::TAKEOFF_RADIUS < _relativePosition;
}

// We are finding crossing point patrol circle and plane course
// If we have only one crossing point with positive time param - we are on course
// position + speed * time *Vector(cos(angle), sin (angle)) is on patrol circle
TurnDecision Aircraft::_getTurnDecision(float patrolRadius) {
	Vector2 positionRelativeToTarget = _position - _target;
	
	//We have quadric equation by time.
	float quadraticCoefficient = 1.f;
	float linearCoefficient = 2 *((positionRelativeToTarget.x) * cos(_angle) + (positionRelativeToTarget.y) *sin(_angle));
	float constatntCoefficient = positionRelativeToTarget.x * positionRelativeToTarget.x + positionRelativeToTarget.y * positionRelativeToTarget.y - patrolRadius* patrolRadius;
	
	if (quadraticCoefficient < params::precision::ZERO_COMPARISON) {
		quadraticCoefficient = -quadraticCoefficient;
		linearCoefficient = -linearCoefficient;
		constatntCoefficient = -constatntCoefficient;
	}

	float discriminant = linearCoefficient * linearCoefficient - 4 * quadraticCoefficient * constatntCoefficient;
	if (abs(discriminant) < params::precision::ZERO_COMPARISON) {
		float answer = -linearCoefficient * 0.5 * quadraticCoefficient;
		if (abs(answer) <= -params::precision::ZERO_COMPARISON) {
			return TurnDecision::OnPatrolCircle;
		} else if (answer >= -params::precision::ZERO_COMPARISON) {
				return TurnDecision::OnCourse;
		} else {
			return TurnDecision::FromCenter;
		}

	} else if (discriminant < 0.f) {
		return TurnDecision::ToCenter;

	} else {
		float biggerAnswer = (-linearCoefficient + sqrt(discriminant)) * 0.5 / quadraticCoefficient;
		if (biggerAnswer >= params::precision::ZERO_COMPARISON) {
			return TurnDecision::FromCenter;
		}
		else {
			return TurnDecision::ToCenter;
		}
	}
}

bool Aircraft::_isReturningTime() {
	//Placeholder
	// Check if time calculated correctly (acceleration)
	assert(params::aircraft::LINEAR_SPEED > params::ship::LINEAR_SPEED);
	float timeForReturning = 0.f;
	timeForReturning += sqrt((_mothership.getPosition() - _position).lengthSquare()) / (params::aircraft::LINEAR_SPEED - params::ship::LINEAR_SPEED);
	timeForReturning += 2 * params::precision::PI_CONST / params::aircraft::ANGULAR_SPEED;
	return _flightTime + timeForReturning  > params::aircraft::MAXIMAL_FLIGHT_TIME;
}

bool Aircraft::_isAircraftNearTheMothership() {
	if ((_mothership.getPosition() - _position).lengthSquare() <= std::pow(params::ship::LANDING_RADIUS, 2)) {
		return true;
	}
	return false;
}


float Aircraft::_deltaSpeed(float acceleration, float dt, float targetSpeed) {
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
	*isSuccess = false;
	Vector2 pathToTarget = _target - _position;
	float pathLength = sqrt(pathToTarget.lengthSquare());
	if (pathLength <= patrolRadius) {
		*isSuccess = false;
		return false;
	}
	else {
		float radiusAngle = asin(patrolRadius / pathLength);
		float targetAngle = atan2(pathToTarget.y, pathToTarget.x);
		float patrolAngle = targetAngle - radiusAngle;
		float pi = params::precision::PI_CONST;
		//We add 2PI for guarantee angle positivness;
		//We add PI in fmod and substract PI outside fmod for making result in [-PI, PI];
		float relativePatrolAngle = fmod((patrolAngle - _angle + (pi)+2 * (pi)), (pi * 2)) - pi;
		*isSuccess = true;
		//placeholder
		if (relativePatrolAngle < 0) {
			std::cout << "WARNING!";
		}
		return relativePatrolAngle;
	}
}

bool Aircraft::_isBrakeTime(float targetSpeed, float acceleration) {
	if (targetSpeed >= _speed) {
		return false;
	}
	float timeToBrake = (_speed - targetSpeed) / acceleration;
	float brakingDistance = targetSpeed * timeToBrake + acceleration * pow(timeToBrake,2)* 0.5;
	return sqrt((_target - _position).lengthSquare()) - params::aircraft::PATROL_RADIUS < brakingDistance;
}

float Aircraft::_getAcceleration(float targetSpeed, float dt) {
	float oneTickAcceleration = (targetSpeed - _speed) / dt;
	if (abs(oneTickAcceleration) > params::aircraft::LINEAR_ACCELERATION) {
		return sign(oneTickAcceleration) * params::aircraft::LINEAR_ACCELERATION;
	}
	return oneTickAcceleration;
}