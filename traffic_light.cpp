#include <iostream>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <memory>

using namespace std;
using namespace sf;

#ifdef _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define _PATH_IMG_ "C:/Program Files/SFML/img/"
#else
#define _PATH_IMG_ "img/"
#endif

const std::string path_image(_PATH_IMG_);

enum class FeuEtat { Rouge, Orange, Vert };

// Classe pour les feux de circulation
class FeuCirculation {
private:
    FeuEtat etat;
    std::mutex mtx;
    std::condition_variable cv;

public:
    Vector2f position; // Position du feu

    FeuCirculation(Vector2f pos) : etat(FeuEtat::Rouge), position(pos) {}

    void changerEtat() {
        std::lock_guard<std::mutex> lock(mtx);
        if (etat == FeuEtat::Rouge) {
            etat = FeuEtat::Vert;  // Passer de Rouge à Vert
        }
        else if (etat == FeuEtat::Vert) {
            etat = FeuEtat::Orange;  // Passer de Vert à Orange
        }
        else if (etat == FeuEtat::Orange) {
            etat = FeuEtat::Rouge;  // Passer d'Orange à Rouge
        }
        cv.notify_all();
    }

    FeuEtat obtenirEtat() {
        std::lock_guard<std::mutex> lock(mtx);
        return etat;
    }
};

// Classe de base pour un usager
class Usager {
protected:
    sf::RectangleShape shape;
    int vitesse;
    FeuCirculation& feu;
    int directionX, directionY; // Direction de déplacement
    Vector2f limiteArret;      // Position où s'arrêter au feu rouge
    bool feuDepasse;           // Indique si le véhicule a dépassé le feu

public:
    Usager(int pos_x, int pos_y, int vitesse_, FeuCirculation& feu_, sf::Color color, int dirX, int dirY)
        : vitesse(vitesse_), feu(feu_), directionX(dirX), directionY(dirY), feuDepasse(false) {
        shape.setSize(sf::Vector2f(20, 20));
        shape.setPosition(pos_x, pos_y);
        shape.setFillColor(color);

        // Calcul de la limite d'arrêt au feu
        limiteArret = feu.position - Vector2f(dirX * 30, dirY * 30); // 30 pixels avant le feu
    }

    virtual void deplacer() {
        while (true) {
            Vector2f pos = shape.getPosition();

            // Vérifier si le véhicule a dépassé le feu
            if (!feuDepasse &&
                ((directionX > 0 && pos.x >= feu.position.x) ||
                    (directionX < 0 && pos.x + 20 <= feu.position.x) ||
                    (directionY > 0 && pos.y >= feu.position.y) ||
                    (directionY < 0 && pos.y + 20 <= feu.position.y))) {
                feuDepasse = true; // Le véhicule ne sera plus impacté par le feu
            }

            // Si le feu est rouge ou orange et que le véhicule n'a pas dépassé, il s'arrête avant le feu
            if (!feuDepasse && (feu.obtenirEtat() == FeuEtat::Rouge || feu.obtenirEtat() == FeuEtat::Orange) &&
                ((directionX != 0 && ((directionX > 0 && pos.x + 20 >= limiteArret.x) ||
                    (directionX < 0 && pos.x <= limiteArret.x))) ||
                    (directionY != 0 && ((directionY > 0 && pos.y + 20 >= limiteArret.y) ||
                        (directionY < 0 && pos.y <= limiteArret.y))))) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            // Déplacement normal
            shape.move(vitesse * directionX, vitesse * directionY);

            // Si le véhicule dépasse les limites de la fenêtre, il revient au point de départ
            if (pos.x < -20 || pos.x > 900 || pos.y < -20 || pos.y > 900) {
                resetPosition();
                feuDepasse = false; // Réinitialiser l'état au point de départ
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    sf::RectangleShape& getShape() { return shape; }

    virtual void resetPosition() {
        // Remet le véhicule au point de départ en fonction de sa direction
        if (directionX > 0)
            shape.setPosition(0, shape.getPosition().y);  // Revenir à gauche
        else if (directionX < 0)
            shape.setPosition(880, shape.getPosition().y); // Revenir à droite
        else if (directionY > 0)
            shape.setPosition(shape.getPosition().x, 0);  // Revenir en haut
        else if (directionY < 0)
            shape.setPosition(shape.getPosition().x, 880); // Revenir en bas
    }
};

// Contrôle des feux de circulation
void controleFeux(FeuCirculation& feu_NS, FeuCirculation& feu_EO) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        feu_NS.changerEtat();
        feu_EO.changerEtat();
    }
}

// Fonction principale
int main() {
    // Initialisation des feux
    FeuCirculation feu_NS(Vector2f(400, 200));
    FeuCirculation feu_EO(Vector2f(300, 400));

    // Création des usagers
    std::vector<std::unique_ptr<Usager>> usagers;
    usagers.emplace_back(std::make_unique<Usager>(400, 0, 2, feu_NS, sf::Color::Blue, 0, 1));  // Voiture Nord-Sud
    usagers.emplace_back(std::make_unique<Usager>(0, 300, 2, feu_EO, sf::Color::Green, 1, 0)); // Voiture Est-Ouest
    usagers.emplace_back(std::make_unique<Usager>(500, 880, 2, feu_NS, sf::Color::Red, 0, -1)); // Voiture Sud-Nord
    usagers.emplace_back(std::make_unique<Usager>(880, 500, 2, feu_EO, sf::Color::Yellow, -1, 0)); // Voiture Ouest-Est

    // Threads pour les usagers
    std::vector<std::thread> threads;
    for (auto& usager : usagers) {
        threads.emplace_back(&Usager::deplacer, usager.get());
    }

    // Thread pour contrôler les feux
    threads.emplace_back(controleFeux, std::ref(feu_NS), std::ref(feu_EO));

    // Fenêtre graphique
    sf::RenderWindow window(sf::VideoMode(900, 900), "Simulation de Carrefour");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Effacer la fenêtre
        window.clear(sf::Color::White);

        // Dessiner les feux
        sf::CircleShape feuNS(10);
        feuNS.setPosition(feu_NS.position);
        if (feu_NS.obtenirEtat() == FeuEtat::Vert)
            feuNS.setFillColor(sf::Color::Green);
        else if (feu_NS.obtenirEtat() == FeuEtat::Orange)
            feuNS.setFillColor(sf::Color(255, 165, 0)); // Couleur orange
        else
            feuNS.setFillColor(sf::Color::Red);

        sf::CircleShape feuEO(10);
        feuEO.setPosition(feu_EO.position);
        if (feu_EO.obtenirEtat() == FeuEtat::Vert)
            feuEO.setFillColor(sf::Color::Green);
        else if (feu_EO.obtenirEtat() == FeuEtat::Orange)
            feuEO.setFillColor(sf::Color(255, 165, 0)); // Couleur orange
        else
            feuEO.setFillColor(sf::Color::Red);

        window.draw(feuNS);
        window.draw(feuEO);

        // Dessiner les usagers
        for (auto& usager : usagers) {
            window.draw(usager->getShape());
        }

        window.display();
    }

    // Attendre la fin des threads
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}


