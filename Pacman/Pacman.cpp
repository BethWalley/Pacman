#include "Pacman.h"
#include <sstream>
#include <iostream>


Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	int i;
	_pacman = new Player{ 250,1 };
	_pacman->dead = false;
	_cherry = new Cherry{ 500 };

		_ghosts = new Ghost{ 4 };
		_ghosts->direction = 0;
		_ghosts->speed = 0.2;

	for ( i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy{ 500 };
		_munchies[i]->mCurrentFrameTime = 0;
		_munchies[i]->dead = false;
	}
	_pop = new SoundEffect;
	_dead = new SoundEffect;
	_cherrySound = new SoundEffect;
	_paused = false;
	_pKeyDown = false;
	_start = true;
	_spaceKeyDown = false;

	int playerScore = 0;
	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	int i;

	delete _pacman->pTexture;
	delete _pacman->pSourceRect;
	delete _pacman->pPosition;
	delete _pacman;
	
	for ( i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchies[i]->mBlueTexture;
		delete _munchies[i]->mInvertedTexture;
		delete _munchies[i]->mRect;
		delete _munchies[i];
	}

	delete _cherry->Texture;
	delete _cherry->Position;
	delete _cherry->Rect;
	delete _cherry;

	delete _ghosts->texture;
	delete _ghosts->position;
	delete _ghosts->rect;
	delete _ghosts;

	delete _pop;
	delete _dead;
	delete _cherrySound;
}

void Pacman::LoadContent()
{
	int i;
	srand(time(NULL));

	// Load Pacman
	_pacman->pTexture = new Texture2D();
	_pacman->pTexture->Load("Textures/Pacman.tga", false);
	_pacman->pPosition = new Vector2(350.0f, 350.0f);
	_pacman->pSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_pacman->pDirection = 0;
	_pacman->pCurrentFrameTime = 2; 
	_pacman->pFrame = 0;
	_pacman->speedMultiplier = 1.0f;

	// Load Munchie
	
	for ( i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->mBlueTexture = new Texture2D();
		_munchies[i]->mBlueTexture->Load("Textures/Munchie.tga", true);
		_munchies[i]->mInvertedTexture = new Texture2D();
		_munchies[i]->mInvertedTexture->Load("Textures/MunchieInverted.tga", false);
		_munchies[i]->mRect = new Rect(60.0f, 60.0f, 12, 12);
		_munchies[i]->mCurrentFrameTime = 0;
		_munchies[i]->mPosition = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}

	// Load cherry
	_cherry->Texture = new Texture2D();
	_cherry->Texture->Load("Textures/Cherry.png", true);
	_cherry->InvertedTexture = new Texture2D();
	_cherry->InvertedTexture->Load("Textures/CherryInverted.png", false);
	_cherry->Rect = new Rect(90.0f, 95.0f, 35, 35);
	_cherry->CurrentFrameTime = 0;
	_cherry->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));

	// Load ghosts
	_ghosts->texture = new Texture2D();
	_ghosts->texture->Load("Textures/GhostBlue.png", false);
	_ghosts->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	_ghosts->rect = new Rect(0.0f, 0.0f, 20, 20);


	// Load sounds
	_pop->Load("Sounds/pop.wav");
	_dead->Load("Sounds/dead.wav");
	_cherrySound->Load("Sounds/cherry.wav");


	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set menu parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	// set start parameters
	_startBackground = new Texture2D();
	_startBackground->Load("Textures/Transparency.png", false);
	_startRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_startStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
}

void Pacman::Update(int elapsedTime)
{

	// Gets the current state of the keyboard and mouse
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();
	_pacman->pCurrentFrameTime += elapsedTime;
	int i;
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->mCurrentFrameTime += elapsedTime;
	}
	_pacman->pSourceRect->Y = _pacman->pSourceRect->Height * _pacman->pDirection;
	_pacman->pSourceRect->X = _pacman->pSourceRect->Width * _pacman->pFrame;
	//start
	if (keyboardState->IsKeyDown(Input::Keys::SPACE) && !_spaceKeyDown)
	{
		_spaceKeyDown = false;
		_start = !_start;
	}
	if (keyboardState->IsKeyDown(Input::Keys::SPACE))
	{
		_spaceKeyDown = true;
	}

	void CheckPaused(Input::KeyboardState * state, Input::Keys pauseKey);

	if (!_paused)
	{
		Input(elapsedTime, keyboardState, mouseState);
		UpdatePacman(elapsedTime);
		UpdateMunchie(elapsedTime);
		UpdateCherry(elapsedTime);
		CheckViewportCollision();
		UpdateGhost(_ghosts, elapsedTime);
		CheckGhostCollisions();
		CheckMunchieCollisions();
		CheckCherryCollisions();
	}
} 
void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	float pacmanSpeed = _pacman->pSpeed * elapsedTime * _pacman->speedMultiplier;
	if (!_paused) {

		// Pacman movement
		if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
		{
			_pacman->speedMultiplier = 2.0f;
		}
		else
		{
			_pacman->speedMultiplier = 0.5f;
		}
		if (state->IsKeyDown(Input::Keys::D))
		{
			_pacman->pPosition->X += pacmanSpeed;
			_pacman->pDirection = 0;
		}
		else if (state->IsKeyDown(Input::Keys::A))
		{
			_pacman->pPosition->X -= pacmanSpeed;
			_pacman->pDirection = 2;
		}
		else if (state->IsKeyDown(Input::Keys::S))
		{
			_pacman->pPosition->Y += pacmanSpeed;
			_pacman->pDirection = 1;
		}
		else if (state->IsKeyDown(Input::Keys::W))
		{
			_pacman->pPosition->Y -= pacmanSpeed;
			_pacman->pDirection = 3;
		}
		if (state->IsKeyDown(Input::Keys::R))
		{
			srand(time(NULL));
			_cherry->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		}
		if (mouseState->LeftButton == Input::ButtonState::PRESSED)
		{
			_cherry->Position->X = mouseState->X;
			_cherry->Position->Y = mouseState->Y;
		}
	}
}

void Pacman::UpdateGhost(Ghost* ghosts, int elapsedTime)
{
	if (ghosts->direction == 0)
	{
		ghosts->position->X += ghosts->speed * elapsedTime;
	}
	else if (ghosts->direction == 1)
	{
		ghosts->position->X -= ghosts->speed * elapsedTime;
	}
	if (ghosts->position->X + ghosts->rect->Width >= Graphics::GetViewportWidth())
	{
		ghosts->direction = 1;
	}
	else if (ghosts->position->X <= 0)
	{
		ghosts->direction = 0;
	}
}

void Pacman::CheckGhostCollisions()
{
	int i = 0;
	int bottom1 = _pacman->pPosition->Y + _pacman->pSourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->pPosition->X;
	int left2 = 0;
	int right1 = _pacman->pPosition->X + _pacman->pSourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->pPosition->Y;
	int top2 = 0;

	
	
		bottom2 = _ghosts->position->Y + _ghosts->rect->Height;
		left2 = _ghosts->position->X;
		right2 = _ghosts->position->X + _ghosts->rect->Width;
		top2 = _ghosts->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			Audio::Play(_dead);
			playerScore = 0;

		}
	}

void Pacman::CheckMunchieCollisions()
{
	int i = 0;
	int bottom1 = _pacman->pPosition->Y + _pacman->pSourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->pPosition->X;
	int left2 = 0;
	int right1 = _pacman->pPosition->X + _pacman->pSourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->pPosition->Y;
	int top2 = 0;

	for (i = 0; i < MUNCHIECOUNT; i++) {
		bottom2 = _munchies[i]->mPosition->Y + _munchies[i]->mRect->Height;
		left2 = _munchies[i]->mPosition->X;
		right2 = _munchies[i]->mPosition->X + _munchies[i]->mRect->Width;
		top2 = _munchies[i]->mPosition->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_munchies[i]->dead = true;
			i = MUNCHIECOUNT;
			Audio::Play(_pop);
			playerScore = playerScore + 100;
			break;
		}
	}
}

void Pacman::CheckCherryCollisions() 
{
	int i = 0;
	int bottom1 = _pacman->pPosition->Y + _pacman->pSourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->pPosition->X;
	int left2 = 0;
	int right1 = _pacman->pPosition->X + _pacman->pSourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->pPosition->Y;
	int top2 = 0;



	bottom2 = _cherry->Position->Y + _cherry->Rect->Height;
	left2 = _cherry->Position->X;
	right2 = _cherry->Position->X + _cherry->Rect->Width;
	top2 = _cherry->Position->Y;

	if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
	{
		_cherry->dead = true;
		Audio::Play(_cherrySound);
		playerScore = playerScore + 500;
	}
}



void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	//pause
	if (state->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = false;
	}
	if (state->IsKeyUp(Input::Keys::P))
	{
		_pKeyDown = false;
		_paused = true;
	}
}

void Pacman::CheckViewportCollision()
{
	//Pacman hitting walls
// right wall
	if (_pacman->pPosition->X + _pacman->pSourceRect->Width > Graphics::GetViewportWidth())
	{
		_pacman->pPosition->X = 0;
	}
	// left wall
	else if (_pacman->pPosition->X < 0)
	{
		_pacman->pPosition->X = Graphics::GetViewportWidth() - _pacman->pSourceRect->Width;
	}
	//bottom wall
	else if (_pacman->pPosition->Y + _pacman->pSourceRect->Height > Graphics::GetViewportHeight())
	{
		_pacman->pPosition->Y = 0;
	}
	//top wall
	else if (_pacman->pPosition->Y < 0)
	{
		_pacman->pPosition->Y = Graphics::GetViewportHeight() - _pacman->pSourceRect->Height;
	}
}

void Pacman::UpdatePacman(int elapsedTime)
{
	if (_pacman->pCurrentFrameTime > _pacman->pFrameTime)
	{
		_pacman->pFrame++;
		if (_pacman->pFrame >= 20)
		{
			_pacman->pFrame = 0;
			_pacman->pCurrentFrameTime = 0;
		}
	}
}

void Pacman::UpdateMunchie(int elapsedTime)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		if (_munchies[i]->mCurrentFrameTime > _munchies[i]->mFrameTime)
		{
			_munchies[i]->mFrameCount++;
			if (_munchies[i]->mFrameCount >= 10)
			{
				_munchies[i]->mFrameCount = 0;
				_munchies[i]->mCurrentFrameTime = 0;
			}
		}
	}
}

void Pacman::UpdateCherry(int elapsedTime)
{
	if (_cherry->CurrentFrameTime > _cherry->FrameTime)
	{
		_cherry->FrameCount++;
		if (_cherry->FrameCount >= 10)
		{
			_cherry->FrameCount = 0;
			_cherry->CurrentFrameTime = 0;
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	int i;

	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->pPosition->X << " Y: " << _pacman->pPosition->Y << "\n Current player score: " << playerScore;
	//draw pacman
	SpriteBatch::BeginDraw(); // Starts Drawing
	if (!_pacman->dead) 
	{
		SpriteBatch::Draw(_pacman->pTexture, _pacman->pPosition, _pacman->pSourceRect);
	}
	// draw start menu
	if(_start)
	{
		std::stringstream startStream;
		startStream << " PRESS [SPACE] TO START";

		SpriteBatch::Draw(_startBackground, _startRectangle, nullptr);
		SpriteBatch::DrawString(startStream.str().c_str(), _startStringPosition, Color::White);
	}
	// draw munchies
	
	for ( i = 0; i < MUNCHIECOUNT; i++)
	{
		
			if (_munchies[i]->mFrameCount == 0)
			{
				if (!_munchies[i]->dead)
				{
					SpriteBatch::Draw(_munchies[i]->mInvertedTexture, _munchies[i]->mPosition, _munchies[i]->mRect);
				}
			}
			else
			{
				if (!_munchies[i]->dead)
				{
					SpriteBatch::Draw(_munchies[i]->mBlueTexture, _munchies[i]->mPosition, _munchies[i]->mRect);
				}
				if (_munchies[i]->mFrameCount > 60)
				{
					_munchies[i]->mFrameCount = 0;
				}
			}
		
	}
	// draw cherry
	if (_cherry->FrameCount < 30)
	{
		if (!_cherry->dead)
		{
			SpriteBatch::Draw(_cherry->InvertedTexture, _cherry->Position, _cherry->Rect);
		}
		_cherry->FrameCount++;
	}
	else
	{
		if (!_cherry->dead)
		{
			SpriteBatch::Draw(_cherry->Texture, _cherry->Position, _cherry->Rect);
		}

		if (_cherry->FrameCount > 60)
		{
			_cherry->FrameCount == 0;
		}
	}
	// draw ghost
	
		SpriteBatch::Draw(_ghosts->texture, _ghosts->position, _ghosts->rect);
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	// draw pause menu
	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}
	SpriteBatch::EndDraw(); // Ends Drawing
}