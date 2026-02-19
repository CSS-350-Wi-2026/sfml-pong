#include <SFML/Graphics.hpp>

int main() {
	sf::RenderWindow myWindow(sf::VideoMode({ 1920, 1080 }), "Colors");
	sf::CircleShape circle(50.f);
	circle.setFillColor(sf::Color(100, 255, 50));

	while (myWindow.isOpen()) {
		while (const std::optional event = myWindow.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				myWindow.close();
			}

		}

		myWindow.clear(sf::Color::Black);
		myWindow.draw(circle);

		myWindow.display();
	}

	return 0;
}

//#include <SFML/Audio.hpp>
//#include <SFML/Graphics.hpp>
//
//int main() {
//	//std::string musicName;
//	sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "SFML Audio Player");
//	const sf::Font font("arial.ttf");
//	sf::Text text(font, "Audio Playing", 50);
//
//	//sf::Music music(musicName);
//	//music.play();
//
//	while (window.isOpen()) {
//		while (const std::optional event = window.pollEvent()) {
//			if (event->is <sf::Event::Closed>()) {
//				window.close();
//			}
//		}
//
//		window.clear();
//		window.draw(text);
//		window.display();
//	}
//
//	return 0;
//}