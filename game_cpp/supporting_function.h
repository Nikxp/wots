#pragma once
#include "../framework/scene.hpp"
#include "../framework/game.hpp"

//-------------------------------------------------------
//	game parameters
//-------------------------------------------------------

namespace params
{
	namespace ship
	{
		constexpr float LINEAR_SPEED = 0.5f;
		constexpr float ANGULAR_SPEED = 0.5f;
		constexpr int AIRCRAFT_SHIP_CAPACITY = 5;
	}

	namespace aircraft
	{
		constexpr float LINEAR_SPEED = 2.f;
		constexpr float ANGULAR_SPEED = 2.5f;
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
};

Vector2 operator+ (Vector2 const& left, Vector2 const& right);
Vector2 operator* (float left, Vector2 const& right);
