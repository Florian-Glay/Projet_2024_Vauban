#include <iostream>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <memory>
#include <cmath> // Pour std::sqrt

using namespace std;
using namespace sf;

// NEW
#ifdef _MSC_VER
constexpr char _PATH_IMG_[] = "C:/Program Files/SFML/img/";
#else
constexpr char _PATH_IMG_[] = "img/";
#endif

const std::string path_image(_PATH_IMG_);
// END NEW

enum class FeuEtat { Rouge, Orange, Vert };

// Classe pour les feux de circulation
class FeuCirculation {
private:
    FeuEtat etat;
    mutable std::mutex mtx; // Le mutex doit être mutable pour être utilisé dans une méthode const

public:
    Vector2f position;
    bool waiting_time = false;
    FeuCirculation(Vector2f pos, FeuEtat f_etat = FeuEtat::Rouge) : etat(f_etat), position(pos) {}

    void changerEtat() {
        std::lock_guard<std::mutex> lock(mtx);
        etat = (etat == FeuEtat::Rouge) ? FeuEtat::Vert :
            (etat == FeuEtat::Vert) ? FeuEtat::Orange : FeuEtat::Rouge;
    }

    FeuEtat obtenirEtat() const { // La méthode est maintenant const
        std::lock_guard<std::mutex> lock(mtx);
        return etat;
    }

    static void controleFeux(vector<FeuCirculation*>& feux) {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            for (auto feu : feux) {
                if (feu->obtenirEtat() == FeuEtat::Vert) { // Changer l'état uniquement si le feu est vert
                    feu->changerEtat();
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
            for (auto feu : feux) {
                if (feu->obtenirEtat() == FeuEtat::Orange) { // Changer l'état uniquement si le feu est orange
                    feu->changerEtat();
                }
                else if (feu->obtenirEtat() == FeuEtat::Rouge) {
                    feu->waiting_time = true; // Laisser le feu en rouge
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            for (auto feu : feux) {
                if (feu->waiting_time == true) { // Changer l'état uniquement si le feu est rouge et qu'il doit passer vert
                    feu->changerEtat();
                    feu->waiting_time = false;
                }
            }
        }
    }

    static Color obtenirCouleurFeu(FeuEtat etat) {
        switch (etat) {
        case FeuEtat::Vert:
            return Color::Green;
        case FeuEtat::Orange:
            return Color(255, 165, 0); // Couleur orange
        case FeuEtat::Rouge:
        default:
            return Color::Red;
        }
    }

    void dessiner(RenderWindow& window) const { // Rendu const
        CircleShape feu(10);
        feu.setPosition(position);
        feu.setFillColor(obtenirCouleurFeu(obtenirEtat()));
        window.draw(feu);
    }
};


float calculerDistanceAuPointArret(const Vector2f& positionVoiture, const FeuCirculation& feu, int directionX, int directionY, bool& depasse) {
    // Calculer la position du point d'arrêt à 30 pixels avant le feu
    Vector2f pointArret = feu.position - Vector2f(directionX * 30, directionY * 30);
    // Calculer la distance entre la voiture et le point d'arrêt
    float deltaX = positionVoiture.x - pointArret.x;
    float deltaY = positionVoiture.y - pointArret.y;
    if (((directionX > 0 && deltaX < 1) ||
            (directionX < 0 && deltaX > 1) ||
            (directionY > 0 && deltaY < 1) ||
            (directionY < 0 && deltaY > 1))) {
        depasse = true; // Le véhicule ne sera plus impacté par le feu
    }
    else {
        depasse = false;
    }

    return std::sqrt(deltaX * deltaX + deltaY * deltaY); // Distance Euclidienne
}



// Classe de base pour un usager
class Usager {
protected:
    RectangleShape shape;
    int vitesse;
    FeuCirculation& feu;
    int directionX, directionY;
    Vector2f limiteArret;
    bool feuDepasse;

public:
    Usager(int pos_x, int pos_y, int vitesse_, FeuCirculation& feu_, Color color, int dirX, int dirY)
        : vitesse(vitesse_), feu(feu_), directionX(dirX), directionY(dirY), feuDepasse(false) {
        shape.setSize(Vector2f(20, 20));
        shape.setPosition(pos_x, pos_y);
        shape.setFillColor(color);

        limiteArret = feu.position - Vector2f(dirX * 30, dirY * 30); // Calcul de la limite d'arrêt
    }

    virtual void deplacer() {

        FeuEtat myLight = feu.obtenirEtat();
        float coeffV = 0.0;
        while (true) {
            Vector2f pos = shape.getPosition();

            // Vérifier si le véhicule a dépassé le feu
            if (!feuDepasse &&
                ((directionX > 0 && pos.x - 20 >= feu.position.x) ||
                    (directionX < 0 && pos.x + 20 <= feu.position.x) ||
                    (directionY > 0 && pos.y - 20 >= feu.position.y) ||
                    (directionY < 0 && pos.y + 20 <= feu.position.y))) {
                feuDepasse = true; // Le véhicule ne sera plus impacté par le feu
            }

            // Si le feu est rouge ou orange et que le véhicule n'a pas dépassé, il s'arrête avant le feu
            if (!feuDepasse && (feu.obtenirEtat() == FeuEtat::Rouge) &&
                ((directionX != 0 && ((directionX > 0 && pos.x + 20 >= limiteArret.x - 50) ||
                    (directionX < 0 && pos.x <= limiteArret.x + 50))) ||
                    (directionY != 0 && ((directionY > 0 && pos.y + 20 >= limiteArret.y - 50) ||
                        (directionY < 0 && pos.y <= limiteArret.y + 30))))) {
                if (coeffV <= 0.7) {
                    coeffV = coeffV * 0.99;
                }
                else {
                    coeffV = 0.7;
                }

                if (!feuDepasse && (feu.obtenirEtat() == FeuEtat::Rouge) &&
                    ((directionX != 0 && ((directionX > 0 && pos.x + 20 >= limiteArret.x) ||
                        (directionX < 0 && pos.x <= limiteArret.x))) ||
                        (directionY != 0 && ((directionY > 0 && pos.y + 20 >= limiteArret.y) ||
                            (directionY < 0 && pos.y <= limiteArret.y - 20))))) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    coeffV = 0.0;
                    continue;
                }
            }
            else if (!feuDepasse && (feu.obtenirEtat() == FeuEtat::Orange && myLight != feu.obtenirEtat()) &&
                ((directionX != 0 && ((directionX > 0 && pos.x + 20 >= limiteArret.x - 50) ||
                    (directionX < 0 && pos.x <= limiteArret.x + 70))) ||
                    (directionY != 0 && ((directionY > 0 && pos.y + 20 >= limiteArret.y - 50) ||
                        (directionY < 0 && pos.y <= limiteArret.y + 50))))) {
                feuDepasse = true;
                myLight = feu.obtenirEtat();
            }
            else if (!feuDepasse && (feu.obtenirEtat() == FeuEtat::Orange) &&
                ((directionX != 0 && ((directionX > 0 && pos.x + 20 >= limiteArret.x - 60) ||
                    (directionX < 0 && pos.x <= limiteArret.x + 60))) ||
                    (directionY != 0 && ((directionY > 0 && pos.y + 20 >= limiteArret.y - 60) ||
                        (directionY < 0 && pos.y <= limiteArret.y + 40))))) {
                if (coeffV <= 0.7) {
                    coeffV = coeffV * 0.99;
                }
                else {
                    coeffV = 0.5;
                }

                myLight = feu.obtenirEtat();
                if (!feuDepasse && (feu.obtenirEtat() == FeuEtat::Orange) &&
                    ((directionX != 0 && ((directionX > 0 && pos.x + 20 >= limiteArret.x) ||
                        (directionX < 0 && pos.x <= limiteArret.x))) ||
                        (directionY != 0 && ((directionY > 0 && pos.y + 20 >= limiteArret.y) ||
                            (directionY < 0 && pos.y <= limiteArret.y))))) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    coeffV = 0.0;
                    continue;
                }

            }
            else {

                coeffV = (coeffV > 0.9) ? 1.0 : ((coeffV < 0.2) ? (coeffV + 0.01) : (coeffV * 1.01)); // Accélération progressive
                myLight = feu.obtenirEtat();
            }

            // Déplacement normal
            shape.move(vitesse * directionX * coeffV, vitesse * directionY * coeffV);


            // Si le véhicule dépasse les limites de la fenêtre, il revient au point de départ
            if (pos.x < -20 || pos.x > 900 || pos.y < -20 || pos.y > 900) {
                resetPosition();
                feuDepasse = false; // Réinitialiser l'état au point de départ
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    RectangleShape& getShape() { return shape; }

    virtual void resetPosition() {
        if (directionX > 0)
            shape.setPosition(0, shape.getPosition().y);
        else if (directionX < 0)
            shape.setPosition(864, shape.getPosition().y);
        else if (directionY > 0)
            shape.setPosition(shape.getPosition().x, 0);
        else if (directionY < 0)
            shape.setPosition(shape.getPosition().x, 864);
    }
};


int main() {
    // Initialisation des feux
    FeuCirculation feu_NS(Vector2f(240, 170), FeuEtat::Vert);
    FeuCirculation feu_SN(Vector2f(620, 680), FeuEtat::Vert);
    FeuCirculation feu_EO(Vector2f(152, 438));
    FeuCirculation feu_OE(Vector2f(705, 420));

    vector<FeuCirculation*> feux = { &feu_NS, &feu_SN, &feu_EO, &feu_OE };

    // Passer une référence au vecteur feux via std::ref
    thread controleThread(&FeuCirculation::controleFeux, std::ref(feux)); // Correction ici

    // Création des usagers
    vector<unique_ptr<Usager>> usagers;
    usagers.emplace_back(make_unique<Usager>(360, 0, 4, feu_NS, Color::Blue, 0, 1));
    usagers.emplace_back(make_unique<Usager>(0, 478, 2, feu_EO, Color::Green, 1, 0));
    usagers.emplace_back(make_unique<Usager>(485, 880, 4, feu_SN, Color::Red, 0, -1));
    usagers.emplace_back(make_unique<Usager>(880, 380, 2, feu_OE, Color::Yellow, -1, 0));

    vector<thread> threads;
    for (auto& usager : usagers) {
        threads.emplace_back(&Usager::deplacer, usager.get());
    }

    RenderWindow window(VideoMode(864, 864), "Simulation de Carrefour");

    Texture backgroundImage;
    Sprite backgroundSprite;
    if (!backgroundImage.loadFromFile(path_image + "map.png")) {
        cerr << "Erreur pendant le chargement des images" << endl;
        return EXIT_FAILURE;
    }
    backgroundSprite.setTexture(backgroundImage);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear(Color::White);
        window.draw(backgroundSprite);

        for (auto feu : feux) {
            feu->dessiner(window);
        }

        for (auto& usager : usagers) {
            window.draw(usager->getShape());
        }

        window.display();
    }

    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }

    controleThread.join();
    return 0;
}
