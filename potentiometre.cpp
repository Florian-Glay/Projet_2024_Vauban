#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <mutex>
using namespace sf;
using namespace std;

class Potentiometer {
private:
    CircleShape slider;       // Slider du potentiomètre
    RectangleShape track;     // Piste du potentiomètre
    bool isDragging = false;  // Indique si la souris est en train de glisser
    float minValue;
    float maxValue;
    float* targetValue;       // Pointeur vers la variable à modifier
    Clock clickClock;         // Horloge pour gérer les doubles clics
    bool firstClick = false;  // Indique si un premier clic a été détecté

public:
    Potentiometer(int x, int y, float width, float height, float minVal, float maxVal, float* target)
        : minValue(minVal), maxValue(maxVal), targetValue(target) {
        // Initialiser la piste
        track.setSize(Vector2f(width, height));
        track.setFillColor(Color(150, 150, 150));
        track.setPosition((float)x, (float)y);

        // Initialiser le slider
        slider.setRadius(height / 2); // Largeur fixe
        slider.setFillColor(Color(0, 0, 0));
        slider.setOrigin(slider.getRadius(), slider.getRadius());
        slider.setPosition((x + width), y + height / 2);
    }

    void handleEvent(const Event& event, const RenderWindow& window) {
        Vector2i mousePos = Mouse::getPosition(window);

        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
            if (slider.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                if (firstClick && clickClock.getElapsedTime().asMilliseconds() < 400) {
                    // Double clic pour réinitialiser la valeur
                    *targetValue = minValue;
                    float trackStart = track.getPosition().x;
                    slider.setPosition(trackStart, slider.getPosition().y);
                    firstClick = false;
                }
                else {
                    firstClick = true;
                    clickClock.restart();
                    isDragging = true;
                }
            }
        }

        if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
            isDragging = false;
        }
    }

    void update(const RenderWindow& window) {
        if (isDragging) {
            Vector2i mousePos = Mouse::getPosition(window);
            float trackStart = track.getPosition().x;
            float trackEnd = track.getPosition().x + track.getSize().x;
            float sliderX = std::clamp(static_cast<float>(mousePos.x), trackStart, trackEnd);

            // Mettre à jour la position du slider
            slider.setPosition(sliderX, slider.getPosition().y);

            // Calculer la valeur associée
            float normalizedValue = (sliderX - trackStart) / (trackEnd - trackStart);
            *targetValue = minValue + normalizedValue * (maxValue - minValue);
        }
    }

    void draw(RenderWindow& window) const {
        window.draw(track);
        window.draw(slider);
    }
};
