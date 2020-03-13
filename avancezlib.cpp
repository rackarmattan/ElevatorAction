#pragma once

#include "avancezlib.h"

#include <iostream>
#include <string>
#include <map>

using namespace std;

/*Maps for checking which keys are currently pressed and how their state was before*/
map<int, bool> currentKeyboard;
map<int, bool> oldKeyboard;

void AvancezLib::destroy()
{
	SDL_Log("Destroy avancez");
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
}

void AvancezLib::quit()
{
	destroy();
	exit(0);
}


bool AvancezLib::init(int width, int height) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	TTF_Init();

	font = TTF_OpenFont("space_invaders.ttf", 15);

	window = SDL_CreateWindow("A window", 10, 25, width, height,
		SDL_WINDOW_OPENGL);

	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_Log("Init avancez");

	return true;
}

void AvancezLib::processInput()
{
	SDL_Event event;
	oldKeyboard = currentKeyboard;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			currentKeyboard[event.key.keysym.sym] = true;  break;
		case SDL_KEYUP:
			currentKeyboard[event.key.keysym.sym] = false; break;
		}
	}
}

void AvancezLib::swapBuffers()
{
	SDL_RenderPresent(renderer);
}

void AvancezLib::clearWindow()
{
	SDL_RenderClear(renderer);
}

Sprite * AvancezLib::createSprite(const char * name)
{
	SDL_Surface * image = SDL_LoadBMP(name);
	Uint32 colorkey = SDL_MapRGB(image->format, 255, 255, 255);
	SDL_SetColorKey(image, SDL_TRUE, colorkey);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
	Sprite *sprite = new Sprite(renderer, texture);
	SDL_FreeSurface(image);
	return sprite;
}

void AvancezLib::drawText(int x, int y, const char * msg)
{
	SDL_Color color = { 0, 0, 0 };
	SDL_Surface * surface = TTF_RenderText_Solid(font,
		msg, color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer,
		surface);

	int texW = 10;
	int texH = 10;

	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = {x, y, texW, texH };

	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);

}

float AvancezLib::getElapsedTime()
{
	return SDL_GetTicks() / 1000.0;
}

/*Check in the map for the current keyboard status if the keys are pressed*/

void AvancezLib::getKeyStatus(KeyStatus & keys)
{
	if (currentKeyboard[SDLK_LEFT]) {
		keys.left = true;
	}
	else {
		keys.left = false;
	}
	if (currentKeyboard[SDLK_RIGHT]) {
		keys.right = true;
	}
	else {
		keys.right = false;
	}
	if (currentKeyboard[SDLK_SPACE]) {
		keys.fire = true;
	}
	else {
		keys.fire = false;
	}
	if (currentKeyboard[SDLK_ESCAPE]) {
		keys.esc = true;
	}
	else {
		keys.esc = false;
	}
	if (currentKeyboard[SDLK_UP]) {
		keys.jump = true;
	}
	else {
		keys.jump = false;
	}
	if (currentKeyboard[SDLK_DOWN]) {
		keys.duck = true;
	}
	else {
		keys.duck = false;
	}
	if (currentKeyboard[SDLK_r]) {
		keys.restart = true;
	}
	else {
		keys.restart = false;
	}
}

/*Check if the keys in the previous keyboard map was pressed and if not in the current keyboard map.*/

void AvancezLib::getJustReleased(KeyStatus & keys)
{
	if (oldKeyboard[SDLK_LEFT] && !currentKeyboard[SDLK_LEFT]) {
		keys.left = true;
	}
	else {
		keys.left = false;
	}
	if (oldKeyboard[SDLK_RIGHT] && !currentKeyboard[SDLK_RIGHT]) {
		keys.right = true;
	}
	else {
		keys.right = false;
	}
	if (oldKeyboard[SDLK_SPACE] && !currentKeyboard[SDLK_SPACE]) {
		keys.fire = true;
	}
	else {
		keys.fire = false;
	}
	if (oldKeyboard[SDLK_UP] && !currentKeyboard[SDLK_UP]) {
		keys.jump = true;
	}
	else {
		keys.jump = false;
	}
	if (oldKeyboard[SDLK_DOWN] && !currentKeyboard[SDLK_DOWN]) {
		keys.duck = true;
	}
	else {
		keys.duck = false;
	}
	if (oldKeyboard[SDLK_ESCAPE] && !currentKeyboard[SDLK_ESCAPE]) {
		keys.esc = true;
	}
	else {
		keys.esc = false;
	}
	if (oldKeyboard[SDLK_r] && !currentKeyboard[SDLK_r]) {
		keys.restart = true;
	}
	else {
		keys.restart = false;
	}
}

/*Check if the keys in the previous keyboard map was not pressed and if they are in the current keyboard map.*/

void AvancezLib::getJustPressed(KeyStatus & keys)
{
	if (!oldKeyboard[SDLK_LEFT] && currentKeyboard[SDLK_LEFT]) {
		keys.left = true;
	}
	else {
		keys.left = false;
	}
	if (!oldKeyboard[SDLK_RIGHT] && currentKeyboard[SDLK_RIGHT]) {
		keys.right = true;
	}
	else {
		keys.right = false;
	}
	if (!oldKeyboard[SDLK_SPACE] && currentKeyboard[SDLK_SPACE]) {
		keys.fire = true;
	}
	else {
		keys.fire = false;
	}
	if (!oldKeyboard[SDLK_UP] && currentKeyboard[SDLK_UP]) {
		keys.jump = true;
	}
	else {
		keys.jump = false;
	}
	if (!oldKeyboard[SDLK_DOWN] && currentKeyboard[SDLK_DOWN]) {
		keys.duck = true;
	}
	else {
		keys.duck = false;
	}
	if (!oldKeyboard[SDLK_ESCAPE] && currentKeyboard[SDLK_ESCAPE]) {
		keys.esc = true;
	}
	else {
		keys.esc = false;
	}
	if (!oldKeyboard[SDLK_r] && currentKeyboard[SDLK_r]) {
		keys.restart = true;
	}
	else {
		keys.restart = false;
	}
}

/*Swap the background colors to get a fun effetc.*/

void AvancezLib::displayMixedBackgroundColors()
{
	static int startColor = 0;
	static int endColor = 255;
	const int colorStep = 2;
	static bool isEndColor = true;

	if (isEndColor) {
		endColor -= colorStep;
		startColor += colorStep;
		if (endColor == 0) isEndColor = false;
	}
	else {
		startColor -= colorStep;
		endColor += colorStep;
		if (startColor == 0) isEndColor = true;
	}

	SDL_SetRenderDrawColor(renderer, startColor, 255, endColor, 255);
}

void AvancezLib::setBackgroundColor(int red, int green, int blue)
{
	if (red > 255 || red < 0 || green > 255 || green < 0 || blue > 255 || blue < 0) return;
	SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
}

Sprite::Sprite(SDL_Renderer * renderer, SDL_Texture * texture)
{
	Sprite::renderer = renderer;
	Sprite::texture = texture;
}

void Sprite::destroy()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
}

void Sprite::draw(int x, int y, int width, int height)
{
	SDL_Rect srcrect = { 0, 0, width, height };
	SDL_Rect dstrect = { x, y, width, height };
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
}
