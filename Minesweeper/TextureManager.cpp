#include "TextureManager.h"
using namespace std;



unordered_map<string, sf::Texture> TextureManager::textures;

void TextureManager::LoadTexture(string fileName)
{
	string path = "files/images/";
	path += fileName + ".png";
	textures[fileName].loadFromFile(path);
}

void TextureManager::LoadTexture(string fileName, string append, int xPos, int yPos, int xSize, int ySize)
{
	string path = "files/images/";
	path += fileName + ".png";
	textures[fileName + append].loadFromFile(path, sf::IntRect(xPos, yPos, xSize, ySize));
}

sf::Texture& TextureManager::GetTexture(string textureName)
{

	if (textures.find(textureName) == textures.end())
		LoadTexture(textureName);
	return textures[textureName];
}

sf::Texture& TextureManager::GetTexture(string textureName, string append, int xPos, int yPos, int xSize, int ySize)
{
	if (textures.find(textureName + append) == textures.end())
		LoadTexture(textureName, append, xPos, yPos, xSize, ySize);
	return textures[textureName + append];
}
void TextureManager::Clear()
{
	textures.clear(); 
}