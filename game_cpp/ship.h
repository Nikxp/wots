#pragma once
#include "../framework/game.hpp"
#include "aircraft.h"
#include "supporting_function.h"
#include <vector>

class Aircraft;

class Ship
{
public:
	Ship();

	void init();
	void deinit();
	void update(float dt);
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseClicked(Vector2 worldPosition, bool isLeftButton);
	Vector2 getPosition();
	float getAngle();

private:
	scene::Mesh* mesh;
	Vector2 position;
	float angle;

	bool input[game::KEY_COUNT];
	std::vector<Aircraft> aircraftStorage;
};

class ship
{
};
