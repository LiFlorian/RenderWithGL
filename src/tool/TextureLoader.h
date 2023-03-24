#pragma once

#include <map>

class TextureLoader
{
public:

	unsigned int LoadTexture(char* ImagePath, bool bFlip = false);

	unsigned int LoadTextureWithChannel(char* ImagePath,  int channelCount, bool bFlip = false);

private:

	std::map<char*, unsigned int> TexturePool;
};

