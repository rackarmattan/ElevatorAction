#pragma once

#include "SDL.h"
#include "SDL_ttf.h"

class Sprite
{
	SDL_Renderer * renderer;
	SDL_Texture * texture;

public:

	Sprite(SDL_Renderer * renderer, SDL_Texture * texture);

	// Destroys the sprite instance
	void destroy();

	// Draw the sprite at the given position.
	// Valid coordinates are between (0,0) (upper left) and (width-32, height-32) (lower right).
	// (All sprites are 32*32 pixels, clipping is not supported)
	void draw(int x, int y, int width, int height);
};


class AvancezLib
{
public:
	// Destroys the avancez library instance
	void destroy();

	// Destroys the avancez library instance and exits
	void quit();

	// Creates the main window. Returns true on success.
	bool init(int width, int height);

	// Clears the screen and draws all sprites and texts which have been drawn
	// since the last update call.
	// If update returns false, the application should terminate.
	void processInput();

	void swapBuffers();

	void clearWindow();

	// Create a sprite given a string.
	// All sprites are 32*32 pixels.
	Sprite* createSprite(const char* name);

	// Draws the given text.
	void drawText(int x, int y, const char* msg);

	// Return the total time spent in the game, in seconds.
	float getElapsedTime();

	struct KeyStatus
	{
		bool fire; // space
		bool left; // left arrow
		bool right; // right arrow
		bool esc; // escape button
		bool jump;
		bool duck;
		bool restart;
	};

	// Returns the keyboard status. If a flag is set, the corresponding key is being held down.
	void getKeyStatus(KeyStatus& keys);

	///////////////////BELOW ADDED FOR PROJECT////////////////////////////////////////////////

	/*Get the keys that was just released*/
	void getJustReleased(KeyStatus& keys);

	/*Get the keys that was just pressed*/
	void getJustPressed(KeyStatus& keys);

	/*Make the screen flimmer between yellow and blue*/
	void displayMixedBackgroundColors();

	/*Set the screen's background colour*/
	void setBackgroundColor(int red, int green, int blue);

	/////////////////////////////////////////////////////////////////////////////////////////

private:
	SDL_Window * window;
	SDL_Renderer * renderer;

	TTF_Font* font;

	KeyStatus key;
};

