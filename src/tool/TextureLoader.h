#pragma once

#include <map>

class TextureLoader
{
public:

	unsigned int LoadTexture(char* ImagePath, bool bFlip = false);

private:

	std::map<char*, unsigned int> TexturePool;
};

