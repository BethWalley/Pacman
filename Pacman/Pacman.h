#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif
#define MUNCHIECOUNT 10
#define GHOSTCOUNT 1
// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
	// Data to represent Pacman
struct Player
{
	const int pFrameTime;
	const float pSpeed;

	Vector2* pPosition;
	Rect* pSourceRect;
	Texture2D* pTexture;
	int pDirection;
	int pFrame;
	int pCurrentFrameTime;
	float speedMultiplier;
	bool dead;

};

struct Enemy
{
	// Data to represent Munchie
	const int mFrameTime;
	int mFrameCount;
	Rect* mRect;
	Texture2D* mBlueTexture;
	Texture2D* mInvertedTexture;
	int mFrame;
	int mCurrentFrameTime;
	Vector2* mPosition;
	bool dead;
};

struct Cherry
{
	const int FrameTime;
	int FrameCount;
	Rect* Rect;
	Texture2D* Texture;
	Texture2D* InvertedTexture;
	int Frame;
	int CurrentFrameTime;
	Vector2* Position;
	Vector2* RandPosition;
	bool dead;
};

struct Ghost
{
	const int frameTime;
	float speed;
	Vector2* position;
	Texture2D* texture;
	Rect* rect;
	int direction;

};

class Pacman : public Game
{
private:
	Player* _pacman;

	Enemy* _munchies[MUNCHIECOUNT];

	Cherry* _cherry;

	Ghost* _ghosts;

	SoundEffect* _pop;

	SoundEffect* _dead;

	SoundEffect* _cherrySound;

	// Position for String
	Vector2* _stringPosition;

	// Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle; 
	Vector2*_menuStringPosition; 
	bool _paused;
	bool _pKeyDown;

	// Data for start screen
	Texture2D* _startBackground;
	Rect* _startRectangle;
	Vector2* _startStringPosition;
	bool _start;
	bool _spaceKeyDown;

	int playerScore = 0;
	//Input methods
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	//Check methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();

	//Update methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(int elapsedTime);
	void UpdateCherry(int elapsedTime);

	// Ghost methods
	void CheckGhostCollisions();
	void UpdateGhost(Ghost*, int elapsedTime);

	void CheckMunchieCollisions();

	void CheckCherryCollisions();

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};