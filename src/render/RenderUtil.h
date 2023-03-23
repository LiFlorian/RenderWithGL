#pragma once

#include <string>

unsigned int LoadTexture(const char* ImagePath, bool bFlip = false);

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
