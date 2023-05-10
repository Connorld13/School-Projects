#include <chrono>
#include <utility>
#include "Board.h"
#include <sstream>
#include <iomanip>
using namespace std;

Board::Board() {
	isPaused = false;
	leaderboardOpenWindow = false;
	pauseByLeaderboard = false;
	totalPausedDuration = chrono::duration<double>(0);
	endTime = chrono::high_resolution_clock::now();
}
Board::Board(unsigned int _width, unsigned int _height, unsigned int tileCount, unsigned int mineCount)
{
	width = _width;
	height = _height;
	numTiles = tileCount;
	numMines = mineCount;
	isPaused = false;
	leaderboardOpenWindow = false;
	pauseByLeaderboard = false;
}



unsigned int Board::GetWidth()
{
	return this->widthPixels;
}

unsigned int Board::GetHeight()
{
	return this->heightPixels;
}

unsigned int Board::GetNumTiles()
{
	return this->numTiles;
}

unsigned int Board::GetNumMines()
{
	return this->numMines;
}

Board::GameState Board::GetGameState()
{
	return gameState;
}

void Board::GetConfig()
{
	ifstream inFile("files/board_config.cfg");

	if (inFile.is_open())
	{
		inFile >> width;
		inFile >> height;
		inFile >> numMines;

		numTiles = width * height;
		widthPixels = width * 32;
		heightPixels = height * 32 + 100;
		mineCounter = numMines;
	}
	else
		cout << "File \"config.cfg\" could not be opened." << endl;
}

void Board::ResetGame()
{
	// Reset game state, mine counter, and revealed tiles
	gameState = playing;
	mineCounter = numMines;
	numRevealedTiles = 0;

	// Clear the tiles vector
	tiles.clear();

	// Randomize the board
	Randomize();

	// Reset the timer
	StartTimer();

	// Reset isPaused and totalPausedDuration variables
	isPaused = false;
	totalPausedDuration = chrono::duration<double>(0);

	endTime = chrono::high_resolution_clock::now();
}


void Board::SetPlayerName(const string& playerName) {
	name = playerName;
}


vector<string> Board::GetLeaderboardContent()
{
	// Read leaderboard data from the file
	leaderboardData.clear();
	ifstream inFile("files/leaderboard.txt");

	if (inFile.is_open())
	{
		string line;
		while (getline(inFile, line))
		{
			istringstream lineStream(line);
			string score, player_name;
			getline(lineStream, score, ',');
			getline(lineStream, player_name);

			leaderboardData.push_back(make_pair(score, player_name));
		}
	}
	else
	{
		cout << "File \"leaderboard.txt\" could not be opened." << endl;
	}

	// Sort the leaderboard data by score
	sort(leaderboardData.begin(), leaderboardData.end(), [](const pair<string, string>& a, const pair<string, string>& b) {
		istringstream aStream(a.first);
	istringstream bStream(b.first);
	int aMinutes, aSeconds, bMinutes, bSeconds;
	char colon;
	aStream >> aMinutes >> colon >> aSeconds;
	bStream >> bMinutes >> colon >> bSeconds;
	return aMinutes * 60 + aSeconds < bMinutes * 60 + bSeconds;
		});

	// Convert the leaderboard data to a vector of strings with formatting
	vector<string> result;
	int rank = 1;
	for (const auto& entry : leaderboardData)
	{
		ostringstream formattedEntry;
		string playerName = entry.second;
		formattedEntry << setw(2) << rank << ".    " << entry.first << "    " << playerName;
		result.push_back(formattedEntry.str());
		rank++;
	}

	return result;
}

void Board::removeAsterisks(string& str) {
	str.erase(remove(str.begin(), str.end(), '*'), str.end());
}

void Board::UpdateLeaderboard(const string& playerTime)
{
	// Read and sort the leaderboard data
	GetLeaderboardContent();

	// Remove any asterisks from the existing names
	for (auto& entry : leaderboardData)
	{
		removeAsterisks(entry.second);
	}

	// Convert playerTime to seconds
	istringstream playerTimeStream(playerTime);
	int playerMinutes, playerSeconds;
	char colon;
	playerTimeStream >> playerMinutes >> colon >> playerSeconds;
	int playerTotalSeconds = playerMinutes * 60 + playerSeconds;

	// Check if the player's time is better than any entry in the leaderboard
	bool playerInserted = false;
	for (auto it = leaderboardData.begin(); it != leaderboardData.end(); ++it)
	{
		istringstream currentEntryTimeStream(it->first);
		int currentEntryMinutes, currentEntrySeconds;
		currentEntryTimeStream >> currentEntryMinutes >> colon >> currentEntrySeconds;
		int currentEntryTotalSeconds = currentEntryMinutes * 60 + currentEntrySeconds;

		if (playerTotalSeconds < currentEntryTotalSeconds)
		{
			string playerNameWithAsterisk = name + "*";
			leaderboardData.insert(it, make_pair(playerTime, playerNameWithAsterisk));
			playerInserted = true;
			break;
		}
	}

	// If the player's time is better than the worst score or leaderboard is not full
	if (!playerInserted && leaderboardData.size() < 5)
	{
		leaderboardData.push_back(make_pair(playerTime, name + "*"));
	}

	// Keep only the top 5 scores
	if (leaderboardData.size() > 5)
	{
		leaderboardData.pop_back();
	}

	// Update the leaderboard.txt file
	ofstream outFile("files/leaderboard.txt");

	if (outFile.is_open())
	{
		for (const auto& entry : leaderboardData)
		{
			outFile << entry.first << "," << entry.second << endl;
		}
	}
	else
	{
		cout << "File \"leaderboard.txt\" could not be opened." << endl;
	}
}



void Board::FillBoard(string fileName)
{
	tiles.clear();

	numMines = 0;
	numRevealedTiles = 0;
	debugMode = false;
	gameState = playing;

	ifstream inFile(fileName);

	if (inFile.is_open())
	{
		string thisRow;
		char token;

		while (getline(inFile, thisRow))
		{
			istringstream thisRowStream(thisRow);
			vector<Tile*> rowTiles;

			while (thisRowStream >> token)
			{
				if (token == '1')
				{
					Tile* newTile = new Tile(true);
					rowTiles.push_back(newTile);
					numMines++;
				}
				else
				{
					Tile* newTile = new Tile(false);
					rowTiles.push_back(newTile);
				}
			}

			mineCounter = numMines;

			if (rowTiles.size() < width)
			{
				unsigned int difference = width - rowTiles.size();

				for (unsigned int x = 0; x < difference; x++)
				{
					Tile* newTile = new Tile(false);
					rowTiles.push_back(newTile);
				}
			}

			tiles.push_back(rowTiles);
		}

		if (tiles.size() < height)
		{
			unsigned int difference = height - tiles.size();

			for (unsigned int y = 0; y < difference; y++)
			{
				vector<Tile*> rowTiles;

				for (unsigned int x = 0; x < width; x++)
				{
					Tile* newTile = new Tile(false);
					rowTiles.push_back(newTile);
				}

				tiles.push_back(rowTiles);
			}
		}

		GetAdjacentTiles();
	}
	else
	{
		cout << "File " << fileName << " could not be opened correctly." << endl;
	}
}

void Board::GetAdjacentTiles()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			for (int yRel = y - 1; yRel < y + 2; yRel++)
			{
				if (yRel < 0 || yRel >= height)
					continue;

				for (int xRel = x - 1; xRel < x + 2; xRel++)
				{
					if (xRel < 0 || xRel >= width || (xRel == x && yRel == y))
						continue;

					if (tiles[yRel][xRel]->Mine())
						tiles[y][x]->AddAdjacentMine();

					tiles[y][x]->AddAdjacentTile(tiles[yRel][xRel]);
				}
			}
		}
	}
}

void Board::RevealTiles(Tile* tile)
{
	tile->Unhide();
	tile->ClickTile();
	numRevealedTiles++;

	if (!tile->Mine() && tile->NumAdjacentMines() == 0)
	{
		for (unsigned int i = 0; i < tile->Adjacents().size(); i++)
		{
			auto nextTile = tile->Adjacents().at(i);

			if (!nextTile->Mine() && !nextTile->Flagged() && !nextTile->IsClicked())
				RevealTiles(tile->Adjacents().at(i));
		}
	}
}

void Board::DrawTimer(sf::RenderWindow& window) {
	chrono::high_resolution_clock::time_point effectiveTime;
	if (gameState != win && gameState != lose) {
		if (isPaused) {
			effectiveTime = pausedTime;
		}
		else {
			effectiveTime = chrono::high_resolution_clock::now();
		}
	}
	else {
		// When game is won or lost, stop the clock at the last time before the game ended
		effectiveTime = endTime;
	}

	chrono::duration<double> elapsedTime = effectiveTime - startTime - totalPausedDuration;
	int elapsedTimeInt = static_cast<int>(elapsedTime.count());
	int minutes = elapsedTimeInt / 60;
	int seconds = elapsedTimeInt % 60;



	sf::Sprite digitMinutes1;
	sf::Sprite digitMinutes2;
	sf::Sprite digitSeconds1;
	sf::Sprite digitSeconds2;

	digitMinutes1.setTexture(TextureManager::GetTexture("digits", to_string(minutes / 10), (minutes / 10) * 21, 0, 21, 32));
	digitMinutes1.setPosition((width * 32) - 97, 32 * (height + 0.5f) + 16);
	window.draw(digitMinutes1);

	digitMinutes2.setTexture(TextureManager::GetTexture("digits", to_string(minutes % 10), (minutes % 10) * 21, 0, 21, 32));
	digitMinutes2.setPosition((width * 32) - 76, 32 * (height + 0.5f) + 16);
	window.draw(digitMinutes2);

	digitSeconds1.setTexture(TextureManager::GetTexture("digits", to_string(seconds / 10), (seconds / 10) * 21, 0, 21, 32));
	digitSeconds1.setPosition((width * 32) - 54, 32 * (height + 0.5f) + 16);
	window.draw(digitSeconds1);

	digitSeconds2.setTexture(TextureManager::GetTexture("digits", to_string(seconds % 10), (seconds % 10) * 21, 0, 21, 32));
	digitSeconds2.setPosition((width * 32) - 33, 32 * (height + 0.5f) + 16);
	window.draw(digitSeconds2);
}

void Board::StartTimer()
{
	startTime = chrono::high_resolution_clock::now();
}

void Board::Randomize()
{
	GetConfig();

	if (tiles.size() > 0)
		tiles.clear();

	numRevealedTiles = 0;
	debugMode = false;
	gameState = playing;
	unsigned int minesPlaced = 0;

	// create board with no mines
	for (unsigned int y = 0; y < height; y++)
	{
		vector<Tile*> rowTiles;

		for (unsigned int x = 0; x < width; x++)
		{
			Tile* newTile = new Tile(false);
			rowTiles.push_back(newTile);
		}

		tiles.push_back(rowTiles);
	}

	random_device rd; // obtain a random number from hardware
	mt19937 gen(rd()); // seed the generator
	uniform_int_distribution<> distr(0, height - 1); // range for height
	uniform_int_distribution<> distr2(0, width - 1); // range for width

	// fill in mines randomly until total number of mines is reached
	while (minesPlaced < numMines)
	{
		unsigned int randY = distr(gen); // choose random y
		unsigned int randX = distr2(gen); // choose random x

		if (!tiles[randY][randX]->Mine())
		{
			tiles[randY][randX]->ChangeMine();
			minesPlaced++;
		}
	}

	GetAdjacentTiles();
}

void Board::WhichTile(int& x, int& y, string button)
{
	
	if (!isPaused && y < heightPixels - 100 && gameState == playing)
	{
		x /= 32; y /= 32;

		if (tiles[y][x]->Hidden())
		{
			if (button == "left" && !tiles[y][x]->Flagged())
			{
				RevealTiles(tiles[y][x]);

				if (tiles[y][x]->Mine())
				{
					gameState = lose;
					debugMode = true;
					leaderboardOpenWindow = true;
					// Record the time the game ended
					endTime = chrono::high_resolution_clock::now();
					cout << "You lose!" << endl;
				}
				else if (numRevealedTiles == numTiles - numMines)
				{
					gameState = win;
					GameWon();
					leaderboardOpenWindow = true;
					UpdateLeaderboard(playerTime);
					// Record the time the game ended
					endTime = chrono::high_resolution_clock::now();
					cout << "You win!" << endl;
				}

			}
			else if (button == "right")
			{
				tiles[y][x]->ChangeFlagged();

				if (tiles[y][x]->Flagged())
					mineCounter--;
				else if (!tiles[y][x]->Flagged() && mineCounter < numMines)
					mineCounter++;

				cout << "Number of mines on the board: " << mineCounter << endl;
			}

			cout << "Number of revealed tiles: " << numRevealedTiles << endl;
		}
	}
	else if (y >= heightPixels - 100 && y <= heightPixels - 36)
	{
		// Smiley face button
		if (x >= widthPixels / 2 - 32 && x <= widthPixels / 2 + 32)
		{
			cout << "Smiley face button pressed." << endl;
			cout << "New game started." << endl;
			ResetGame();
		}
		// Debug button
		else if (x >= (widthPixels - 304) && x < (widthPixels - 240))
		{
			cout << "Debug button pressed." << endl;

			if (gameState == playing)
			{
				if (debugMode == false)
					debugMode = true;
				else
					debugMode = false;
			}
		}
		// Pause/Play button
		else if (x >= (widthPixels - 240) && x < (widthPixels - 176))
		{
			cout << "Pause/Play button pressed." << endl;
			if (gameState != win && gameState != lose)
			{
				if (!isPaused && !pauseByLeaderboard) {
					isPaused = true;
					previousGameState = gameState;
					gameState = pause;
					pausedTime = chrono::high_resolution_clock::now();
				}
				else if (isPaused && !pauseByLeaderboard) {
					isPaused = false;
					gameState = previousGameState;
					totalPausedDuration += chrono::high_resolution_clock::now() - pausedTime;
				}
			}
		}
		// Leaderboard button
		else if (x >= (widthPixels - 176) && x < (widthPixels - 112))
		{
			cout << "Leaderboard button pressed." << endl;

			if (!leaderboardOpenWindow) {
				leaderboardOpenWindow = true;
				// Pause the game only if it's not already paused
				if (!isPaused) {
					isPaused = true;
					previousGameState = gameState;
					gameState = pause;
					pausedTime = chrono::high_resolution_clock::now();
					// Track that pause was triggered by leaderboard
					pauseByLeaderboard = true;
				}
			}
			else {
				leaderboardOpenWindow = false;
				// Unpause the game only if it was paused by leaderboard
				if (isPaused && pauseByLeaderboard) {
					isPaused = false;
					gameState = previousGameState;
					totalPausedDuration += chrono::high_resolution_clock::now() - pausedTime;
					pauseByLeaderboard = false;
				}
			}
		}
	}
}


void Board::DrawMineCounter(sf::RenderWindow& window)
{
	sf::Sprite digit1;
	sf::Sprite digit2;
	sf::Sprite digit3;
	sf::Sprite digit4;

	int yPosition = 32 * (height + 0.5f) + 16;
	int mineCounterAbs = abs(mineCounter);

	if (mineCounter < 0)
	{
		digit1.setTexture(TextureManager::GetTexture("digits", "-", 210, 0, 21, 32));
		digit1.setPosition(12, yPosition);
		window.draw(digit1);
	}

	digit2.setTexture(TextureManager::GetTexture("digits", to_string(mineCounterAbs / 100), (mineCounterAbs / 100) * 21, 0, 21, 32));
	digit2.setPosition(33, yPosition);
	window.draw(digit2);

	digit3.setTexture(TextureManager::GetTexture("digits", to_string((mineCounterAbs % 100) / 10), ((mineCounterAbs % 100) / 10) * 21, 0, 21, 32));
	digit3.setPosition(54, yPosition);
	window.draw(digit3);

	digit4.setTexture(TextureManager::GetTexture("digits", to_string(mineCounterAbs % 10), (mineCounterAbs % 10) * 21, 0, 21, 32));
	digit4.setPosition(75, yPosition);
	window.draw(digit4);
}


void Board::DrawBoard(sf::RenderWindow& window)
{
	// draw board tiles
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			sf::Sprite square;
			sf::Sprite number;
			sf::Sprite flag;
			sf::Sprite mine;
			square.setPosition(x * 32.f, y * 32.f);
			number.setPosition(x * 32.f, y * 32.f);
			mine.setPosition(x * 32.f, y * 32.f);
			flag.setPosition(x * 32.f, y * 32.f);
			if (gameState == pause) {
				square.setTexture(TextureManager::GetTexture("tile_revealed"));
			}
			else if (tiles[y][x]->Hidden())
			{
				square.setTexture(TextureManager::GetTexture("tile_hidden"));

				if (tiles[y][x]->Flagged())
				{
					flag.setTexture(TextureManager::GetTexture("flag"));
				}

				if (debugMode && tiles[y][x]->Mine())
					mine.setTexture(TextureManager::GetTexture("mine"));
			}
			else
			{
				square.setTexture(TextureManager::GetTexture("tile_revealed"));

				if (tiles[y][x]->Mine())
					mine.setTexture(TextureManager::GetTexture("mine"));
				else
				{
					if (tiles[y][x]->NumAdjacentMines() > 8)
					{
						cout << "x: " << x << endl;
						cout << "y: " << y << endl;
					}
					else if (tiles[y][x]->NumAdjacentMines() > 0)
					{
						string arg = "number_";
						int arg2 = tiles[y][x]->NumAdjacentMines();
						arg += to_string(arg2);

						number.setTexture(TextureManager::GetTexture(arg));
					}
				}
			}

			window.draw(square);
			window.draw(number);

			if (debugMode)
			{
				window.draw(flag);
				window.draw(mine);
			}
			else
			{
				window.draw(mine);
				window.draw(flag);
			}
		}
	}
	DrawMineCounter(window);
	DrawTimer(window);
}

void Board::DrawButtons(sf::RenderWindow& window)
{

	sf::Sprite happyFace;
	sf::Sprite debug;
	sf::Sprite pausePlay;
	sf::Sprite leaderboard;

	if (gameState == GameState::win) {
		happyFace.setTexture(TextureManager::GetTexture("face_win"));
	}
	else if (gameState == GameState::lose) {
		happyFace.setTexture(TextureManager::GetTexture("face_lose"));
	}
	else {
		happyFace.setTexture(TextureManager::GetTexture("face_happy"));
	}

	debug.setTexture(TextureManager::GetTexture("debug"));
	leaderboard.setTexture(TextureManager::GetTexture("leaderboard"));

	if (gameState == pause) {
		pausePlay.setTexture(TextureManager::GetTexture("play"));
	}
	else {
		pausePlay.setTexture(TextureManager::GetTexture("pause"));
	}

	int yPosition = height * 32 + 16;

	happyFace.setPosition(widthPixels / 2 - 16, yPosition);
	debug.setPosition(widthPixels - 304, yPosition);
	pausePlay.setPosition(widthPixels - 240, yPosition);
	leaderboard.setPosition(widthPixels - 176, yPosition);

	window.draw(happyFace);
	window.draw(debug);

	
	window.draw(pausePlay);
		
	
	window.draw(leaderboard);
}


void Board::GameWon()
{
	
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			if (tiles[y][x]->Mine() && !tiles[y][x]->Flagged())
			{
				tiles[y][x]->ChangeFlagged();
				mineCounter--;
			}
		}
	}
	// Compute the player's time
	auto currentTime = chrono::high_resolution_clock::now();
	auto elapsedSeconds = chrono::duration_cast<chrono::seconds>(currentTime - startTime - totalPausedDuration).count();
	int minutes = elapsedSeconds / 60;
	int seconds = elapsedSeconds % 60;

	// Format the player's time as a string
	stringstream playerTimeStream;
	playerTimeStream << setw(2) << setfill('0') << minutes << ":" << setw(2) << setfill('0') << seconds;
	playerTime = playerTimeStream.str();
}