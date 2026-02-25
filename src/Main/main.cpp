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

// Window Dimensions
constexpr unsigned WINDOW_W   = 800;
constexpr unsigned WINDOW_H   = 600;
// Paddle Dimensions
constexpr float    PADDLE_W   = 14.f;
constexpr float    PADDLE_H   = 90.f;

struct Paddle
{
    sf::RectangleShape shape;

	// Create a paddle at the given x position, centered vertically
    Paddle(float x)
    {
        shape.setSize({ PADDLE_W, PADDLE_H });
        shape.setOrigin({ PADDLE_W / 2.f, PADDLE_H / 2.f });
        shape.setFillColor(sf::Color::White);
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
            sf::RectangleShape dash({ 4.f, 20.f });
            dash.setFillColor(sf::Color::White);
            dash.setPosition({ WINDOW_W / 2.f - 2.f, i * (WINDOW_H / numDashes) + 10.f });
			segments.push_back(dash);
        }
    }
};


int main() {

	sf::RenderWindow window(sf::VideoMode({ WINDOW_W, WINDOW_H }), "Pong",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(144);
	
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
