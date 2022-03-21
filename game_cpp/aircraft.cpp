#include "aircraft.h"
#include "ship.h"
#include <cassert>
#include <cmath>

//Delete me
#include <iostream>

Aircraft::Aircraft(Ship& mothership) :
	_mesh(nullptr),
	_mothership(mothership),
	_status(AircraftStatus::ReadyToFlight)
{
}

void Aircraft::init() {
	_position = Vector2(0.f, 0.f);
	_angle = 0.f;

}

void Aircraft::deinit()
{
	if (_mesh != NULL) {
		scene::destroyMesh(_mesh);
		_mesh = nullptr;
	}
}

void Aircraft::Takeoff()
{
	assert(!_mesh);
	_mesh = scene::createAircraftMesh();
	_position = _mothership.getPosition();
	_relativePosition = 0.f;
	_angle = _mothership.getAngle();
	scene::placeMesh(_mesh, _position.x, _position.y, _angle);
	_status = TakeOff;
}

void Aircraft::update(float dt) {

	float angularSpeed = 0.f;
	TurnDecision turnDecision;
	switch (_status) {
	case ReadyToFlight:
		return;
		break;
	case TakeOff:
		_speed = params::aircraft::LINEAR_SPEED * 0.25;
		_angle = _mothership.getAngle();
		_relativePosition = _relativePosition + _speed * dt;
		_position = _mothership.getPosition() + _relativePosition * Vector2(std::cos(_angle), std::sin(_angle));
		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
		if (_isTakeOffFinished()) {
			_status = LayInACourse;
		}
		break;
	case LayInACourse:
		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
		turnDecision = _getTurnDecision(0.7f);
		switch (turnDecision) {
		case OnPatrolCircle:
			_status = Patroling;
			break;
			std::cout << "On Patrol" << std::endl;
		case OnCourse:
			_status = FlyForward;
			std::cout << "Forward" << std::endl;
			break;
		case ToCenter:
			_speed = params::aircraft::LINEAR_SPEED * 0.5;
			_angle = _angle + 2 * dt;
			_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));
			std::cout << "To Center" << std::endl;
			break;
		case FromCenter:
			_speed = params::aircraft::LINEAR_SPEED * 0.5;
			_angle = _angle - 2 * dt;
			_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));

			std::cout << "From center" << std::endl;
			break;
		}
		break;
	case FlyForward:
		_speed = params::aircraft::LINEAR_SPEED;
		_angle = _angle;
		_position = _position + _speed * dt * Vector2(std::cos(_angle), std::sin(_angle));
		scene::placeMesh(_mesh, _position.x, _position.y, _angle);
		break;
	case Fuelling:
		return;
		break;
	}
}

void Aircraft::setTarget(Vector2 target) {
	_target = target;
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