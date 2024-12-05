#include "feuVoiture.cpp"


enum class PlaqueEtat { Ralentisseur, Stop, TournerDG, TournerD, TournerG, CommencerTurn };
enum class PlaqueDeg { Droite, Gauche, Haut, Bas };
enum class Orientation { Turn, Gauche, Droite, GaucheDroite };

class Plaque {
private:
    RectangleShape plaque;
    FeuCirculation* feuAssocie; // Feu associé à cette plaque
    PlaqueEtat myEtat;
    
public:
    Plaque(float x, float y, FeuCirculation* feu, PlaqueEtat p_etat, float tailleX, float tailleY, PlaqueDeg orientation)
        : feuAssocie(feu), myEtat(p_etat) {
        plaque.setSize(Vector2f(tailleX, tailleY));
        plaque.setFillColor(Color(200, 200, 200, 100));
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

class PlaqueOrientation {
private:
    sf::RectangleShape plaque;
    Orientation valeur;
    bool voitureTouchePlaque = false; // Pour éviter de tourner plusieurs fois tant que la voiture est sur la plaque
    int directionX, directionY;
    bool mustTurn;
public:
    // Constructeur
    PlaqueOrientation(float x, float y, float tailleX, float tailleY, Orientation orientation, int dirX, int dirY, bool mustTurn_ = false)
        : valeur(orientation), directionX(dirX), directionY(dirY), mustTurn(mustTurn_){
        plaque.setSize(sf::Vector2f(tailleX, tailleY));
        plaque.setFillColor(sf::Color(150, 150, 200, 150)); // Couleur distinctive
        plaque.setPosition(x, y);
    }

    void dessiner(RenderWindow& window) const {
        window.draw(plaque);
    }

    sf::FloatRect getGlobalBounds() const {
        return plaque.getGlobalBounds();
    }

    Orientation getValeur() const {
        return valeur;
    }

    int getDirX() const {
        return directionX;
    }

    int getDirY() const {
        return directionY;
    }

    int getMustTurn() const {
        return mustTurn;
    }

};