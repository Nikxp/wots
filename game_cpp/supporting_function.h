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
		constexpr float ZERO_COMPARISON = 1e-6;
	}
	namespace ship
	{
		constexpr float LINEAR_SPEED = 0.5f;
		constexpr float ANGULAR_SPEED = 0.5f;
		constexpr int AIRCRAFT_SHIP_CAPACITY = 5;
		constexpr float LANDING_RADIUS = 0.2f;
		constexpr float FUELLING_COEFFICIENT = 3.f;
	}

	namespace aircraft
	{
		constexpr float LINEAR_SPEED = 2.f;
		constexpr float ANGULAR_SPEED = 2.5f;
		constexpr float LINEAR_ACCELERATION = 0.8f;
		constexpr float TAKEOFF_RADIUS = 0.4f;
		constexpr float MAXIMAL_FLIGHT_TIME = 10.f;
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
