#include "plaques.cpp"
#include <mutex>

// Structure pour représenter une position et une direction
struct PositionDirection {
    Vector2f position;
    int directionX;
    int directionY;
};

// Liste des positions possibles
const std::vector<PositionDirection> positions = {
    {{-100, 340}, 1, 0},   // Droite
    {{360, 1200}, 0, -1},  // Haut
    {{860, 1200}, 0, -1},  // Haut
    {{2000, 870}, -1, 0},  // Gauche
    {{795, -160}, 0, 1},   // Bas
    {{2000, 285}, -1, 0}   // Gauche
};

const std::vector<PositionDirection> positionsBus = {
    {{-100, 380}, 1, 0},   // Droite
    {{2000, 240}, -1, 0},  // Gauche
};


class Usager {
protected:
    bool hasTurn = false;
    Sprite sprite;
    Texture texture;
    Texture texturebus;
    int vitesse;
    int directionX, directionY;
    int nextDirectionX, nextDirectionY;
    bool isBus;
    float size2;
    bool collisionDetectee = false;
    const std::vector<std::shared_ptr<FeuCirculation>>& feuVehicules;
    RectangleShape hitbox; // Rectangle suivant la voiture
    bool aSupprimer = false; // Indique si la voiture doit être supprimée

public:
    Usager(int pos_x, int pos_y, int vitesse_, std::vector<std::shared_ptr<FeuCirculation>>& feux, float size, const std::string& image_path, int dirX, int dirY, bool bus)
        : vitesse(vitesse_), directionX(dirX), directionY(dirY), isBus(bus), feuVehicules(feux), nextDirectionX(0), nextDirectionY(0),size2(size){
        if (!texture.loadFromFile(image_path + "voiture_1.png")) {
            cerr << "Erreur lors du chargement de la texture" << endl;
        }
        if (!texturebus.loadFromFile(image_path + "bus_1.png")) {
            cerr << "Erreur lors du chargement de la texture" << endl;
        }

        if (isBus) {
            sprite.setTexture(texturebus);
            sf::FloatRect bounds = sprite.getGlobalBounds();
            sprite.setOrigin(bounds.width / 2, bounds.height / 2);
            sprite.setPosition(pos_x, pos_y);
            sprite.setScale(size2*0.8, size2*0.8);

            // Initialiser le rectangle (hitbox)
            hitbox.setSize(Vector2f(200.f, 60.f)); // Dimensions 100x80
            hitbox.setFillColor(Color(255, 0, 0, 100)); // Couleur semi-transparente pour la visualisation
            hitbox.setOrigin((hitbox.getSize().x / 2.f) - 200, hitbox.getSize().y / 2.f);
            hitbox.setPosition(pos_x, pos_y);
            hitbox.setScale(size2*0.8, size2*0.8);
        }
        else {
            sprite.setTexture(texture);
            sf::FloatRect bounds = sprite.getGlobalBounds();
            sprite.setOrigin(bounds.width / 2, bounds.height / 2);

            sprite.setPosition(pos_x, pos_y);
            sprite.setScale(size2, size2);

            // Initialiser le rectangle (hitbox)
            hitbox.setSize(Vector2f(100.f, 60.f)); // Dimensions 100x80
            hitbox.setFillColor(Color(255, 0, 0, 100)); // Couleur semi-transparente pour la visualisation
            hitbox.setOrigin((hitbox.getSize().x / 2.f) - 100, hitbox.getSize().y / 2.f);
            hitbox.setPosition(pos_x, pos_y);
            hitbox.setScale(size2, size2);
        }

		resetPosition();

    }

    virtual void deplacer(vector<std::shared_ptr<Plaque>>& plaques, vector<PlaqueOrientation>& plaquesOrientation, vector<Usager*>& voitures, float& timeSpeed, int& entityNum) {
        float coeffV = 1.0; // Coefficient de vitesse
        int plaque_touch = 1;
        hasTurn = false;
        bool touchTurn = false;
        sf::Clock lowSpeedClock; // Horloge pour mesurer le temps à faible vitesse
        Vector2f pos = sprite.getPosition();
        std::srand(time(NULL));

        while (true) {
            Vector2f pos = sprite.getPosition();
            int count = 0;
            bool stop_P = false;
            bool touching = false;
            float dist = -1.0;

            // Si coeffV est inférieur à 0.3, démarrer ou continuer à mesurer le temps
            if (coeffV < 0.3) {
                if (lowSpeedClock.getElapsedTime().asSeconds() >= 30.0 * timeSpeed) {
                    resetPosition(); // Réinitialiser la position si 10 secondes sont écoulées
                    lowSpeedClock.restart(); // Réinitialiser l'horloge
                    coeffV = 1.0; // Remettre la vitesse à 1
                    continue; // Passer à l'itération suivante
                    aSupprimer = true;
                }
            }
            else {
                lowSpeedClock.restart(); // Réinitialiser l'horloge si coeffV >= 0.3
            }

            // Détection de collision avec les plaques
            for (auto& plaque : plaques) {
                if (plaque->getGlobalBounds().intersects(sprite.getGlobalBounds())) {
                    touching = true;
                    FeuEtat etatFeu = plaque->obtenirEtatFeu();
                    PlaqueEtat etatP = plaque->obtenirEtatPlaque();
                    if (etatP == PlaqueEtat::Ralentisseur) {
                        if ((etatFeu == FeuEtat::Rouge || etatFeu == FeuEtat::Orange) && plaque_touch != 3) {
                            plaque_touch = 2; // Ralentissement progressive
                            //std::cout << coeffV << std::endl;
                        }
                        if (etatFeu == FeuEtat::Vert) {
                            plaque_touch = 1;
                            touching = false;
                        }
                    }
                    else if (etatP == PlaqueEtat::Stop) {
                        if (etatFeu == FeuEtat::Rouge || etatFeu == FeuEtat::Orange) {
                            plaque_touch = 3;
                            Vector2f plaqueBounds = plaque->getPosition();
                            // Calcul de la distance
                            
                            if (isBus) {
                                float dx = plaqueBounds.x - (pos.x + directionX * 45);
                                dist = sqrt(dx * dx);
                                if (dist < 5 && ((directionX > 0) ? (dx > 0) : (dx < 0))) {
                                    dist = 0;
                                }
                            }
                            else {
                                float dx = plaqueBounds.x - pos.x;
                                float dy = plaqueBounds.y - pos.y;
                                dist = sqrt(dx * dx + dy * dy) - 25;
                            }
                            
                            if (dist < 0) {
                                plaque_touch = 1;
                            }
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

            // Détection de collision avec les plaques d'orientation
            touchTurn = false;
            for (auto& plaqueOrientation : plaquesOrientation) {
                if (plaqueOrientation.getGlobalBounds().intersects(sprite.getGlobalBounds())) {
                    Orientation val = plaqueOrientation.getValeur();
                    if (!hasTurn && !touchTurn && val != Orientation::Turn) { // Empêche de tourner plusieurs fois sur la même plaque
                        if (plaqueOrientation.getMustTurn()) {
                            if (val == Orientation::GaucheDroite) {
                                touchTurn = true;
                                int choix = std::rand() % 2;
                                hasTurn = true;
                                if (choix == 1) {
                                    if (directionX > 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                    else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                    else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                    else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = 1; }
                                }
                                else {
                                    if (directionX > 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                    else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                    else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = 1; }
                                    else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                }
                            }
                            else {
                                touchTurn = true;
                                hasTurn = true;
                                if (val == Orientation::Gauche) {
                                    if (directionX > 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                    else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                    else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = 1; }
                                    else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                }
                                if (val == Orientation::Droite) {
                                    if (directionX > 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                    else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                    else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                    else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = 1; }
                                }
                            }

                        }
                        else {
                            touchTurn = true;
                            //std::srand(time(NULL));
                            int choix = std::rand() % 50;
                            if (choix == 1) {
                                hasTurn = true;
                                if (val == Orientation::Gauche) {
                                    if (directionX > 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                    else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                    else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = 1; }
                                    else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                }

                                if (val == Orientation::Droite) {
                                    if (directionX > 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                    else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                    else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                    else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = 1; }
                                }
                                if (val == Orientation::GaucheDroite) {
                                    touchTurn = true;
                                    int choix = std::rand() % 2;
                                    hasTurn = true;
                                    if (choix == 1) {
                                        if (directionX > 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                        else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                        else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                        else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = 1; }
                                    }
                                    else {
                                        if (directionX > 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                        else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                        else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = 1; }
                                        else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                    }
                                }
                            }
                            else {
                                hasTurn = false;
                                nextDirectionX = directionX;
                                nextDirectionY = directionY;
                            }
                        }
                    }
                }
            }

            if (hasTurn && !isBus) {
                for (auto& plaqueOrientation : plaquesOrientation) {
                    Orientation val = plaqueOrientation.getValeur();
                    if (val == Orientation::Turn && plaqueOrientation.getGlobalBounds().intersects(sprite.getGlobalBounds()) && plaqueOrientation.getDirX() == nextDirectionX && plaqueOrientation.getDirY() == nextDirectionY) {
                        directionY = nextDirectionY;
                        directionX = nextDirectionX;
                        coeffV = coeffV * 0.5;
                        hasTurn = false;
                    }
                }

            }



            // Détection de collision avec d'autres voitures
            mettreAJourHitbox();
            if (verifierCollision(voitures)) {
                //std::cout << "Collision détectée ! Arrêt de la voiture." << std::endl;
                coeffV = coeffV * 0.85 * timeSpeed; // Arrêter la voiture
                sprite.move((vitesse * directionX * coeffV * timeSpeed), (vitesse * directionY * coeffV * timeSpeed));
            }
            else {
                if (isBus) {
                    orienterSprite();
                    if (plaque_touch == 1) {
                        coeffV = (coeffV > 0.9) ? 1.0 : ((coeffV < 0.2) ? (coeffV + 0.01) : (coeffV * 1.01 / timeSpeed)); // Accélération progressive
                    }
                    else if (plaque_touch == 2) {
                        coeffV = (coeffV < 0.1) ? 0 : ((coeffV > 0.8) ? 0.6 : ((coeffV < 0.2) ? (coeffV - 0.01) : (coeffV * 0.99 * timeSpeed)));
                    }
                    else if (plaque_touch == 3) {
                        if (dist >= 0.0) {
                            if (coeffV < dist) {
                                dist = dist * 0.8 * timeSpeed;
                            }
                            else {
                                coeffV = (coeffV < 0.1) ? 0 : ((coeffV > 0.8) ? 0.6 : ((coeffV < 0.2) ? (coeffV - 0.01) : (coeffV * 0.9 * timeSpeed)));
                            }

                        }
                    }


                    // Déplacement normal
                    if (dist >= 0.0) {
                        sprite.move(static_cast<float>(vitesse * directionX * 0.01 * dist * int(round(1.0 / timeSpeed))), static_cast<float>(vitesse * directionY * 0.01 * dist * int(round(1.0 / timeSpeed))));
                    }
                    else {
                        sprite.move(static_cast<float>(vitesse * directionX * coeffV * int(round(1.0 / timeSpeed))), static_cast<float>(vitesse * directionY * coeffV * int(round(1.0 / timeSpeed))));
                    }
                }
                else {
                    
                    if (plaque_touch == 1) {
                        coeffV = (coeffV > 0.9) ? 1.0 : ((coeffV < 0.2) ? (coeffV + 0.01) : (coeffV * 1.01 / timeSpeed)); // Accélération progressive
                    }
                    else if (plaque_touch == 2) {
                        coeffV = (coeffV < 0.1) ? 0 : ((coeffV > 0.8) ? 0.6 : ((coeffV < 0.2) ? (coeffV - 0.01) : (coeffV * 0.99 * timeSpeed)));
                    }
                    else if (plaque_touch == 3) {
                        if (dist >= 0.0) {
                            if (coeffV < dist) {
                                dist = dist * 0.9 * timeSpeed;
                            }
                            else {
                                coeffV = (coeffV < 0.1) ? 0 : ((coeffV > 0.8) ? 0.6 : ((coeffV < 0.2) ? (coeffV - 0.01) : (coeffV * 0.99 * timeSpeed)));
                            }

                        }
                    }


                    // Déplacement normal
                    if (dist >= 0.0) {
                        sprite.move(static_cast<float>(vitesse * directionX * 0.01 * dist * int(round(1.0 / timeSpeed))), static_cast<float>(vitesse * directionY * 0.01 * dist * int(round(1.0 / timeSpeed))));
                    }
                    else {
                        sprite.move(static_cast<float>(vitesse * directionX * coeffV * int(round(1.0 / timeSpeed))), static_cast<float>(vitesse * directionY * coeffV * int(round(1.0 / timeSpeed))));
                    }
                }
            }

            if (pos.x < -300 || pos.x > 2200 || pos.y < -300 || pos.y > 1400) {
                resetPosition();
                aSupprimer = true; // Marquer pour suppression
            }

            orienterSprite();

            std::this_thread::sleep_for(std::chrono::milliseconds(int(round(10 * timeSpeed))));
        }
    }

    bool doitEtreSupprime() const {
        return aSupprimer;
    }

    Sprite& getSprite() { return sprite; }

    virtual void resetPosition() {
        // Générer un index aléatoire


        std::srand(std::time(NULL));
        
        if (isBus) {
            int index = std::rand() % (positionsBus.size());

            // Sélectionner une position et une direction aléatoires
            const auto& chosen = positionsBus[index];
            sprite.setPosition(chosen.position);
            directionX = chosen.directionX;
            directionY = chosen.directionY;
        }
        else {
            int index = std::rand() % (positions.size());

            // Sélectionner une position et une direction aléatoires
            const auto& chosen = positions[index];
            sprite.setPosition(chosen.position);
            directionX = chosen.directionX;
            directionY = chosen.directionY;
        }

        if (directionX > 0) { sprite.setRotation(0); }
        else if (directionX < 0) { sprite.setRotation(180); }
        else if (directionY > 0) { sprite.setRotation(90); }
        else if (directionY < 0) { sprite.setRotation(270); }

        hasTurn = false;
    }

    void dessiner(RenderWindow& window) const {
        // Dessiner la voiture
        window.draw(sprite);
        window.draw(hitbox);
    }

    FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    // Vérifier les collisions avec d'autres voitures
    bool verifierCollision(const vector<Usager*>& voitures) {
        for (const auto voiture : voitures) {
            if (voiture == nullptr) continue; // Vérifiez que le pointeur est valide
            if (voiture != this && hitbox.getGlobalBounds().intersects(voiture->sprite.getGlobalBounds())) {
                return true; // Collision détectée
            }
        }
        return false; // Pas de collision
    }



private:

    void mettreAJourHitbox() {
        // Synchroniser la position et la rotation de la hitbox avec le sprite
        hitbox.setPosition(sprite.getPosition());
        hitbox.setRotation(sprite.getRotation());
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
        }
        else {
            // Sinon, ajuster progressivement
            sprite.setRotation(currentAngle + rotationStep * (angleDifference > 0 ? 1 : -1));
        }
    }

};