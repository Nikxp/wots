#include "supporting_function.h"

Vector2::Vector2() :
	x(0.f),
	y(0.f)
{
}


Vector2::Vector2(float vx, float vy) :
	x(vx),
	y(vy)
{
}


Vector2::Vector2(Vector2 const& other) :
	x(other.x),
	y(other.y)
{
}

float Vector2::lengthSquare() {
	return x * x + y * y;
}


Vector2 operator + (Vector2 const& left, Vector2 const& right)
{
	return Vector2(left.x + right.x, left.y + right.y);
}

Vector2 operator- (Vector2 const& left, Vector2 const& right)
{
	return left + (-1) * right;
}

Vector2 operator * (float left, Vector2 const& right)
{
	return Vector2(left * right.x, left * right.y);
}

bool isVectorsClockviseOrder(Vector2 const& first, Vector2 const& second) {
	return (first.x * second.y - first.y * second.x) >= -params::precision::ZERO_COMPARISON;
}