#pragma once

#include <map>
#include <vector>

class TextureLoader
{
public:

	unsigned int LoadTexture(char* ImagePath, bool bFlip = false);

	unsigned int LoadTextureWithChannel(char* ImagePath,  int channelCount, bool bFlip = false);

	unsigned int LoadCubeMap(std::vector<std::string> faceList);

	unsigned int LoadHDRTexture(char* ImagePath);

private:

	std::map<char*, unsigned int> TexturePool;
};

