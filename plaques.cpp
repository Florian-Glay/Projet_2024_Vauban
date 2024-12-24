#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "feuVoiture.cpp"

using namespace sf;
using namespace std;

enum class PlaqueEtat { Ralentisseur, Stop, TournerDG, TournerD, TournerG, CommencerTurn, Stop_P};
enum class PlaqueDeg { Droite, Gauche, Haut, Bas };
enum class Orientation { Turn, Gauche, Droite, GaucheDroite };

class Plaque {
private:
    RectangleShape plaque;
    std::shared_ptr<FeuCirculation> feuAssocie; // Feu associé à cette plaque
    PlaqueEtat myEtat;
	PlaqueDeg orientation2;

public:
    Plaque(int x, int y, std::shared_ptr<FeuCirculation> feu, PlaqueEtat p_etat, int tailleX, int tailleY, PlaqueDeg orientation)
        : feuAssocie(feu), myEtat(p_etat), orientation2(orientation) {
        plaque.setSize(Vector2f(tailleX, tailleY));
		plaque.setFillColor(Color(200, 50, 200, 0)); // Couleur distinctive !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Transparence
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

	PlaqueDeg obtenirOrientation() const {
		return orientation2;
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
    RectangleShape plaque;
    Orientation valeur;
    bool voitureTouchePlaque = false; // Évite les actions répétées sur la plaque
    int directionX, directionY;
    bool mustTurn;

public:
    PlaqueOrientation(int x, int y, int tailleX, int tailleY, Orientation orientation, int dirX, int dirY, bool mustTurn_ = false)
        : valeur(orientation), directionX(dirX), directionY(dirY), mustTurn(mustTurn_) {
        plaque.setSize(Vector2f(tailleX, tailleY));
        plaque.setFillColor(Color(150, 50, 200, 0));  // Couleur distinctive !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Transparence
        plaque.setPosition(x, y);
    }

    void dessiner(RenderWindow& window) const {
        window.draw(plaque);
    }

    FloatRect getGlobalBounds() const {
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
