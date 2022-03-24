#pragma once
#include "../framework/scene.hpp"
#include "../framework/game.hpp"

//-------------------------------------------------------
//	game parameters
//-------------------------------------------------------

namespace params
{
	namespace precision
	{
		constexpr float ZERO_COMPARISON = 1.e-6f;
		constexpr float PI_CONST = 3.141593f;
		constexpr float PATROL_RADIUS_VARIATION = 1.e-4f;
		constexpr float ON_COURSE_ANGLE_VARIATION = 1.e-4f;
	}
	namespace ship
	{
		constexpr float LINEAR_SPEED = 0.5f;
		constexpr float ANGULAR_SPEED = 0.5f;
		constexpr float LANDING_RADIUS = 0.2f;
		constexpr float FUELLING_COEFFICIENT = 3.f;
		constexpr int AIRCRAFT_SHIP_CAPACITY = 5;
	}

	namespace aircraft
	{
		constexpr float LINEAR_SPEED = 2.f;
		constexpr float LINEAR_ACCELERATION = 0.7f;
		constexpr float ANGULAR_SPEED = 2.5f;
		constexpr float MAXIMAL_FLIGHT_TIME = 30.f;

		constexpr float TAKEOFF_RADIUS = 0.4f;
		constexpr float TAKEOFF_SPEED_COEFICIENT = 0.25f;

		constexpr float PATROL_RADIUS = 0.7f;
		constexpr float PATROL_SPEED_COEFFICIENT = 0.6f;

		// Relativly to ship LINEAR_SPEED;
		constexpr float LANDING_SPEED_COEFFICIENT = 1.5f;
	}
}

//-------------------------------------------------------
//	Basic Vector2 class
//-------------------------------------------------------

class Vector2
{
public:
	float x;
	float y;

	Vector2();
	Vector2(float vx, float vy);
	Vector2(Vector2 const &other);
	//sqrt is not efficiency operation;
	float lengthSquare();
};

Vector2 operator+ (Vector2 const& left, Vector2 const& right);
Vector2 operator- (Vector2 const& left, Vector2 const& right);
Vector2 operator* (float left, Vector2 const& right);
bool isVectorsClockviseOrder(Vector2 const& first, Vector2 const& second);
int sign(float number);
