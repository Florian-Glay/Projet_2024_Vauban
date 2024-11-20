#include <iostream> 
#include <thread>   
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <mutex>

// NEW 15/11/24
#include <condition_variable>
#include <cstdlib>

using namespace std;
using namespace sf;

#ifdef _MSC_VER 
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define _PATH_IMG_ "C:/Program Files/SFML/img/"
#else
#define _PATH_IMG_ "img/"
#endif

const std::string path_image(_PATH_IMG_);
// END_NEW 15/11/24

enum class FeuEtat { Rouge, Vert };

class FeuCirculation {
private:
    FeuEtat etat;
    std::mutex mtx;
    std::condition_variable cv;

public:
    FeuCirculation() : etat(FeuEtat::Rouge) {}

    void changerEtat() {
        std::lock_guard<std::mutex> lock(mtx);
        etat = (etat == FeuEtat::Rouge) ? FeuEtat::Vert : FeuEtat::Rouge;
        cv.notify_all();
    }

    FeuEtat obtenirEtat() {
        std::lock_guard<std::mutex> lock(mtx);
        return etat;
    }

    void attendreVert() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return etat == FeuEtat::Vert; });
    }
};

class Usager {
protected:
    sf::RectangleShape shape;
    int vitesse;
    FeuCirculation& feu;

public:
    Usager(int pos_x, int pos_y, int vitesse_, FeuCirculation& feu_, sf::Color color)
        : vitesse(vitesse_), feu(feu_) {
        shape.setSize(sf::Vector2f(20, 20));
        shape.setPosition(pos_x, pos_y);
        shape.setFillColor(color);
    }

    virtual void deplacer() {
        while (true) {
            feu.attendreVert();  // Attendre que le feu soit vert pour avancer
            shape.move(vitesse, 0);  // Avance sur l'axe des x
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    sf::RectangleShape& getShape() { return shape; }
};

// Gestion des feux de chaque direction du carrefour
void controleFeux(FeuCirculation& feu_NS, FeuCirculation& feu_EO) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        feu_NS.changerEtat();
        feu_EO.changerEtat();
        std::cout << "Changement de feux! Nord-Sud : "
            << (feu_NS.obtenirEtat() == FeuEtat::Vert ? "Vert" : "Rouge")
            << ", Est-Ouest : "
            << (feu_EO.obtenirEtat() == FeuEtat::Vert ? "Vert" : "Rouge")
            << std::endl;
    }
}

int main() {
    // Feux de circulation pour les directions Nord-Sud et Est-Ouest
    FeuCirculation feu_NS;
    FeuCirculation feu_EO;

    // Usagers arrivant de chaque direction du carrefour
    Usager voiture_NS(400, 100, 5, feu_NS, sf::Color::Blue);
    Usager voiture_EO(100, 300, 5, feu_EO, sf::Color::Green);

    std::vector<std::thread> threads;
    threads.emplace_back(&Usager::deplacer, &voiture_NS);
    threads.emplace_back(&Usager::deplacer, &voiture_EO);
    threads.emplace_back(controleFeux, std::ref(feu_NS), std::ref(feu_EO));

    // Création de la fenêtre SFML pour afficher la simulation du carrefour
    sf::RenderWindow window(sf::VideoMode(900, 900), "Simulation de Carrefour");

    // NEW 15/11/24 : Fond d'écran
    Texture backgroundImage;
    Sprite backgroundSprite;
    //carSprite, runnerSprite; !carImage.loadFromFile(path_image + "car.png")

    if (!backgroundImage.loadFromFile(path_image + "map.png")) {
        cerr << "Erreur pendant le chargement des images" << endl;
        return EXIT_FAILURE; // On ferme le programme
    }

    backgroundSprite.setTexture(backgroundImage);
    // END_NEW 15/11/24

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Effacer la fenêtre pour redessiner les éléments
        window.clear(sf::Color::White);


        // Dessiner les routes
        window.draw(backgroundSprite);

        /*
        sf::RectangleShape routeH(sf::Vector2f(800, 50));
        routeH.setPosition(0, 275);
        routeH.setFillColor(sf::Color(128, 128, 128)); // Route horizontale

        sf::RectangleShape routeV(sf::Vector2f(50, 600));
        routeV.setPosition(375, 0);
        routeV.setFillColor(sf::Color(128, 128, 128)); // Route verticale

        window.draw(routeH);
        window.draw(routeV);
        */

        // Dessin des feux de circulation
        sf::CircleShape feuNS(10);
        feuNS.setPosition(390, 250);
        feuNS.setFillColor(feu_NS.obtenirEtat() == FeuEtat::Vert ? sf::Color::Green : sf::Color::Red);

        sf::CircleShape feuEO(10);
        feuEO.setPosition(350, 300);
        feuEO.setFillColor(feu_EO.obtenirEtat() == FeuEtat::Vert ? sf::Color::Green : sf::Color::Red);

        window.draw(feuNS);
        window.draw(feuEO);

        // Dessin des usagers
        window.draw(voiture_NS.getShape());
        window.draw(voiture_EO.getShape());

        // Afficher tous les éléments dessinés
        window.display();
    }

    // Attendre que les threads se terminent
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}
