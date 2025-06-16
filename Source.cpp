#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <string>
#include <algorithm>
#include <fstream>
#include <utility>
using namespace std;

std::default_random_engine rng(static_cast<unsigned>(time(nullptr)));
enum GameState { MENU, GAME, GAME_OVER };
// funtion to set starting position
void setStartingPosition(sf::Sprite& sprite, float x, float y) {
    sprite.setPosition(x, y);
}

//setting scale
void setingScale(sf::Sprite& sprite, const sf::Texture& texture, const sf::RenderWindow& window) {
    sf::Vector2u textureSize = texture.getSize();
    sf::Vector2u windowSize = window.getSize();

    sprite.setScale(
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    );
}

//movement of background 
void checkbackground(sf::Sprite& backgroundSprite, sf::Sprite& backgroundSprite2, const sf::RenderWindow& window, const sf::Texture&backgroundTexture,float scrollSpeed) {
    //moving background and eneny car
    backgroundSprite.move(0.f, scrollSpeed);
    backgroundSprite2.move(0.f, scrollSpeed);

    if (backgroundSprite.getPosition().y >= window.getSize().y) {
        setStartingPosition(backgroundSprite, 0, backgroundSprite2.getPosition().y - (backgroundTexture.getSize().y * backgroundSprite.getScale().y));
    }
    if (backgroundSprite2.getPosition().y >= window.getSize().y) {
        setStartingPosition(backgroundSprite2, 0, backgroundSprite.getPosition().y - (backgroundTexture.getSize().y * backgroundSprite.getScale().y));
    }
}

//to move spirit
void spiritmovement(sf::Sprite &playerspirit,sf::Event &event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::A:
            if (playerspirit.getPosition().x > 130) {
                playerspirit.move(-100.f, 0.f);  // Move left
            }
            break;
        case sf::Keyboard::D:
            if (playerspirit.getPosition().x != 630) {
                playerspirit.move(100.f, 0.f);   // Move right
            }
            break;
        case sf::Keyboard::W:
            if (playerspirit.getPosition().y != 20) {
                playerspirit.move(0.f, -10.f);   // Move up
            }
            break;
        case sf::Keyboard::S:
            if (playerspirit.getPosition().y != 500) {
                playerspirit.move(0.f, 10.f);    // Move down
            }
            break;
        default:
            break;
        }
    }
}

//randome lane function
float getRandomLane(const std::vector<float>& lanes, const std::vector<sf::Sprite>& enemies, float spriteWidth) {
    std::vector<float> available = lanes;
   
    // Removing unavailabe lanes
    for (const auto& car : enemies) {
        float x = car.getPosition().x;
        available.erase(std::remove_if(available.begin(), available.end(), [&](float laneX) {
            return std::abs(x - laneX) < 40.f; 
            }
        ), available.end());
    }

    // Return a random available lane or default to a lane
    if (!available.empty()) {
        std::uniform_int_distribution<int> dist(0, available.size() - 1);
        return available[dist(rng)];
    }

    return lanes[0]; // fallback
}

//collision
bool checkCollision(const sf::Sprite& a, const sf::Sprite& b) {
    return a.getGlobalBounds().intersects(b.getGlobalBounds());
}

//leaderboard
//writing
void saveScoreToFile(const string& playerName, int score, const string& filename = "scores.txt") {
    ofstream outFile(filename, ios::app);
    if (outFile.is_open()) {
        outFile << playerName << " " << score << "\n";
        outFile.close();
    }
}
vector<std::pair<std::string, int>> loadTopScores(const string& filename = "scores.txt", int topN = 5) {
    ifstream inFile(filename);
    vector<std::pair<string, int>> scores;
    string name;
    int score;

    while (inFile >> name >> score) {
        scores.emplace_back(name, score);
    }

    std::sort(scores.begin(), scores.end(), [](auto& a, auto& b) {
        return a.second > b.second; // descending order
        });

    if (scores.size() > topN) scores.resize(topN);
    return scores;
}
void showLeaderboard(sf::RenderWindow& window, sf::Font& font) {
    auto topScores = loadTopScores();

    sf::Text title("Leaderboard", font, 40);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(250, 50);

    std::vector<sf::Text> scoreTexts;
    for (size_t i = 0; i < topScores.size(); ++i) {
        sf::Text line("", font, 28);
        line.setFillColor(sf::Color::White);
        line.setString(to_string(i + 1) + ". " + topScores[i].first + " - " + to_string(topScores[i].second));
        line.setPosition(220, 120 + i * 40);
        scoreTexts.push_back(line);
    }

    sf::Text backText("Press Enter to return", font, 24);
    backText.setFillColor(sf::Color::Green);
    backText.setPosition(250, 400);

    window.clear();
    window.draw(title);
    for (const auto& text : scoreTexts) window.draw(text);
    window.draw(backText);
    window.display();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
                return;
        }
    }
}
//menu
void menu(sf::RenderWindow& window, sf::Text& title, std::vector<sf::Text>& options, int selectedOption) {
    window.clear();
    window.draw(title);
    for (size_t i = 0; i < options.size(); ++i) {
        options[i].setFillColor(i == selectedOption ? sf::Color::Red : sf::Color::White);
        window.draw(options[i]);
    }
    window.display();
}

//menue
int game(sf::RenderWindow& window, sf::Font& font) {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("background_image.png")) {
        return -1;
    }

    //loading player car
    sf::Texture Player_car_texture;
    if (!Player_car_texture.loadFromFile("user car_image.png")) {
        return -1;
    }

    //creating player spirit
    sf::Sprite playerspirit;
    playerspirit.setTexture(Player_car_texture);
    playerspirit.setScale(0.31f, 0.3f);
    setStartingPosition(playerspirit, 330.f, 500.f);


    // Create a sprite for the background
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    setingScale(backgroundSprite, backgroundTexture, window);

    //second background spitit ......used for continious going down
    sf::Sprite backgroundSprite2(backgroundTexture);
    setingScale(backgroundSprite2, backgroundTexture, window);

    // Postion each above other
    backgroundSprite.setPosition(0, 0);
    setStartingPosition(backgroundSprite2, 0, backgroundTexture.getSize().y * backgroundSprite.getScale().y);

    //load enemy car
    sf::Texture enemyCarTexture;
    if (!enemyCarTexture.loadFromFile("lane_car_image.png")) {
        return -1;
    }

    // Create 4 enemy car sprites
    std::vector<sf::Sprite> enemyCars(4);
    std::vector<float> laneX = { 133.f, 225.f,330.f, 430.f,530.f, 625.f }; // Lane positions
    for (int i = 0; i < 4; ++i) {
        enemyCars[i].setTexture(enemyCarTexture);
        enemyCars[i].setScale(0.31f, 0.3f);
        setStartingPosition(enemyCars[i], laneX[i], -100.f * i);
    }

    //for scrolling 
    float speed = 0.3f;
    int score = 0;
    sf::Clock clock, scoreClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // to sense key
            if (event.type == sf::Event::KeyPressed) {
                spiritmovement(playerspirit, event);
            }

            // Close event
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        //Increase speed
        if (clock.getElapsedTime().asSeconds() > 5.f) {
            if (speed < 5.f) {
                speed += 0.05f;
                clock.restart();
            }
        }
        checkbackground(backgroundSprite, backgroundSprite2, window, backgroundTexture, speed);
        //car speed 
        for (int i = 0;i < 4;i++) {
            enemyCars[i].move(0.f, speed * 1.5);
            if (enemyCars[i].getPosition().y > window.getSize().y) {
                float spriteWidth = enemyCars[i].getGlobalBounds().width;
                enemyCars[i].setPosition(getRandomLane(laneX, enemyCars, spriteWidth), -100.f);
                score++;
            }
            if (checkCollision(enemyCars[i], playerspirit)) {
                return score;
                window.close();
            }
        }
        //clear window
        window.clear();
        // Draw the background image
        window.draw(backgroundSprite);
        window.draw(backgroundSprite2);
        //draw enemy cars
        for (int i = 0;i < 4;i++) {
            window.draw(enemyCars[i]);
        }
        window.draw(playerspirit);
        window.display();
    }
}

void showGameOver(sf::RenderWindow& window, sf::Font& font, int score) {
    sf::Text title("Game Over", font, 50);
    title.setFillColor(sf::Color::Red);
    title.setPosition(250, 100);

    sf::Text scoreText("Score: " + to_string(score), font, 30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(300, 200);

    sf::Text restart("Press Enter to Restart or Esc to Exit", font, 24);
    restart.setFillColor(sf::Color::Green);
    restart.setPosition(180, 300);

    window.clear();
    window.draw(title);
    window.draw(scoreText);
    window.draw(restart);
    window.display();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) return;
                if (event.key.code == sf::Keyboard::Escape) window.close();
            }
        }
    }
}
int main()
{
    string playername;
    cout << "Enter Your name : ";
    cin >> playername;
    //For background Audio
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("gameaudio.mp3")) {
        return -1;
    }
    backgroundMusic.setLoop(true);
    backgroundMusic.play();
    // Create the window
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window");
    //font size
    sf::Font font;
    if (!font.loadFromFile("font.ttf")) {
        cerr << "Failed to load font\n";
        return -1;
    }
    // displaying
    sf::Text title("CAR GAME", font, 48);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(250, 100);

    vector<sf::Text> menuOptions(3);
    vector<string> labels = { "Start Game", "Leaderboard", "Exit" };
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        menuOptions[i].setFont(font);
        menuOptions[i].setString(labels[i]);
        menuOptions[i].setCharacterSize(30);
        menuOptions[i].setPosition(300, 250 + 50 * i);
    }
    int selectedOption = 0;
    GameState state = MENU;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (state == MENU && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    selectedOption = (selectedOption + 1) % menuOptions.size();
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    if (selectedOption == 0) {
                        sf::Clock CLOCK;
                        int finalScore = game(window, font);
                        if (finalScore >= 0) {
                            saveScoreToFile(playername, finalScore);
                            showGameOver(window, font, finalScore);
                        }
                    }
                    else if (selectedOption == 1) {
                        showLeaderboard(window, font);
                    }
                    else if (selectedOption == 2) {
                        window.close();
                    }
                }

            }
        }
        if (state == MENU)
            menu(window, title, menuOptions, selectedOption);
    }
    return 0;
}

