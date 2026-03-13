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
#include <unordered_map>
#include <functional>
#include <cstring>
#include <fstream>

// Font file — place DejaVuSans.ttf in the same directory as the executable.
static const char FONT_PATH[] = "DejaVuSans.ttf";



// Window Dimensions
constexpr unsigned WINDOW_W   = 800;
constexpr unsigned WINDOW_H   = 600;
// Paddle Dimensions
constexpr float    PADDLE_W   = 14.f;
constexpr float    PADDLE_H   = 90.f;
// Ball Radius
constexpr float    BALL_R     = 9.f;
// Object Speeds
constexpr float    PADDLE_SPD = 2.f;
constexpr float    BALL_SPD   = 200.f;
constexpr float    BALL_SPD_INCREMENT = 0.1f; // Increment to increase ball speed after each paddle hit
constexpr float    AI_SPD     = 1.8f;         // AI paddle speed (slightly slower than player)
// Winning Score
constexpr int      WIN_SCORE  = 7;
// Keybinds
std::vector<sf::Keyboard::Key> UP_KEYS = {sf::Keyboard::Key::W, sf::Keyboard::Key::Up};
std::vector<sf::Keyboard::Key> DOWN_KEYS = { sf::Keyboard::Key::S, sf::Keyboard::Key::Down};

// Midline Dash Dimensions
constexpr float    DASH_W = 4.f;
constexpr float    DASH_H = 20.f;
// Theme Color
constexpr sf::Color THEME_COLOR = sf::Color::White;
// framerate
int fps = 60;
// background color
sf::Color bkgColor = sf::Color::Black;
std::string bkgColorStr = "black"; //string version

// Game States
enum class State { Menu, Playing, Paused, GameOver };

// Paddle Structure
struct Paddle
{
private:
	float yVelocity = 0; // Velocity of the paddle in the vertical direction
public:
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

    // Set the vertical velocity of the paddle
    void setYVelocity(float velocity) { yVelocity = velocity; }

	// Move the paddle by a certain amount
	void moveY() 
    { 
		if (bounds().position.y + yVelocity < 0 || bounds().position.y + bounds().size.y + yVelocity > WINDOW_H) {
            yVelocity = 0; // Stop movement if it would go out of bounds
        }
        shape.move({ 0.f, yVelocity }); 
    }
    // Move AI paddle smoothly toward a target Y
    void aiMoveTo(float targetY)
    {
        float paddleY = shape.getPosition().y;
        float diff    = targetY - paddleY;
        float move    = std::min(std::abs(diff), AI_SPD);
        if (std::abs(diff) > 2.f)
            shape.move({ 0.f, diff > 0.f ? move : -move });
        clamp();
    }
};

struct Ball
{
    sf::CircleShape shape;
    sf::Vector2f    vel;
    float           speed = BALL_SPD;
	int             bounces = 0;

    Paddle* leftPaddle = nullptr;
    Paddle* rightPaddle = nullptr;

    // Sound callbacks — set from main() so Ball can trigger sounds
    std::function<void()> onPaddleHit;
    std::function<void()> onWallHit;

    Ball(Paddle& left, Paddle& right)
    {
        shape.setRadius(BALL_R);
        shape.setOrigin({ BALL_R, BALL_R });
        shape.setFillColor(sf::Color::White);

        leftPaddle = &left;
        rightPaddle = &right;
    }

    void reset(bool leftServe)
    {
        shape.setPosition({ WINDOW_W / 2.f, WINDOW_H / 2.f });
        speed = BALL_SPD;
        bounces = 0;

        float angle = (std::rand() % 60 - 30) * 3.14159f / 180.f;
        float dirX = leftServe ? 1.f : -1.f;

        vel = { dirX * speed * std::cos(angle),
               speed * std::sin(angle) };
    }

    sf::FloatRect bounds() const
    {
        return shape.getGlobalBounds();
    }

    int update(float dt)
    {
        //move Ball
        shape.move(vel * dt);

        sf::FloatRect ballRect = bounds();
        sf::FloatRect leftRect = leftPaddle->bounds();
        sf::FloatRect rightRect = rightPaddle->bounds();

        //Paddle collisions
        if (ballRect.findIntersection(leftRect).has_value())
        {
            vel.x = std::abs(vel.x); //bounce right
            vel *= (1.f + BALL_SPD_INCREMENT);

             // Push ball out of paddle to prevent sticking
            shape.setPosition({ leftRect.position.x + leftRect.size.x + BALL_R,
                                 shape.getPosition().y });
			bounces++; //increment bounce count for speed increase
            if (onPaddleHit) { onPaddleHit(); }
        }

        if (ballRect.findIntersection(rightRect).has_value())
        {
            vel.x = -std::abs(vel.x); //bounce left
            vel *= (1.f + BALL_SPD_INCREMENT);

            shape.setPosition({ rightRect.position.x - BALL_R,
                                 shape.getPosition().y });
			bounces++; //increment bounce count for speed increase
            if (onPaddleHit) { onPaddleHit(); }
        }

        //recompute after bounce
        ballRect = bounds();

        //wall collisions
        if (ballRect.position.y <= 0.f)
        {
            vel.y = std::abs(vel.y); //bounce down
            if (onWallHit) { onWallHit(); }
        }
        else if (ballRect.position.y + ballRect.size.y >= WINDOW_H)
        {
            vel.y = -std::abs(vel.y); //bounce up
            if (onWallHit) { onWallHit(); }
        }

        // Scoring
        float bx = shape.getPosition().x;
        if (bx < -BALL_R)            return -1; // AI scores
        if (bx > WINDOW_W + BALL_R)  return  1; // Player scores
        return 0;
    }
};

struct MidLine
{
	std::vector<sf::RectangleShape> segments;

	//create a dashed midline with the specified number of dashes
    MidLine(int numDashes) {
        for (int i = 0; i < numDashes; ++i) {
            sf::RectangleShape dash({ DASH_W, DASH_H });
            dash.setFillColor(THEME_COLOR);
            dash.setPosition({ WINDOW_W / 2.f - 2.f, i * (WINDOW_H / numDashes) + 10.f });
			segments.push_back(dash);
        }
    }
};

//convert a given string to sf::Color object, all lower case, if invalid string, return black
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

//load user settings from a txt file called config.txt, if file not found, create one with default value
//file structure should be fps"\n"color
void loadConfig() {
    std::ifstream readFile("config.txt");

    if (!readFile.is_open()) {
        std::ofstream writeFile("config.txt");

        writeFile << fps << "\n" << "black";
        writeFile.close();
    }
    else {
        std::string temp;
        readFile >> fps;
        readFile >> temp;
        bkgColor = stoC(temp);
        readFile.close();
    }
}

//update the user settings file, will create a file if no file found
//file structure should be fps"\n"color
void updateConfig(const int& fps, const std::string& color) {
    std::ofstream writeFile("config.txt", std::ios::trunc);

    if (writeFile.is_open()) {
        writeFile << fps << "\n" << color;
    }
    else {
        std::cerr << "Error: cannot save config" << std::endl;
    }

    writeFile.close();
}

//window for settings menu
void settingsWindow(const sf::Font& font)
{
    sf::RenderWindow settings(sf::VideoMode({ 400, 260 }), "Settings");

    sf::Text bgColorLabel(font, "Background color:", 20);
    bgColorLabel.setFillColor(sf::Color::Black);
    bgColorLabel.setPosition({ 20, 15 });

    sf::Text maxFpsLabel(font, "Max FPS:", 20);
    maxFpsLabel.setFillColor(sf::Color::Black);
    maxFpsLabel.setPosition({ 20, 105 });

    sf::Text colorInput(font, "", 18);
    colorInput.setFillColor(sf::Color::Black);
    colorInput.setPosition({ 28, 60 });

    sf::Text fpsInput(font, "", 18);
    fpsInput.setFillColor(sf::Color::Black);
    fpsInput.setPosition({ 28, 148 });

    sf::Text applyLabel(font, "Close window to apply", 15);
    applyLabel.setFillColor(sf::Color(80, 80, 80));
    applyLabel.setPosition({ 20, 220 });

    sf::RectangleShape colorBox({ 360.f, 32.f });
    colorBox.setPosition({ 20, 55 });
    colorBox.setFillColor(sf::Color::White);
    colorBox.setOutlineThickness(2);
    colorBox.setOutlineColor(sf::Color(120, 120, 120));

    sf::RectangleShape fpsBox({ 360.f, 32.f });
    fpsBox.setPosition({ 20, 143 });
    fpsBox.setFillColor(sf::Color::White);
    fpsBox.setOutlineThickness(2);
    fpsBox.setOutlineColor(sf::Color(120, 120, 120));

    std::string colorStr, fpsStr;
    bool onColor = true, onFps = false;

    while (settings.isOpen())
    {
        while (const auto ev = settings.pollEvent())
        {
            if (ev->is<sf::Event::Closed>()) settings.close();

            if (const auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f mp(static_cast<float>(mb->position.x),
                                static_cast<float>(mb->position.y));
                if      (colorBox.getGlobalBounds().contains(mp)) { onColor = true;  onFps = false; }
                else if (fpsBox.getGlobalBounds().contains(mp))   { onColor = false; onFps = true;  }
            }

            if (const auto* te = ev->getIf<sf::Event::TextEntered>()) {
                char ch = static_cast<char>(te->unicode);
                if (te->unicode == '\b') {
                    if (onColor && !colorStr.empty()) colorStr.pop_back();
                    else if (onFps && !fpsStr.empty()) fpsStr.pop_back();
                } else if (te->unicode < 128) {
                    if (onColor) colorStr += ch;
                    else if (onFps && std::isdigit(ch)) fpsStr += ch;
                }
            }
        }

        colorInput.setString(colorStr);
        fpsInput.setString(fpsStr);
        colorBox.setOutlineColor(onColor ? sf::Color::Blue : sf::Color(120,120,120));
        fpsBox.setOutlineColor  (onFps   ? sf::Color::Blue : sf::Color(120,120,120));

        settings.clear(sf::Color(220, 220, 220));
        settings.draw(colorBox);
        settings.draw(fpsBox);
        settings.draw(bgColorLabel);
        settings.draw(maxFpsLabel);
        settings.draw(colorInput);
        settings.draw(fpsInput);
        settings.draw(applyLabel);
        settings.display();
    }

    if (!fpsStr.empty()) { 
        fps = std::stoi(fpsStr); 
        updateConfig(fps, bkgColorStr);
    }
    if (!colorStr.empty()) { 
        bkgColor = stoC(colorStr);
        bkgColorStr = colorStr;
        updateConfig(fps, colorStr);
    }
}


// Helper function to generate a tone of a specific frequency and duration, returns an sf::SoundBuffer that can be used to play the sound
sf::SoundBuffer makeTone(float freqHz, float durationSec, float amplitude = 0.4f)
{
    const unsigned sampleRate  = 44100;
    const unsigned sampleCount = static_cast<unsigned>(sampleRate * durationSec);

    std::vector<std::int16_t> samples(sampleCount);
    for (unsigned i = 0; i < sampleCount; ++i)
    {
        float t    = static_cast<float>(i) / sampleRate;
        float env  = 1.f - (t / durationSec);          // linear fade-out
        samples[i] = static_cast<std::int16_t>(
            amplitude * env * 32767.f *
            std::sin(2.f * 3.14159265f * freqHz * t));
    }

    sf::SoundBuffer buf;
    // loadFromSamples
    if (!buf.loadFromSamples(samples.data(), sampleCount, 1, sampleRate,
                             { sf::SoundChannel::Mono }))
    {
        std::cerr << "makeTone: loadFromSamples failed for " << freqHz << " Hz\n";
    }
    return buf;
}


int main() {
    sf::Listener::setGlobalVolume(100.f);
	sf::RenderWindow window(sf::VideoMode({ WINDOW_W, WINDOW_H }), "Pong",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(fps);

     // Fonts
    sf::Font font;
    if (!font.openFromFile(FONT_PATH)) {
        std::cerr << "WARNING: Could not load " << FONT_PATH
                  << " — place DejaVuSans.ttf next to the executable\n";
    }

    //load user settings
    loadConfig();

    // Sounds
    sf::SoundBuffer bufPaddle = makeTone(480.f, 0.06f);
    sf::SoundBuffer bufWall   = makeTone(300.f, 0.05f);
    sf::SoundBuffer bufScore  = makeTone(180.f, 0.35f, 0.6f);
    sf::SoundBuffer bufWin    = makeTone(660.f, 0.6f,  0.7f);

    sf::Sound sndPaddle(bufPaddle);
    sf::Sound sndWall  (bufWall);
    sf::Sound sndScore (bufScore);
    sf::Sound sndWin   (bufWin);
	
	//create Paddles for players 1 and 2
    Paddle player1Paddle(40.f);
    Paddle player2Paddle(WINDOW_W - 40.f);

	//create Ball
	Ball ball(player1Paddle, player2Paddle);
    ball.onPaddleHit = [&]() { sndPaddle.play(); };
    ball.onWallHit   = [&]() { sndWall.play();   };
	ball.reset(true); // Start with player serve

	//create Midline
	MidLine midline(15);

    // Scores & state
    int   playerScore = 0;
    int   aiScore     = 0;
    State state = State::Menu;

    sf::Clock clock;

    // Text helpers
    auto makeText = [&](const std::string& s, unsigned sz,
                        sf::Color col = sf::Color::White) -> sf::Text
    {
        sf::Text t(font, s, sz);
        t.setFillColor(col);
        return t;
    };

    auto centreText = [&](sf::Text& t, float y)
    {
        sf::FloatRect r = t.getLocalBounds();
        t.setOrigin({ r.position.x + r.size.x / 2.f,
                      r.position.y + r.size.y / 2.f });
        t.setPosition({ WINDOW_W / 2.f, y });
    };


	//game loop
	while ( window.isOpen() )
	{
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);

        // Events
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() ) {
				window.close();
			}
            if (const auto* kp = event->getIf<sf::Event::KeyPressed>())
            {
                switch (kp->code)
                {
                    case sf::Keyboard::Key::Tab:
                        if (state == State::Menu)
                        {
                            settingsWindow(font);
                            window.setFramerateLimit(fps);
                        }
                        break;

                    case sf::Keyboard::Key::Escape:
                        if      (state == State::Playing) { state = State::Paused; }
                        else if (state == State::Paused)   { state = State::Playing; }
                        break;

                    case sf::Keyboard::Key::Enter:
                    case sf::Keyboard::Key::Space:
                        if (state == State::Menu || state == State::GameOver)
                        {
                            playerScore = 0;
                            aiScore     = 0;
                            player1Paddle.shape.setPosition({ 40.f,            WINDOW_H / 2.f });
                            player2Paddle.shape.setPosition({ WINDOW_W - 40.f, WINDOW_H / 2.f });
                            ball.reset(true);
                            state = State::Playing;
                        }
                        else if (state == State::Paused)
                        {
                            state = State::Playing;
                        }
                        break;

                    default: break;
                }
            }            
		}

        // Update
        if (state == State::Playing)
        {
            // Player 1 input
            if      (sf::Keyboard::isKeyPressed(UP_KEYS[0]))   player1Paddle.setYVelocity(-PADDLE_SPD);
            else if (sf::Keyboard::isKeyPressed(DOWN_KEYS[0])) player1Paddle.setYVelocity(PADDLE_SPD);
            else {                                               player1Paddle.setYVelocity(0.f); }
            player1Paddle.moveY();

            // AI tracks the ball
            player2Paddle.aiMoveTo(ball.shape.getPosition().y);

            // Ball update + scoring
            int scored = ball.update(dt);
            if (scored == 1)        // player scored
            {
                ++playerScore;
                sndScore.play();
                if (playerScore >= WIN_SCORE) { sndWin.play(); state = State::GameOver; }
                else { ball.reset(false); }
            }
            else if (scored == -1)  // AI scored
            {
                ++aiScore;
                sndScore.play();
                if (aiScore >= WIN_SCORE) { sndWin.play(); state = State::GameOver; }
                else { ball.reset(true); }
            }
        }

        // Drawing
		window.clear(bkgColor);

		//draw Paddles
        window.draw(player1Paddle.shape);
        window.draw(player2Paddle.shape);

		//draw Midline
        for (const auto& dash : midline.segments) {window.draw(dash);}

        // draw Ball only if game is active
         if (state == State::Playing || state == State::Paused) {
            window.draw(ball.shape);
        }

        // Scores
        {
            auto p = makeText(std::to_string(playerScore), 64);
            p.setPosition({ WINDOW_W / 2.f - 80.f, 18.f });
            window.draw(p);
            auto a = makeText(std::to_string(aiScore), 64);
            a.setPosition({ WINDOW_W / 2.f + 36.f, 18.f });
            window.draw(a);
        }


        // Overlay Menu
        if (state == State::Menu)
        {
            auto title = makeText("PONG", 88);
            centreText(title, WINDOW_H / 2.f - 70.f);
            window.draw(title);

            auto controls = makeText("W / S  or  UP / DOWN  to move", 22, sf::Color(180,180,180));
            centreText(controls, WINDOW_H / 2.f + 10.f);
            window.draw(controls);

            auto startHint = makeText("SPACE or ENTER to start", 22, sf::Color(180,180,180));
            centreText(startHint, WINDOW_H / 2.f + 45.f);
            window.draw(startHint);

            auto tabHint = makeText("TAB = settings     ESC = pause", 17, sf::Color(110,110,110));
            centreText(tabHint, WINDOW_H / 2.f + 85.f);
            window.draw(tabHint);
        }

        // Overlay: Paused
        if (state == State::Paused)
        {
            sf::RectangleShape dim({ static_cast<float>(WINDOW_W), static_cast<float>(WINDOW_H) });
            dim.setFillColor(sf::Color(0, 0, 0, 140));
            window.draw(dim);

            auto t = makeText("PAUSED", 72);
            centreText(t, WINDOW_H / 2.f - 30.f);
            window.draw(t);

            auto sub = makeText("ESC or SPACE to resume", 24, sf::Color(180,180,180));
            centreText(sub, WINDOW_H / 2.f + 40.f);
            window.draw(sub);
        }

        // Overlay: Game Over
        if (state == State::GameOver)
        {
            sf::RectangleShape dim({ static_cast<float>(WINDOW_W), static_cast<float>(WINDOW_H) });
            dim.setFillColor(sf::Color(0, 0, 0, 160));
            window.draw(dim);

            std::string msg = (playerScore >= WIN_SCORE) ? "YOU WIN!" : "AI WINS!";
            auto t = makeText(msg, 72);
            centreText(t, WINDOW_H / 2.f - 50.f);
            window.draw(t);

            auto scoreLine = makeText(
                std::to_string(playerScore) + "  —  " + std::to_string(aiScore),
                36, sf::Color(200,200,200));
            centreText(scoreLine, WINDOW_H / 2.f + 10.f);
            window.draw(scoreLine);

            auto sub = makeText("ENTER to play again", 26, sf::Color(160,160,160));
            centreText(sub, WINDOW_H / 2.f + 60.f);
            window.draw(sub);
        }



		//opens Window
		window.display();
	}
    return 0;
}
