#pragma once
#include <string>
#include <unordered_map>
#include <SDL_ttf.h>
#include "../Math.h"

class Font
{
public:
	Font();
	~Font();

	// Load/unload from a file
	bool Load(const std::string& fileName);
	void Unload();

	// Given string and this font, draw to a texture
	class Texture* RenderText(const std::string& text, const Vector3& color = Color::White,
							  int pointSize = 30, unsigned wrapLength = 900);

private:
	// Map of point sizes to font data
	std::unordered_map<int, TTF_Font*> mFontData;
};
