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
#include <ctime>   // Pour time()


using namespace std;
using namespace sf;

#ifdef _MSC_VER
constexpr char _PATH_IMG_[] = "C:/Program Files/SFML/img/";
#else
constexpr char _PATH_IMG_[] = "img/";
#endif

const std::string path_image(_PATH_IMG_);

enum class FeuEtat { Rouge, Orange, Vert };

class FeuCirculation {
private:
    FeuEtat etat;
    mutable std::mutex mtx;
    Texture textureRouge;
    Texture textureOrange;
    Texture textureVert;
    Sprite spriteFeu;

public:
    Vector2f position;
    bool waiting_time = false;

    FeuCirculation(Vector2f pos, float size, FeuEtat f_etat = FeuEtat::Rouge) : etat(f_etat), position(pos) {
        // Charger les textures pour chaque état
        if (!textureRouge.loadFromFile(path_image + "traffic_red_light.png")) {
            cerr << "Erreur lors du chargement de l'image feu_rouge.png" << endl;
        }
        if (!textureOrange.loadFromFile(path_image + "traffic_orange_light.png")) {
            cerr << "Erreur lors du chargement de l'image feu_orange.png" << endl;
        }
        if (!textureVert.loadFromFile(path_image + "traffic_green_light.png")) {
            cerr << "Erreur lors du chargement de l'image feu_vert.png" << endl;
        }

        // Initialiser le sprite
        spriteFeu.setTexture(textureRouge); // Par défaut, rouge
        spriteFeu.setOrigin(textureRouge.getSize().x / 2, textureRouge.getSize().y);
        spriteFeu.setPosition(pos);
        spriteFeu.setScale(size, size);
        changerTexture();
    }

    void changerEtat() {
        std::lock_guard<std::mutex> lock(mtx);
        etat = (etat == FeuEtat::Rouge) ? FeuEtat::Vert :
            (etat == FeuEtat::Vert) ? FeuEtat::Orange : FeuEtat::Rouge;
        changerTexture();
    }

    void changerTexture() {
        // Mettre à jour la texture du sprite ici
        if (etat == FeuEtat::Rouge) {
            spriteFeu.setTexture(textureRouge);
        }
        else if (etat == FeuEtat::Orange) {
            spriteFeu.setTexture(textureOrange);
        }
        else if (etat == FeuEtat::Vert) {
            spriteFeu.setTexture(textureVert);
        }
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
        window.draw(spriteFeu);
    }

};