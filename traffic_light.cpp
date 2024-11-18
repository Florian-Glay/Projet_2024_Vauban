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
    sf::Sprite sprite;
    sf::Texture texture;
    int vitesse;
    FeuCirculation& feu;

public:
    Usager(const std::string& imagePath, int pos_x, int pos_y, int vitesse_, FeuCirculation& feu_)
        : vitesse(vitesse_), feu(feu_) {
        if (!texture.loadFromFile(imagePath)) {
            cerr << "Erreur lors du chargement de l'image de la voiture" << endl;
            exit(EXIT_FAILURE);
        }
        sprite.setTexture(texture);
        sprite.setPosition(pos_x, pos_y);
    }

    virtual void deplacer() {
        while (true) {
            feu.attendreVert();  // Attendre que le feu soit vert pour avancer
            sprite.move(vitesse, 0);  // Avance sur l'axe des x
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    sf::Sprite& getSprite() { return sprite; }
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

    // Usagers avec images
    Usager voiture_NS(path_image + "voiture_bleue.png", 400, 100, 5, feu_NS);
    Usager voiture_EO(path_image + "voiture_verte.png", 100, 300, 5, feu_EO);

    std::vector<std::thread> threads;
    threads.emplace_back(&Usager::deplacer, &voiture_NS);
    threads.emplace_back(&Usager::deplacer, &voiture_EO);
    threads.emplace_back(controleFeux, std::ref(feu_NS), std::ref(feu_EO));

    // Création de la fenêtre SFML pour afficher la simulation du carrefour
    sf::RenderWindow window(sf::VideoMode(1280, 800), "Simulation de Carrefour");

    // Fond d'écran
    Texture backgroundImage;
    Sprite backgroundSprite;

    if (!backgroundImage.loadFromFile(path_image + "map.png")) {
        cerr << "Erreur pendant le chargement de l'arrière-plan" << endl;
        return EXIT_FAILURE; // On ferme le programme
    }

    backgroundSprite.setTexture(backgroundImage);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Effacer la fenêtre pour redessiner les éléments
        window.clear(sf::Color::White);

        // Dessiner le fond d'écran
        window.draw(backgroundSprite);

        // Dessin des feux de circulation
        sf::CircleShape feuNS(10);
        feuNS.setPosition(390, 250);
        feuNS.setFillColor(feu_NS.obtenirEtat() == FeuEtat::Vert ? sf::Color::Green : sf::Color::Red);

        sf::CircleShape feuEO(10);
        feuEO.setPosition(350, 300);
        feuEO.setFillColor(feu_EO.obtenirEtat() == FeuEtat::Vert ? sf::Color::Green : sf::Color::Red);

        window.draw(feuNS);
        window.draw(feuEO);

        // Dessin des voitures
        window.draw(voiture_NS.getSprite());
        window.draw(voiture_EO.getSprite());

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


