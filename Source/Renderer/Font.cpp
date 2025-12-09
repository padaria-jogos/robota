#include "Font.h"
#include "Texture.h"
#include <vector>
#include "../Game.h"
#include "../Math.h"

namespace
{
int RoundToInt(float value)
{
	return (value >= 0.0f) ? static_cast<int>(value + 0.5f) : static_cast<int>(value - 0.5f);
}

void CopyBytes(void* dst, const void* src, int byteCount)
{
	if (!dst || !src || byteCount <= 0)
	{
		return;
	}
	Uint8* destBytes = static_cast<Uint8*>(dst);
	const Uint8* srcBytes = static_cast<const Uint8*>(src);
	for (int i = 0; i < byteCount; ++i)
	{
		destBytes[i] = srcBytes[i];
	}
}

void MoveBytes(void* dst, const void* src, int byteCount)
{
	if (!dst || !src || byteCount <= 0 || dst == src)
	{
		return;
	}
	Uint8* destBytes = static_cast<Uint8*>(dst);
	const Uint8* srcBytes = static_cast<const Uint8*>(src);
	if (destBytes < srcBytes)
	{
		for (int i = 0; i < byteCount; ++i)
		{
			destBytes[i] = srcBytes[i];
		}
	}
	else
	{
		for (int i = byteCount - 1; i >= 0; --i)
		{
			destBytes[i] = srcBytes[i];
		}
	}
}

void SetBytes(void* dst, Uint8 value, int byteCount)
{
	if (!dst || byteCount <= 0)
	{
		return;
	}
	Uint8* destBytes = static_cast<Uint8*>(dst);
	for (int i = 0; i < byteCount; ++i)
	{
		destBytes[i] = value;
	}
}

struct RowSpan
{
	int left;
	int right;
};

Uint8 ExtractAlpha(const SDL_PixelFormat* format, Uint32 pixel)
{
	if (!format || format->Amask == 0)
	{
		return 255;
	}

	Uint32 alpha = (pixel & format->Amask) >> format->Ashift;
	if (format->Aloss != 0)
	{
		alpha <<= format->Aloss;
	}
	return static_cast<Uint8>(alpha);
}

bool RowBounds(SDL_Surface* surface, int y, int& left, int& right)
{
	if (!surface)
	{
		return false;
	}

	const int width = surface->w;
	const int bytesPerPixel = surface->format->BytesPerPixel;
	Uint8* row = static_cast<Uint8*>(surface->pixels) + y * surface->pitch;

	left = width;
	right = -1;

	for (int x = 0; x < width; ++x)
	{
		Uint32 pixel = 0;
		CopyBytes(&pixel, row + x * bytesPerPixel, bytesPerPixel);
		Uint8 alpha = ExtractAlpha(surface->format, pixel);
		if (alpha != 0)
		{
			if (x < left)
			{
				left = x;
			}
			if (x > right)
			{
				right = x;
			}
		}
	}

	if (right >= left)
	{
		return true;
	}

	left = 0;
	right = -1;
	return false;
}

bool RowIsEmpty(SDL_Surface* surface, int y)
{
	int left = 0;
	int right = -1;
	return !RowBounds(surface, y, left, right);
}

void ShiftRow(SDL_Surface* surface, int y, int left, int right, int shift, int bytesPerPixel)
{
	if (!surface || shift == 0)
	{
		return;
	}

	Uint8* row = static_cast<Uint8*>(surface->pixels) + y * surface->pitch;
	const int rowWidth = right - left + 1;
	Uint8* src = row + left * bytesPerPixel;
	Uint8* dst = row + (left + shift) * bytesPerPixel;
	MoveBytes(dst, src, rowWidth * bytesPerPixel);

	if (shift > 0)
	{
		SetBytes(row + left * bytesPerPixel, 0, shift * bytesPerPixel);
	}
	else
	{
		SetBytes(row + (right + 1 + shift) * bytesPerPixel, 0, (-shift) * bytesPerPixel);
	}
}

void CenterWrappedLines(SDL_Surface* surface)
{
	if (!surface || surface->w == 0 || surface->h == 0)
	{
		return;
	}

	const int height = surface->h;
	const int bytesPerPixel = surface->format->BytesPerPixel;

	int y = 0;
	while (y < height)
	{
		while (y < height && RowIsEmpty(surface, y))
		{
			++y;
		}

		if (y >= height)
		{
			break;
		}

		const int blockStart = y;
		std::vector<RowSpan> rowBounds;
		int blockLeft = surface->w;
		int blockRight = -1;

		while (y < height)
		{
			int rowLeft = 0;
			int rowRight = -1;
			if (!RowBounds(surface, y, rowLeft, rowRight))
			{
				break;
			}

			rowBounds.push_back(RowSpan{rowLeft, rowRight});
			blockLeft = Math::Min(blockLeft, rowLeft);
			blockRight = Math::Max(blockRight, rowRight);
			++y;
		}

		if (blockRight < blockLeft)
		{
			continue;
		}

		const float targetCenter = (static_cast<float>(surface->w) - 1.0f) * 0.5f;
		const float blockCenter = (static_cast<float>(blockLeft) + static_cast<float>(blockRight)) * 0.5f;
		int desiredShift = RoundToInt(targetCenter - blockCenter);
		desiredShift = Math::Clamp(desiredShift, -blockLeft, surface->w - blockRight - 1);

		if (desiredShift == 0)
		{
			continue;
		}

		for (size_t rowIdx = 0; rowIdx < rowBounds.size(); ++rowIdx)
		{
			const int rowY = blockStart + static_cast<int>(rowIdx);
			ShiftRow(surface, rowY, rowBounds[rowIdx].left, rowBounds[rowIdx].right, desiredShift, bytesPerPixel);
		}
	}
}
}

Font::Font()
{
}

Font::~Font()
{
}

bool Font::Load(const std::string& fileName)
{
	// We support these font sizes
	std::vector<int> fontSizes = {8,  9,  10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
								  34, 36, 38, 40, 42, 44, 46, 48, 52, 56, 60, 64, 68, 72};

	for (auto& size : fontSizes)
	{
		TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
		if (font == nullptr)
		{
			SDL_Log("Failed to load font %s in size %d", fileName.c_str(), size);
			return false;
		}
		mFontData.emplace(size, font);
	}
	return true;
}

void Font::Unload()
{
	for (auto& font : mFontData)
	{
		TTF_CloseFont(font.second);
	}
}

Texture* Font::RenderText(const std::string& text, const Vector3& color /*= Color::White*/,
					  int pointSize /*= 24*/, unsigned wrapLength /*= 900*/, bool centerWrappedLines /*= false*/)
{
	Texture* texture = nullptr;

	// Convert to SDL_Color
	SDL_Color sdlColor;
	// Swap red and blue so we get RGBA instead of BGRA
	sdlColor.b = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.r = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	// Find the font data for this point size
	auto iter = mFontData.find(pointSize);
	if (iter != mFontData.end())
	{
		TTF_Font* font = iter->second;
		// Draw this to a surface (blended for alpha)
		SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), sdlColor, wrapLength);
		if (surf != nullptr)
		{
			if (centerWrappedLines)
			{
				CenterWrappedLines(surf);
			}

			// Convert from surface to texture
			texture = new Texture();
			texture->CreateFromSurface(surf);
			SDL_FreeSurface(surf);
		}
	}
	else
	{
		SDL_Log("Point size %d is unsupported", pointSize);
	}

	return texture;
}
