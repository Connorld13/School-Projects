#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctype.h>
#include "Board.h"

using namespace std;

int main()
{
    // Initialize board object, configure window size, and fill board w/ mines
    Board gameBoard = Board();
    gameBoard.Randomize();
    
    // Create welcome window
    sf::RenderWindow welcomeWindow(sf::VideoMode(gameBoard.GetWidth(), gameBoard.GetHeight()), "Minesweeper");

    // Load font
    sf::Font font;
    if (!font.loadFromFile("files/font.ttf"))
    {
        cout << "Failed to load font." << endl;
        return -1;
    }

    // Create welcome text
    sf::Text welcomeText;
    welcomeText.setFont(font);
    welcomeText.setString("WELCOME TO MINESWEEPER!");
    welcomeText.setCharacterSize(24);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    welcomeText.setPosition(gameBoard.GetWidth() / 2.0f, gameBoard.GetHeight() / 2.0f - 150);
    welcomeText.setOrigin(welcomeText.getLocalBounds().width / 2.0f, welcomeText.getLocalBounds().height / 2.0f);

    // Create input prompt text
    sf::Text inputPromptText;
    inputPromptText.setFont(font);
    inputPromptText.setString("Enter your name:");
    inputPromptText.setCharacterSize(20);
    inputPromptText.setFillColor(sf::Color::White);
    inputPromptText.setStyle(sf::Text::Bold);
    inputPromptText.setPosition(gameBoard.GetWidth() / 2.0f, gameBoard.GetHeight() / 2.0f - 75);
    inputPromptText.setOrigin(inputPromptText.getLocalBounds().width / 2.0f, inputPromptText.getLocalBounds().height / 2.0f);

    // Create input text
    sf::Text inputText;
    inputText.setFont(font);
    inputText.setCharacterSize(18);
    inputText.setFillColor(sf::Color::Yellow);
    inputText.setStyle(sf::Text::Bold);
    inputText.setPosition(gameBoard.GetWidth() / 2.0f, gameBoard.GetHeight() / 2.0f);
    inputText.setOrigin(inputText.getLocalBounds().width / 2.0f, inputText.getLocalBounds().height / 2.0f); // Update origin to keep text centered


    string name = "";

    while (welcomeWindow.isOpen())
    {
        sf::Event event;
        sf::Vector2i mousePosition = sf::Mouse::getPosition(welcomeWindow);

        while (welcomeWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                welcomeWindow.close();
                return 0;
            }
            else if (event.type == sf::Event::TextEntered)
            {
                // Only accept alphabets and limit input to 10 characters
                if (isalpha(event.text.unicode) && name.length() < 10)
                {
                    name += static_cast<char>(event.text.unicode);
                    inputText.setString(name);
                    inputText.setOrigin(inputText.getLocalBounds().width / 2.0f, inputText.getLocalBounds().height / 2.0f); // Add this line
                }
                // Backspace removes the last character
                else if (event.text.unicode == 8 && name.length() > 0)
                {
                    name.pop_back();
                    inputText.setString(name);
                    inputText.setOrigin(inputText.getLocalBounds().width / 2.0f, inputText.getLocalBounds().height / 2.0f); // Add this line
                }
                // Enter key closes welcome window and opens game window
                else if (event.text.unicode == 13 && name.length() > 0)
                {
                    // Capitalize first letter and lowercase the rest
                    name[0] = toupper(name[0]);
                    for (int i = 1; i < name.length(); i++)
                    {
                        name[i] = tolower(name[i]);
                    }
                    welcomeWindow.close();

                    cout << "Name entered: " << name << std::endl;
                    gameBoard.SetPlayerName(name);
                    // Create game window
                    sf::RenderWindow gameWindow(sf::VideoMode(gameBoard.GetWidth(), gameBoard.GetHeight()), "Minesweeper");
                    gameBoard.StartTimer();

                    // Create leaderboard window
                    sf::RenderWindow leaderboardWindow;

                    while (gameWindow.isOpen())
                    {
                        
                        sf::Event event;
                        sf::Vector2i mousePosition = sf::Mouse::getPosition(gameWindow);

                        while (gameWindow.pollEvent(event))
                        {
                            if (event.type == sf::Event::Closed)
                                gameWindow.close();
                            else if ((event.type == sf::Event::MouseButtonReleased) &&
                                (mousePosition.x >= 0 && mousePosition.x < gameBoard.GetWidth()) &&
                                (mousePosition.y >= 0 && mousePosition.y < gameBoard.GetHeight()))
                            {
                                if (event.mouseButton.button == sf::Mouse::Left)
                                    gameBoard.WhichTile(mousePosition.x, mousePosition.y, "left");
                                else if (event.mouseButton.button == sf::Mouse::Right)
                                    gameBoard.WhichTile(mousePosition.x, mousePosition.y, "right");
                            }
                        }
                 
                        // leaderboard window handling
                        if (gameBoard.IsLeaderboardOpenWindow() && !leaderboardWindow.isOpen()) {
                            leaderboardWindow.create(sf::VideoMode(gameBoard.GetWidth() / 2, gameBoard.GetHeight() / 2), "Leaderboard");
                        }

                        if (leaderboardWindow.isOpen()) {
                            sf::Event event;
                            while (leaderboardWindow.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    leaderboardWindow.close();
                                    gameBoard.SetLeaderboardOpenWindow(false);
                                }
                            }


                            // Set up leaderboard title text
                            sf::Text leaderboardTitle;
                            leaderboardTitle.setFont(font);
                            leaderboardTitle.setString("LEADERBOARD");
                            leaderboardTitle.setCharacterSize(20);
                            leaderboardTitle.setFillColor(sf::Color::White);
                            leaderboardTitle.setStyle(sf::Text::Bold | sf::Text::Underlined);
                            leaderboardTitle.setPosition(leaderboardWindow.getSize().x / 2.0f, leaderboardWindow.getSize().y / 2.0f - 120);
                            leaderboardTitle.setOrigin(leaderboardTitle.getLocalBounds().width / 2.0f, leaderboardTitle.getLocalBounds().height / 2.0f);

                            // Set up leaderboard content text
                            sf::Text leaderboardContent;
                            leaderboardContent.setFont(font);
                            leaderboardContent.setCharacterSize(18);
                            leaderboardContent.setFillColor(sf::Color::White);
                            leaderboardContent.setStyle(sf::Text::Bold);

                            // Add leaderboard data to the content string separated by two line spaces
                            string contentString;
                            vector<string> leaderboardData = gameBoard.GetLeaderboardContent();
                            for (const string& entry : leaderboardData) {
                                contentString += entry + "\n\n";
                            }
                            leaderboardContent.setString(contentString);
                            leaderboardContent.setPosition(leaderboardWindow.getSize().x / 2.0f, leaderboardWindow.getSize().y / 2.0f - 0 * 2);
                            leaderboardContent.setOrigin(leaderboardContent.getLocalBounds().width / 2.0f, leaderboardContent.getLocalBounds().height / 2.0f);

                            // Draw leaderboard window content
                            leaderboardWindow.clear(sf::Color::Blue);
                            leaderboardWindow.draw(leaderboardTitle);
                            leaderboardWindow.draw(leaderboardContent);
                            leaderboardWindow.display();
                        }
                        else {
                            if (leaderboardWindow.isOpen()) {
                                leaderboardWindow.close();
                            }
                        }

                        // Clear anything already drawn
                        gameWindow.clear(sf::Color::White);

                        // Draw whatever
                        gameBoard.DrawBoard(gameWindow);
                        gameBoard.DrawButtons(gameWindow);

                        // Display whatever was drawn
                        gameWindow.display();
                    }
                }
            }
        }

        // Clear anything already drawn
        welcomeWindow.clear(sf::Color::Blue);

        // Draw welcome text, input prompt text, and input text
        welcomeWindow.draw(welcomeText);
        welcomeWindow.draw(inputPromptText);
        welcomeWindow.draw(inputText);

        // Draw cursor indicator
        sf::RectangleShape cursor(sf::Vector2f(2, 18));
        cursor.setFillColor(sf::Color::Yellow);
        cursor.setPosition(inputText.getPosition().x + inputText.getLocalBounds().width / 2.0f - 1, inputText.getPosition().y - inputText.getLocalBounds().height / 2.0f + 1); // Update cursor position


        welcomeWindow.draw(cursor);

        // Display whatever was drawn
        welcomeWindow.display();
    }

    cout << "Program ended." << endl;

    // Clear out any textures before program ends
    TextureManager::Clear();

    return 0;
}
