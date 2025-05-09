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
    CircleShape slider;       // Slider du potentiom�tre
    RectangleShape track;     // Piste du potentiom�tre
    bool isDragging = false;  // Indique si la souris est en train de glisser
    float minValue;
    float maxValue;
    float* targetValue;       // Pointeur vers la variable � modifier
    Clock clickClock;         // Horloge pour g�rer les doubles clics
    bool firstClick = false;  // Indique si un premier clic a �t� d�tect�

public:
    Potentiometer(int x, int y, float width, float height, float minVal, float maxVal, float* target)
        : minValue(minVal), maxValue(maxVal), targetValue(target) {
        // Initialiser la piste
        track.setSize(Vector2f(width, height));
        track.setFillColor(Color(150, 150, 150));
        track.setPosition((float)x+10, (float)y);

        // Initialiser le slider
        slider.setRadius(height); // Largeur fixe
        slider.setFillColor(Color(0, 0, 0));
        slider.setOrigin(slider.getRadius(), slider.getRadius());
        slider.setPosition((x + width), y + height / 2);
    }

    void handleEvent(const Event& event, const RenderWindow& window) {
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
            Vector2i mousePos = Mouse::getPosition(window);
            if (slider.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                // V�rifier si c'est un double-clic
                if (firstClick && clickClock.getElapsedTime().asMilliseconds() < 400) {
                    *targetValue = 1.0f; // R�initialiser la valeur
                    float trackStart = track.getPosition().x;
                    float trackEnd = track.getPosition().x + track.getSize().x;
                    float sliderX = trackStart + (1.0f - minValue) / (maxValue - minValue) * (trackEnd - trackStart);
                    slider.setPosition(sliderX - slider.getRadius() / 2, slider.getPosition().y);
                    // R�initialiser le double-clic
                    firstClick = false;
                }
                else {
                    // Premier clic
                    firstClick = true;
                    clickClock.restart();
                    isDragging = true; // Commence � glisser
                }
            }
        }
        if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
            isDragging = false; // Arr�te de glisser
        }
    }

    void update(const RenderWindow& window) {
        if (isDragging) {
            Vector2i mousePos = Mouse::getPosition(window);
            float sliderX = std::clamp(static_cast<float>(mousePos.x), track.getPosition().x, track.getPosition().x + track.getSize().x);
            slider.setPosition(sliderX - slider.getRadius() / 2, slider.getPosition().y);
            // Calculer la nouvelle valeur associ�e au potentiom�tre
            float trackStart = track.getPosition().x;
            float trackEnd = track.getPosition().x + track.getSize().x;
            float normalizedValue = (sliderX - trackStart) / (trackEnd - trackStart);
            *targetValue = minValue + normalizedValue * (maxValue - minValue);
        }
    }

    void draw(RenderWindow& window) const {
        window.draw(track);
        window.draw(slider);
    }
};
