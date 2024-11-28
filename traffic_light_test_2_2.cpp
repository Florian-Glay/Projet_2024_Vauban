#include <iostream>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <memory>
#include <cmath>
#include <random>
#include <string>

using namespace std;
using namespace sf;

#ifdef _MSC_VER
constexpr char _PATH_IMG_[] = "C:/Program Files/SFML/img/";
#else
constexpr char _PATH_IMG_[] = "img/";
#endif

const std::string path_image(_PATH_IMG_);

enum class FeuEtat { Rouge, Orange, Vert };
enum class PlaqueEtat { Ralentisseur, Stop, TournerDG, TournerD, TournerG, CommencerTurn };
enum class PlaqueDeg { Droite, Gauche, Haut, Bas };

class FeuCirculation {
private:
    FeuEtat etat;
    mutable std::mutex mtx;

public:
    Vector2f position;
    bool waiting_time = false;
    FeuCirculation(Vector2f pos, FeuEtat f_etat = FeuEtat::Rouge) : etat(f_etat), position(pos) {}

    void changerEtat() {
        std::lock_guard<std::mutex> lock(mtx);
        etat = (etat == FeuEtat::Rouge) ? FeuEtat::Vert :
            (etat == FeuEtat::Vert) ? FeuEtat::Orange : FeuEtat::Rouge;
    }

    FeuEtat obtenirEtat() const {
        std::lock_guard<std::mutex> lock(mtx);
        return etat;
    }

    static void controleFeux(vector<FeuCirculation*>& feux) {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            for (auto feu : feux) {
                if (feu->obtenirEtat() == FeuEtat::Vert) {
                    feu->changerEtat();
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
            for (auto feu : feux) {
                if (feu->obtenirEtat() == FeuEtat::Orange) {
                    feu->changerEtat();
                }
                else if (feu->obtenirEtat() == FeuEtat::Rouge) {
                    feu->waiting_time = true;
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            for (auto feu : feux) {
                if (feu->waiting_time) {
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
            return Color(255, 165, 0);
        case FeuEtat::Rouge:
        default:
            return Color::Red;
        }
    }

    void dessiner(RenderWindow& window) const {
        CircleShape feu(10);
        feu.setPosition(position);
        feu.setFillColor(obtenirCouleurFeu(obtenirEtat()));
        window.draw(feu);
    }
};

class Plaque {
private:
    RectangleShape plaque;
    FeuCirculation* feuAssocie; // Feu associé à cette plaque
    PlaqueEtat myEtat;

public:
    Plaque(float x, float y, FeuCirculation* feu, PlaqueEtat p_etat, float tailleX, float tailleY, PlaqueDeg orientation)
        : feuAssocie(feu), myEtat(p_etat) {
        plaque.setSize(Vector2f(tailleX, tailleY));
        plaque.setFillColor(Color(200, 200, 200, 150));
        plaque.setPosition(x, y);
        if (orientation == PlaqueDeg::Droite) {
            plaque.setOrigin(plaque.getGlobalBounds().width, plaque.getGlobalBounds().height / 2);
        }
        else if (orientation == PlaqueDeg::Gauche) {
            plaque.setOrigin(0, plaque.getGlobalBounds().height / 2);
        }
        else if (orientation == PlaqueDeg::Haut) {
            plaque.setOrigin(plaque.getGlobalBounds().width / 2, plaque.getGlobalBounds().height);
        }
        else if (orientation == PlaqueDeg::Bas) {
            plaque.setOrigin(plaque.getGlobalBounds().width / 2, 0);
        }
    }

    Vector2f getPosition() const {
        return plaque.getPosition();
    }

    void dessiner(RenderWindow& window) const {
        window.draw(plaque);
    }

    FeuEtat obtenirEtatFeu() const {
        return feuAssocie->obtenirEtat();
    }

    PlaqueEtat obtenirEtatPlaque() const {
        return myEtat;
    }

    FloatRect getGlobalBounds() const {
        return plaque.getGlobalBounds();
    }
};


class Usager {
protected:
    bool hasTurn = false;
    Sprite sprite;
    Texture texture;
    int vitesse;
    int directionX, directionY;
    Vector2f limiteArret;
    bool feuDepasse;
    bool isBus;
    bool collisionDetectee = false;
    int croisements[4][2] = { {410, 414}, {410, 450}, {460, 414}, {460, 450} };
    FeuCirculation** feuVehicules; // Tableau de pointeurs vers les feux
    FeuCirculation* feu;

public:
    Usager(int pos_x, int pos_y, int vitesse_, FeuCirculation* tab[], float size, const std::string& image_path, int dirX, int dirY, bool bus)
        : vitesse(vitesse_), directionX(dirX), directionY(dirY), feuDepasse(false), isBus(bus), feuVehicules(tab), feu(tab[0]) {
        if (!texture.loadFromFile(image_path)) {
            cerr << "Erreur lors du chargement de la texture" << endl;
        }
        sprite.setTexture(texture);
        sf::FloatRect bounds = sprite.getGlobalBounds();
        sprite.setOrigin(bounds.width/2, bounds.height / 2);

        sprite.setPosition(pos_x, pos_y);
        resetPosition();
        sprite.setScale(size, size);
    }

    virtual void deplacer(vector<Plaque>& plaques) {
        float coeffV = 1.0; // Coefficient de vitesse
        int plaque_touch = 1;
       

        while (true) {
            Vector2f pos = sprite.getPosition();
            int count = 0;
            bool stop_P = false;
            bool touching = false;
            float dist = -1.0;
            // Détection de collision avec les plaques
            for (auto& plaque : plaques) {
                if (plaque.getGlobalBounds().intersects(sprite.getGlobalBounds())) {
                    touching = true;
                    FeuEtat etatFeu = plaque.obtenirEtatFeu();
                    PlaqueEtat etatP = plaque.obtenirEtatPlaque();
                    if (etatP == PlaqueEtat::Ralentisseur) {
                        if ((etatFeu == FeuEtat::Rouge || etatFeu == FeuEtat::Orange) && plaque_touch != 3) {
                            plaque_touch = 2; // Ralentissement progressive
                            //std::cout << coeffV << std::endl;
                        }
                        if (etatFeu == FeuEtat::Vert){
                            plaque_touch = 1;
                            touching = false;
                        }
                    }
                    else if (etatP == PlaqueEtat::Stop) {
                         if (etatFeu == FeuEtat::Rouge || etatFeu == FeuEtat::Orange) {
                             plaque_touch = 3;
                             Vector2f plaqueBounds = plaque.getPosition();
                             // Calcul de la distance
                             float dx = plaqueBounds.x - pos.x;
                             float dy = plaqueBounds.y - pos.y;
                             dist = sqrt(dx * dx + dy * dy) - 25;
                             // Ralentissement progressive
                            //std::cout << coeffV << std::endl;
                         }
                         if (etatFeu == FeuEtat::Vert) {
                             plaque_touch = 1;
                             touching = false;
                         }
                    }
                }

                if ((count > plaques.size() - 1) && !touching) { //plaque == plaques.end() && !touching
                    plaque_touch = 1;
                }
                count++;
            }

            if (plaque_touch == 1) {
                coeffV = (coeffV > 0.9) ? 1.0 : ((coeffV < 0.2) ? (coeffV + 0.01) : (coeffV * 1.01)); // Accélération progressive
            }
            else if(plaque_touch == 2){
                coeffV = (coeffV < 0.1) ? 0 : ((coeffV > 0.8) ? 0.6 : ((coeffV < 0.2) ? (coeffV - 0.01) : (coeffV * 0.99)));
            }
            else if (plaque_touch == 3) {
                if (dist >= 0.0) {
                    if (coeffV < dist) {
                        dist = dist * 0.9;
                    }
                    else {
                        coeffV = (coeffV < 0.1) ? 0 : ((coeffV > 0.8) ? 0.6 : ((coeffV < 0.2) ? (coeffV - 0.01) : (coeffV * 0.99)));
                    }
                    
                }
            }
            

            // Déplacement normal
            if (dist >= 0.0) {
                sprite.move(vitesse * directionX * 0.01 * dist, vitesse * directionY * 0.01 * dist);
            }
            else {
                sprite.move(vitesse * directionX * coeffV, vitesse * directionY * coeffV);
            }


            // Si le véhicule dépasse les limites de la fenêtre, il revient au point de départ
            if (pos.x < -160 || pos.x > 1000 || pos.y < -160 || pos.y > 1000) {
                resetPosition();
                feuDepasse = false; // Réinitialiser l'état au point de départ
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    Sprite& getSprite() { return sprite; }

    virtual void resetPosition() {
        if (isBus) {
            if (directionX > 0)
                sprite.setPosition(-150, 525);
            else if (directionX < 0)
                sprite.setPosition(1000, 335);
        }
        else {
            if (directionX > 0)
                sprite.setPosition(-150, 490);
            else if (directionX < 0)
                sprite.setPosition(990, 370);
            else if (directionY > 0)
                sprite.setPosition(370, -150);
            else if (directionY < 0)
                sprite.setPosition(500, 990);
        }

        if (directionX > 0) { sprite.setRotation(0); }
        else if (directionX < 0) { sprite.setRotation(180); }
        else if (directionY > 0) { sprite.setRotation(90); }
        else if (directionY < 0) { sprite.setRotation(270); }

        orienterSpriteFeu(feuVehicules); // Réorienter après réinitialisation
        hasTurn = false;

    }

private:

    void orienterSpriteFeu(FeuCirculation* tab[]) {
        if (directionX > 0) {
            feu = tab[2];
        }
        else if (directionX < 0) {
            feu = tab[3];
        }
        else if (directionY > 0) {
            feu = tab[0];
        }
        else if (directionY < 0) {
            feu = tab[1];
        }
        limiteArret = feu->position - Vector2f(directionX * 30, directionY * 30);
    }

    void orienterSprite() {
        static const float rotationSpeed = 18.0f; // Vitesse de rotation en degrés par seconde
        float targetAngle = sprite.getRotation(); // Angle actuel du sprite

        // Déterminer l'angle cible en fonction de la direction
        if (directionX > 0) { targetAngle = 0; }
        else if (directionX < 0) { targetAngle = 180; }
        else if (directionY > 0) { targetAngle = 90; }
        else if (directionY < 0) { targetAngle = 270; }

        // Obtenir l'angle actuel
        float currentAngle = sprite.getRotation();

        // Gérer les cas où l'angle dépasse 360 ou devient négatif
        while (currentAngle < 0) currentAngle += 360;
        while (currentAngle >= 360) currentAngle -= 360;

        // Calculer la différence d'angle en considérant la continuité angulaire
        float angleDifference = targetAngle - currentAngle;
        if (angleDifference > 180) angleDifference -= 360;
        if (angleDifference < -180) angleDifference += 360;

        // Calculer la rotation à appliquer en fonction de la vitesse et du temps écoulé
        float rotationStep = rotationSpeed * 0.4;
        if (std::abs(angleDifference) < rotationStep) {
            // Si la différence est petite, ajuster directement
            sprite.setRotation(targetAngle);
            hasTurn = false;
        }
        else {
            // Sinon, ajuster progressivement
            sprite.setRotation(currentAngle + rotationStep * (angleDifference > 0 ? 1 : -1));
        }
    }

};

int main() {
    FeuCirculation feu_NS(Vector2f(240, 170), FeuEtat::Vert);
    FeuCirculation feu_SN(Vector2f(620, 680), FeuEtat::Vert);
    FeuCirculation feu_EO(Vector2f(152, 438));
    FeuCirculation feu_OE(Vector2f(705, 420));
    FeuCirculation* feu_vehicule[4] = { &feu_NS, &feu_SN, &feu_EO, &feu_OE };
    vector<FeuCirculation*> feux = { &feu_NS, &feu_SN, &feu_EO, &feu_OE };
    thread controleThread(&FeuCirculation::controleFeux, std::ref(feux));
    vector<Plaque> plaques;
    plaques.emplace_back(100, 490, &feu_EO, PlaqueEtat::Ralentisseur, 70, 10, PlaqueDeg::Droite);
    plaques.emplace_back(150, 490, &feu_EO, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite);

    plaques.emplace_back(370, 100, &feu_NS, PlaqueEtat::Ralentisseur, 10, 70, PlaqueDeg::Haut);
    plaques.emplace_back(370, 150, &feu_NS, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Haut);
    
    plaques.emplace_back((864-100), 370, &feu_EO, PlaqueEtat::Ralentisseur, 70, 10, PlaqueDeg::Gauche);
    plaques.emplace_back((864-150), 370, &feu_EO, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche);

    plaques.emplace_back(500, (864-100), &feu_NS, PlaqueEtat::Ralentisseur, 10, 70, PlaqueDeg::Bas);
    plaques.emplace_back(500, (864-150), &feu_NS, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas);


    vector<unique_ptr<Usager>> usagers;
    usagers.emplace_back(make_unique<Usager>(370, 0, 4, feu_vehicule, 0.8, path_image + "voiture_1.png", 0, 1, false));
    usagers.emplace_back(make_unique<Usager>(420, 880, 4, feu_vehicule, 0.8, path_image + "voiture_1.png", 0, -1, false));

    usagers.emplace_back(make_unique<Usager>(0, 490, 4, feu_vehicule, 0.8, path_image + "voiture_1.png", 1, 0, false));
    usagers.emplace_back(make_unique<Usager>(880, 414, 4, feu_vehicule, 0.8, path_image + "voiture_1.png", -1, 0, false));

    // Ajout des threads pour déplacer les usagers
    vector<thread> threads;
    for (auto& usager : usagers) {
        threads.emplace_back(&Usager::deplacer, usager.get(), std::ref(plaques));
    }

    // Résolution cible pour le contenu
    const int targetWidth = 864;  // Largeur du contenu
    const int targetHeight = 864; // Hauteur du contenu
    float targetAspectRatio = static_cast<float>(targetWidth) / targetHeight;

    // Création de la fenêtre
    RenderWindow window(VideoMode(targetWidth, targetHeight), "Redimensionnement avec Ratio");

    // Création de la vue par défaut
    View view(FloatRect(0, 0, targetWidth, targetHeight));
    window.setView(view);


    // Création de la boîte en bas à droite
    RectangleShape box(Vector2f(100, 50));
    box.setFillColor(Color(200, 200, 200, 255)); // Couleur gris clair
    box.setOutlineColor(Color::Black);          // Bordure noire
    box.setOutlineThickness(2);
    box.setPosition(window.getSize().x - 110, window.getSize().y - 60); // En bas à droite

    // Chargement de la police
    Font font;
    if (!font.loadFromFile(path_image + "arial.ttf")) {
        cerr << "Erreur lors du chargement de la police" << endl;
        return EXIT_FAILURE;
    }

    // Création du texte pour afficher les coordonnées
    Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(Color::Black);
    text.setPosition(window.getSize().x - 105, window.getSize().y - 55); // À l'intérieur de la boîte


    Texture backgroundImage;
    Sprite backgroundSprite;
    if (!backgroundImage.loadFromFile(path_image + "map.png")) {
        cerr << "Erreur pendant le chargement des images" << endl;
        return EXIT_FAILURE;
    }
    backgroundSprite.setTexture(backgroundImage);
    sf::FloatRect bounds = backgroundSprite.getLocalBounds();
    backgroundSprite.setOrigin(bounds.width / 2, bounds.height / 2);
    backgroundSprite.setPosition(window.getSize().x / 2, window.getSize().y / 2);
    backgroundSprite.setScale(1, 1);
    // Définir la transparence à 50 %
    sf::Color transparentColor = backgroundSprite.getColor(); // Récupère la couleur actuelle
    transparentColor.a = 255; // 128 correspond à 50 % de transparence (255/2)
    backgroundSprite.setColor(transparentColor);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            // Détecter le redimensionnement de la fenêtre
            if (event.type == Event::Resized) {
                // Nouvelle taille de la fenêtre
                int screenWidth = event.size.width;
                int screenHeight = event.size.height;
                float screenAspectRatio = static_cast<float>(screenWidth) / screenHeight;

                // Ajuster la vue pour conserver le ratio
                if (screenAspectRatio > targetAspectRatio) {
                    // Écran plus large que le contenu -> bandes noires verticales
                    float viewportWidth = targetAspectRatio / screenAspectRatio;
                    view.setViewport(FloatRect((1.0f - viewportWidth) / 2.0f, 0, viewportWidth, 1.0f));
                }
                else {
                    // Écran plus haut que le contenu -> bandes noires horizontales
                    float viewportHeight = screenAspectRatio / targetAspectRatio;
                    view.setViewport(FloatRect(0, (1.0f - viewportHeight) / 2.0f, 1.0f, viewportHeight));
                }

                // Mettre à jour la vue de la fenêtre
                window.setView(view);
            }
        }

        window.clear(Color::Black);
        window.draw(backgroundSprite);

        for (auto feu : feux) {
            feu->dessiner(window);
        }

        for (auto& usager : usagers) {
            window.draw(usager->getSprite());
        }

        // Dessiner les plaques
        for (const auto& plaque : plaques) {
            plaque.dessiner(window);
        }

        // Récupération des coordonnées de la souris
        Vector2i mousePos = Mouse::getPosition(window);
        text.setString("X: " + to_string(mousePos.x) + "\nY: " + to_string(mousePos.y));

        // Affichage
        window.draw(box);  // Dessiner la boîte
        window.draw(text); // Dessiner le texte
        window.display();
    }

    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }

    controleThread.join();
    return 0;
}