#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <cmath> //std::pow, std::cos

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;
const int FRAMES_PER_ANIMATION = 60;

// global tween function
std::function<float(float, float, float)> tween = [](float a, float b, float t) {
    return (1 - t) * a + t * b;
};

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        // TODO: (Q2)
        //  implement key presses (1-9) that replace the tween function
        //  with different alternate tween functions.
        //  Functions can be from lecture or from https://easings.net/#
        // ====== ====== ======
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Num1:
                    // Ease-in quad: starts slow, speeds up
                    tween = [](float a, float b, float t) {
                        float e = t * t;
                        return (1 - e) * a + e * b;
                    };
                    break;
                
                case sf::Keyboard::Key::Num2:
                    // Ease-out quad: starts fast, slows down
                    tween = [](float a, float b, float t) {
                        float e = 1 - (1 - t) * (1 - t);
                        return (1 - e) * a + e * b;
                    };
                    break;
                
                case sf::Keyboard::Key::Num3:
                    // Ease-in-out cubic: slow, fast, slow
                    tween = [](float a, float b, float t) {
                        float e = (t < 0.5f)
                            ? 4 * t * t * t
                            : 1 - std::pow(-2 * t + 2, 3) / 2;
                        return (1 - e) * a + e * b;
                    };
                    break;
                
                case sf::Keyboard::Key::Num4:
                    // Ease-in sine: gentle acceleration
                    tween = [](float a, float b, float t) {
                        float e = 1 - std::cos(t * 3.14159265f / 2);
                        return (1 - e) * a + e * b;
                    };
                    break;
                
                case sf::Keyboard::Key::Num5:
                    // Ease-out cubic: fast start, long smooth slowdown
                    tween = [](float a, float b, float t) {
                        float e = 1 - std::pow(1 - t, 3);
                        return (1 - e) * a + e * b;
                    };
                    break;
                
                case sf::Keyboard::Key::Num6:
                    // Ease-out sine: gentle deceleration
                    tween = [](float a, float b, float t) {
                        float e = std::sin(t * 3.14159265f / 2);
                        return (1 - e) * a + e * b;
                    };
                    break;

                case sf::Keyboard::Key::Num7:
                    // Ease-in expo: almost still, then shoots off at the end
                    tween = [](float a, float b, float t) {
                        float e = (t == 0.0f) ? 0.0f : std::pow(2.0f, 10 * t - 10);
                        return (1 - e) * a + e * b;
                    };
                    break;
                
                case sf::Keyboard::Key::Num9:
                    // Ease-in cubic: slow start, strong acceleration
                    tween = [](float a, float b, float t) {
                        float e = t * t * t;
                        return (1 - e) * a + e * b;
                    };
                    break;
                
                case sf::Keyboard::Key::Num8:
                // Ease-out back: overshoots the target, then settles
                tween = [](float a, float b, float t) {
                    const float c1 = 1.70158f;
                    const float c3 = c1 + 1;
                    float e = 1 + c3 * std::pow(t - 1, 3) + c1 * std::pow(t - 1, 2);
                    return (1 - e) * a + e * b;
                };
                break;
                
                default:
                    break;
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    // Clear with blue background (sky)
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Q1) Draw circle that moves between
    // the left/right half of the screen.
    // Movement should be governed by the tween function.
    // ====== ====== ======
    sf::CircleShape circle;
    circle.setFillColor(sf::Color::White);
    circle.setRadius(10.0f);
    static int frame = 0;
    float t = static_cast<float>(frame % FRAMES_PER_ANIMATION) / FRAMES_PER_ANIMATION;
    float x = tween(0.0f, WINDOW_WIDTH, t);
    circle.setPosition(sf::Vector2f{x, (WINDOW_HEIGHT / 3.0f)});
    window.draw(circle);
    frame++;

    // ====== ====== ======
    // TODO: (Q3) Draw tween function graph with a dot
    // on the current portion of the curve
    // ====== ====== ======
    // Make a graph box in the bottom left
    const float graphSize = 200.0f;
    const float graphLeft = 300.0f;
    const float graphBottom = WINDOW_HEIGHT - 100.0f;
    const float graphTop = graphBottom - graphSize;

    // Outline of the graph
    sf::RectangleShape box(sf::Vector2f{graphSize, graphSize});
    box.setPosition(sf::Vector2f{graphLeft, graphTop});
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Green);
    box.setOutlineThickness(1.0f);
    window.draw(box);

    // The curve: the tween function from t = 0 to 1
    const int NUM_SAMPLES = 100;
    sf::VertexArray curve(sf::PrimitiveType::LineStrip);
    for (int i = 0; i <= NUM_SAMPLES; i++) {
        float s = static_cast<float>(i) / NUM_SAMPLES;
        float value = tween(0.0f, 1.0f, s);
        float px = graphLeft + s * graphSize;
        float py = graphBottom - value * graphSize;
        curve.append(sf::Vertex{sf::Vector2f{px, py}, sf::Color::White});
    }
    window.draw(curve);

    // The dot: current position on the curve
    float currentValue = tween(0.0f, 1.0f, t);
    sf::CircleShape dot(5.0f);
    dot.setFillColor(sf::Color::Red);
    dot.setOrigin(sf::Vector2f{5.0f, 5.0f});   // center the dot on the point
    dot.setPosition(sf::Vector2f{
        graphLeft + t * graphSize,
        graphBottom - currentValue * graphSize
    });
    window.draw(dot);


    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
