

#include <cassert>
#include <cmath>

#include "../framework/scene.hpp"
#include "../framework/game.hpp"
#include <vector>


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
	Vector2( float vx, float vy );
	Vector2( Vector2 const &other );
};


Vector2::Vector2() :
	x( 0.f ),
	y( 0.f )
{
}


Vector2::Vector2( float vx, float vy ) :
	x( vx ),
	y( vy )
{
}


Vector2::Vector2( Vector2 const &other ) :
	x( other.x ),
	y( other.y )
{
}


Vector2 operator + ( Vector2 const &left, Vector2 const &right )
{
	return Vector2( left.x + right.x, left.y + right.y );
}


Vector2 operator * ( float left, Vector2 const &right )
{
	return Vector2( left * right.x, left * right.y );
}

//-------------------------------------------------------
//	Aircraft definition
//-------------------------------------------------------

class Ship;
#include <memory>

enum AircraftStatus {
	ReadyToFlight,
	TakeOff,
	FlyForward,
	Fuelling,
	Count
};

class Aircraft
{
public:
	Aircraft(Ship& mothership);
	void init();
	void deinit();
	void Takeoff();
	void update(float dt);
private:
	scene::Mesh* mesh;
	Ship& _mothership;

	Vector2 position;
	float angle;
	AircraftStatus status;
};


//-------------------------------------------------------
//	Simple ship logic
//-------------------------------------------------------

class Ship
{
public:
	Ship();

	void init();
	void deinit();
	void update( float dt );
	void keyPressed( int key );
	void keyReleased( int key );
	void mouseClicked( Vector2 worldPosition, bool isLeftButton );
	Vector2 getPosition();
	float getAngle();

private:
	scene::Mesh *mesh;
	Vector2 position;
	float angle;

	bool input[ game::KEY_COUNT ];
	std::vector<Aircraft> aircraftStorage;
};

Aircraft::Aircraft(Ship& mothership) :
	mesh(nullptr),
	_mothership(mothership),
	status(AircraftStatus::ReadyToFlight)
{
}

void Aircraft::init() {
	position = Vector2(0.f, 0.f);
	angle = 0.f;

}

void Aircraft::deinit()
{
	if (mesh != NULL) {
		scene::destroyMesh(mesh);
		mesh = nullptr;
	}
}

void Aircraft::Takeoff()
{
	assert(!mesh);
	mesh = scene::createAircraftMesh();
	position = _mothership.getPosition();
	angle = _mothership.getAngle();
	scene::placeMesh(mesh, position.x, position.y, angle);
	status = FlyForward;
}


void Aircraft::update(float dt) {
	float linearSpeed = 0.f;
	float angularSpeed = 0.f;
	switch (status) {
		case ReadyToFlight:
			return;
			break;
		case TakeOff:

			break;
		case FlyForward:
			linearSpeed = params::aircraft::LINEAR_SPEED;
			
			break;
		case Fuelling:
			return;
			break;
	}
	angle = angle;
	position = position + linearSpeed * dt * Vector2(std::cos(angle), std::sin(angle));
	scene::placeMesh(mesh, position.x, position.y, angle);
};

Ship::Ship() :
	mesh( nullptr )
{
}


void Ship::init()
{
	assert( !mesh );
	mesh = scene::createShipMesh();
	position = Vector2( 0.f, 0.f );
	angle = 0.f;
	for ( bool &key : input )
		key = false;
	for (int index = 0; index < params::ship::AIRCRAFT_SHIP_CAPACITY; index++) {
		aircraftStorage.push_back(Aircraft(*this));
		aircraftStorage[index].init();
	}
}


void Ship::deinit()
{
	scene::destroyMesh( mesh );
	mesh = nullptr;
	for (auto& aircraft : aircraftStorage) {
		aircraft.deinit();
	}
}


void Ship::update( float dt )
{
	float linearSpeed = 0.f;
	float angularSpeed = 0.f;

	if ( input[ game::KEY_FORWARD ] )
	{
		linearSpeed = params::ship::LINEAR_SPEED;
	}
	else if ( input[ game::KEY_BACKWARD ] )
	{
		linearSpeed = -params::ship::LINEAR_SPEED;
	}

	if ( input[ game::KEY_LEFT ] && linearSpeed != 0.f )
	{
		angularSpeed = params::ship::ANGULAR_SPEED;
	}
	else if ( input[ game::KEY_RIGHT ] && linearSpeed != 0.f )
	{
		angularSpeed = -params::ship::ANGULAR_SPEED;
	}

	angle = angle + angularSpeed * dt;
	position = position + linearSpeed * dt * Vector2( std::cos( angle ), std::sin( angle ) );
	scene::placeMesh( mesh, position.x, position.y, angle );
	for (auto& aircraft : aircraftStorage) {
		aircraft.update(dt);
	}
}


void Ship::keyPressed( int key )
{
	assert( key >= 0 && key < game::KEY_COUNT );
	input[ key ] = true;
}


void Ship::keyReleased( int key )
{
	assert( key >= 0 && key < game::KEY_COUNT );
	input[ key ] = false;
}


void Ship::mouseClicked( Vector2 worldPosition, bool isLeftButton )
{
	// TODO: placeholder, remove it and implement aircarfts logic
	if ( isLeftButton )
	{
		scene::placeGoalMarker( worldPosition.x, worldPosition.y );
	}
	else
	{
		aircraftStorage[0].Takeoff();
	}
}

Vector2 Ship::getPosition() {
	return position;
}

float Ship::getAngle() {
	return angle;
}


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


	void update( float dt )
	{
		ship.update( dt );
	}


	void keyPressed( int key )
	{
		ship.keyPressed( key );
	}


	void keyReleased( int key )
	{
		ship.keyReleased( key );
	}


	void mouseClicked( float x, float y, bool isLeftButton )
	{
		Vector2 worldPosition( x, y );
		scene::screenToWorld( &worldPosition.x, &worldPosition.y );
		ship.mouseClicked( worldPosition, isLeftButton );
	}
}

