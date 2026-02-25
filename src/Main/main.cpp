#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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


int main() {
	sf::RenderWindow window( sf::VideoMode( { 200, 200 } ), "SFML works!" );
	sf::CircleShape shape( 100.f );
	shape.setFillColor( sf::Color::Green );

	while ( window.isOpen() )
	{
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

		window.clear();
		window.draw( shape );
		window.display();
	}
}
