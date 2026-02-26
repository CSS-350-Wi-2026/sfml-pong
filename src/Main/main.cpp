#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Export.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

// Window Dimensions
constexpr unsigned WINDOW_W   = 800;
constexpr unsigned WINDOW_H   = 600;
// Paddle Dimensions
constexpr float    PADDLE_W   = 14.f;
constexpr float    PADDLE_H   = 90.f;
// Midline Dash Dimensions
constexpr float    DASH_W = 4.f;
constexpr float    DASH_H = 20.f;
// Theme Color
constexpr sf::Color THEME_COLOR = sf::Color::White;
// framerate
int fps = 60;
// background color
sf::Color bkgColor = sf::Color::Black;

struct Paddle
{
    sf::RectangleShape shape;

	// Create a paddle at the given x position, centered vertically
    Paddle(float x)
    {
        shape.setSize({ PADDLE_W, PADDLE_H });
        shape.setOrigin({ PADDLE_W / 2.f, PADDLE_H / 2.f });
        shape.setFillColor(THEME_COLOR);
        shape.setPosition({ x, WINDOW_H / 2.f });
    }

	// Ensure the paddle stays within the window bounds
    void clamp()
    {
        float y = shape.getPosition().y;
        y = std::max(PADDLE_H / 2.f, std::min(WINDOW_H - PADDLE_H / 2.f, y));
        shape.setPosition({ shape.getPosition().x, y });
    }

	// Get the bounding box of the paddle for collision detection
    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
};

struct MidLine
{
	std::vector<sf::RectangleShape> segments;

	// Create a dashed midline with the specified number of dashes
    MidLine(int numDashes) {
        for (int i = 0; i < numDashes; ++i) {
            sf::RectangleShape dash({ DASH_W, DASH_H });
            dash.setFillColor(THEME_COLOR);
            dash.setPosition({ WINDOW_W / 2.f - 2.f, i * (WINDOW_H / numDashes) + 10.f });
			segments.push_back(dash);
        }
    }
};

//convert a given string to sf::Color object. if invalid string, return black
sf::Color stoC(std::string colorStr) {
    static const std::unordered_map<std::string, sf::Color> colorMap{
        {"black",   sf::Color::Black},
        {"white",   sf::Color::White},
        {"red",     sf::Color::Red},
        {"green",   sf::Color::Green},
        {"blue",    sf::Color::Blue},
        {"yellow",  sf::Color::Yellow},
        {"magenta", sf::Color::Magenta},
        {"cyan",    sf::Color::Cyan}
    };

    auto itera = colorMap.find(colorStr);
    if (itera != colorMap.end()) {
        return itera->second;
    }

    return sf::Color::Black;
}

//window for settings menu, user input background color and fps
void settingsWindow() {
    sf::RenderWindow settings(sf::VideoMode({ 400, 250 }), "Settings");

    const sf::Font font("arial.ttf");

    //label
    sf::Text bgColor(font, "Enter background color: ", 20);
    bgColor.setPosition({ 20, 50 });
    sf::Text maxFps(font, "Entermax fps: ", 20);
    maxFps.setPosition({ 20, 120 });

    //display input text
    sf::Text colorInput(font, "", 18);
    colorInput.setPosition({ 20, 60 });
    sf::Text fpsInput(font, "", 18);
    fpsInput.setPosition({ 20, 140 });

    //text box
    sf::RectangleShape colorBox(sf::Vector2f(360, 30));
    colorBox.setPosition({ 20, 55 });
    colorBox.setFillColor(sf::Color::White);
    colorBox.setOutlineThickness(2);
    colorBox.setOutlineColor(sf::Color::Black);

    sf::RectangleShape fpsBox(sf::Vector2f(360, 30));
    fpsBox.setPosition({ 20, 135 });
    fpsBox.setFillColor(sf::Color::White);
    fpsBox.setOutlineThickness(2);
    fpsBox.setOutlineColor(sf::Color::Black);

    std::string colorStr;
    std::string fpsStr;
    bool onColor = true;
    bool onFps = false;

    while (settings.isOpen()) {
        while (const auto event = settings.pollEvent()) {
            if (event->is<sf::Event::Closed>()){
                settings.close();
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f mousePos(
                    static_cast<float>(mouse->position.x),
                    static_cast<float>(mouse->position.y)
                );

                if (colorBox.getGlobalBounds().contains(mousePos)) {
                    onColor = true;
                    onFps = false;
                }
                else if (fpsBox.getGlobalBounds().contains(mousePos)) {
                    onColor = false;
                    onFps = true;
                }
            }

            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                char input = static_cast<char>(textEvent->unicode);

                if (textEvent->unicode == '\b') {   //backspace
                    if (onColor && !colorStr.empty()) {
                        colorStr.pop_back();
                    }
                    else if (onFps && !fpsStr.empty()) {
                        fpsStr.pop_back();
                    }
                }
                else if (textEvent->unicode < 128) {
                    if (onColor) {
                        colorStr += input;
                    }
                    else if (onFps && std::isdigit(input)) {
                        fpsStr += input;
                    }
                }
            }
        }

        colorInput.setString(colorStr);
        fpsInput.setString(fpsStr);

        settings.clear(sf::Color());
        settings.draw(bgColor);
        settings.draw(maxFps);
        settings.draw(colorBox);
        settings.draw(fpsBox);
        settings.draw(colorInput);
        settings.draw(fpsInput);
        settings.display();
    }

    if (!fpsStr.empty()) {
        fps = std::stoi(fpsStr);
    }
    if (!colorStr.empty()) {
        bkgColor = stoC(colorStr);
    }
}

int main() {

	sf::RenderWindow window(sf::VideoMode({ WINDOW_W, WINDOW_H }), "Pong",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(fps);
	
	// Create Paddles for player and AI
    Paddle playerPaddle(40.f);
    Paddle aiPaddle(WINDOW_W - 40.f);

	// Create Midline
	MidLine midline(15);

	// Game Loop
	while ( window.isOpen() )
	{
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

		window.clear();
		// Draw Paddles
        window.draw(playerPaddle.shape);
        window.draw(aiPaddle.shape);

		// Draw Midline
        for (const auto& dash : midline.segments) {window.draw(dash);}

		// Opens Window
		window.display();
	}
}
