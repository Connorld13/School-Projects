#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <random>
#include <vector> 
#include <chrono> /
#include "TextureManager.h"
#include "Tile.h"
using namespace std;

class Board
{
	string name;
	string playerTime;
	unsigned int width;
	unsigned int widthPixels;
	unsigned int height;
	unsigned int heightPixels;
	unsigned int numTiles;
	unsigned int numMines;
	unsigned int numRevealedTiles = 0;
	int mineCounter;
	bool debugMode = false;
	bool isPaused;
	bool pauseByLeaderboard = false;
	bool leaderboardOpenWindow;
	bool gameStarted = false;
	vector<vector<Tile*>> tiles;
	vector<pair<string, string>> leaderboardData;

	chrono::high_resolution_clock::time_point endTime;
	chrono::high_resolution_clock::time_point pausedTime;
	chrono::duration<double> totalPausedDuration;

	enum GameState { playing, win, lose, pause };
	GameState gameState = playing;
	GameState previousGameState;
public:
	chrono::high_resolution_clock::time_point startTime;
	chrono::duration<double> pausedElapsedTime;

	vector<string> GetLeaderboardContent();


	Board();
	Board(unsigned int _width, unsigned int _height, unsigned int tileCount, unsigned int mineCount);

	void SetPlayerName(const string& playerName);
	bool IsLeaderboardOpenWindow() const { return leaderboardOpenWindow; }
	void SetLeaderboardOpenWindow(bool value) { leaderboardOpenWindow = value; }
	void WhichTile(int& x, int& y, string button = "left");
	void DrawMineCounter(sf::RenderWindow& window);
	void DrawTimer(sf::RenderWindow& window);
	void DrawBoard(sf::RenderWindow& window);
	void DrawButtons(sf::RenderWindow& window);
	void GetConfig();
	void FillBoard(string fileName);
	void GetAdjacentTiles();
	void RevealTiles(Tile* tile);
	void Randomize();
	void GameWon();
	void ResetGame();
	void StartTimer();
	void removeAsterisks(string& str);
	void UpdateLeaderboard(const string& playerTime);

	unsigned int GetWidth();
	unsigned int GetHeight();
	unsigned int GetNumTiles();
	unsigned int GetNumMines();
	GameState GetGameState();
	vector<pair<string, int>> leaderboard;

};
