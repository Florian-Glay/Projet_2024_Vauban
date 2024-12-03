#include "plaques.cpp"

class Usager {
protected:
    bool hasTurn = false;
    Sprite sprite;
    Texture texture;
    int vitesse;
    int directionX, directionY;
    int nextDirectionX, nextDirectionY;
    Vector2f limiteArret;
    bool isBus;
    bool collisionDetectee = false;
    int croisements[4][2] = { {410, 414}, {410, 450}, {460, 414}, {460, 450} };
    FeuCirculation** feuVehicules; // Tableau de pointeurs vers les feux
    FeuCirculation* feu;
    RectangleShape hitbox; // Rectangle suivant la voiture

public:
    Usager(int pos_x, int pos_y, int vitesse_, FeuCirculation* tab[], float size, const std::string& image_path, int dirX, int dirY, bool bus)
        : vitesse(vitesse_), directionX(dirX), directionY(dirY), isBus(bus), feuVehicules(tab), feu(tab[0]), nextDirectionX(0), nextDirectionY(0) {
        if (!texture.loadFromFile(image_path)) {
            cerr << "Erreur lors du chargement de la texture" << endl;
        }
        sprite.setTexture(texture);
        sf::FloatRect bounds = sprite.getGlobalBounds();
        sprite.setOrigin(bounds.width / 2, bounds.height / 2);

        sprite.setPosition(pos_x, pos_y);
        resetPosition();
        sprite.setScale(size, size);

        // Initialiser le rectangle (hitbox)
        hitbox.setSize(Vector2f(100.f, 50.f)); // Dimensions 100x50
        hitbox.setFillColor(Color(255, 0, 0, 100)); // Couleur semi-transparente pour la visualisation
        hitbox.setOrigin((hitbox.getSize().x / 2.f) - 90, hitbox.getSize().y / 2.f);
        hitbox.setPosition(pos_x, pos_y);
        hitbox.setScale(size, size);
    }

    virtual void deplacer(vector<Plaque>& plaques, vector<PlaqueOrientation>& plaquesOrientation, vector<Usager*>& voitures) {
        float coeffV = 1.0; // Coefficient de vitesse
        int plaque_touch = 1;
        hasTurn = false;
        bool touchTurn = false;
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
                        if (etatFeu == FeuEtat::Vert) {
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

            // Détection de collision avec les plaques d'orientation
            touchTurn = false;
            for (auto& plaqueOrientation : plaquesOrientation) {
                if (plaqueOrientation.getGlobalBounds().intersects(sprite.getGlobalBounds())) {
                    Orientation val = plaqueOrientation.getValeur();
                    if (!hasTurn && !touchTurn  && val != Orientation::Turn) { // Empêche de tourner plusieurs fois sur la même plaque
                        touchTurn = true;
                        std::srand(pos.x + pos.y);
                        int choix = std::rand() % 100;
                        

                        if (choix == 1) {
                            hasTurn = true;
                            if (val == Orientation::Gauche) {
                                if (directionX > 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = -1; }
                            }

                            if (val == Orientation::Droite) {
                                if (directionX > 0) { nextDirectionX = 0; nextDirectionY = -1; }
                                else if (directionX < 0) { nextDirectionX = 0; nextDirectionY = 1; }
                                else if (directionY > 0) { nextDirectionY = 0; nextDirectionX = -1; }
                                else if (directionY < 0) { nextDirectionY = 0; nextDirectionX = 1; }
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

            if (hasTurn) {
                for (auto& plaqueOrientation : plaquesOrientation) {
                    Orientation val = plaqueOrientation.getValeur();
                    if (val == Orientation::Turn && plaqueOrientation.getGlobalBounds().intersects(sprite.getGlobalBounds())) {
                        directionY = nextDirectionY;
                        directionX = nextDirectionX;
                        hasTurn = false;
                    }
                }

            }

            orienterSprite();

            if (plaque_touch == 1) {
                coeffV = (coeffV > 0.9) ? 1.0 : ((coeffV < 0.2) ? (coeffV + 0.01) : (coeffV * 1.01)); // Accélération progressive
            }
            else if (plaque_touch == 2) {
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

            // Détection de collision avec d'autres voitures
            if (verifierCollision(voitures)) {
                //std::cout << "Collision détectée ! Arrêt de la voiture." << std::endl;
                coeffV = coeffV * 0.9; // Arrêter la voiture
                sprite.move(vitesse* directionX* coeffV, vitesse* directionY* coeffV);
            }
            else {
                // Déplacement normal
                if (dist >= 0.0) {
                    sprite.move(vitesse * directionX * 0.01 * dist, vitesse * directionY * 0.01 * dist);
                }
                else {
                    sprite.move(vitesse * directionX * coeffV, vitesse * directionY * coeffV);
                }


                // Si le véhicule dépasse les limites de la fenêtre, il revient au point de départ
                if (pos.x < -200 || pos.x > 2100 || pos.y < -200 || pos.y > 1200) {
                    resetPosition();
                }
            }


            mettreAJourHitbox();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    Sprite& getSprite() { return sprite; }

    virtual void resetPosition() {
        if (isBus) {
            if (directionX > 0)
                sprite.setPosition(-150, 340);
            else if (directionX < 0)
                sprite.setPosition(1000, 335);
        }
        else {
            if (directionX > 0)
                sprite.setPosition(-150, 340);
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

    void dessiner(RenderWindow& window) const {
        // Dessiner la voiture
        window.draw(sprite);
        window.draw(hitbox);
    }

    // Vérifier les collisions avec d'autres voitures
    bool verifierCollision(const vector<Usager*>& voitures) {
        for (const auto& voiture : voitures) {
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
        }
        else {
            // Sinon, ajuster progressivement
            sprite.setRotation(currentAngle + rotationStep * (angleDifference > 0 ? 1 : -1));
        }
    }

};