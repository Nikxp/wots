

#include <cassert>
#include <cmath>

#include "ship.h"
#include "aircraft.h"


//-------------------------------------------------------
//	game public interface
//-------------------------------------------------------

namespace game
{
	Ship ship;


	void init()
	{
		ship.init();
	}


	void deinit()
	{
		ship.deinit();
	}


	void update(float dt)
	{
		ship.update(dt);
	}


	void keyPressed(int key)
	{
		ship.keyPressed(key);
	}


	void keyReleased(int key)
	{
		ship.keyReleased(key);
	}


	void mouseClicked(float x, float y, bool isLeftButton)
	{
		Vector2 worldPosition(x, y);
		scene::screenToWorld(&worldPosition.x, &worldPosition.y);
		ship.mouseClicked(worldPosition, isLeftButton);
	}
}

