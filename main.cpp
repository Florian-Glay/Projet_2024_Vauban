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

#include "usagerVoiture.cpp"

int main() {

    std::srand(std::time(0));

    FeuCirculation feu_NS(Vector2f(240, 170), 0.5, FeuEtat::Vert);
    FeuCirculation feu_SN(Vector2f(620, 680), 0.5, FeuEtat::Vert);
    FeuCirculation feu_EO(Vector2f(152, 438), 0.5);
    FeuCirculation feu_OE(Vector2f(705, 420), 0.5);
    FeuCirculation* feu_vehicule[4] = { &feu_NS, &feu_SN, &feu_EO, &feu_OE };
    vector<FeuCirculation*> feux = { &feu_NS, &feu_SN, &feu_EO, &feu_OE };
    thread controleThread(&FeuCirculation::controleFeux, std::ref(feux));
    vector<Plaque> plaques;
    vector<PlaqueOrientation> plaquesOrientation;

    /*
    plaques.emplace_back(100, 490, &feu_EO, PlaqueEtat::Ralentisseur, 70, 10, PlaqueDeg::Droite);
    plaques.emplace_back(150, 490, &feu_EO, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite);

    plaques.emplace_back(370, 100, &feu_NS, PlaqueEtat::Ralentisseur, 10, 70, PlaqueDeg::Haut);
    plaques.emplace_back(370, 150, &feu_NS, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Haut);

    plaques.emplace_back((864 - 100), 370, &feu_EO, PlaqueEtat::Ralentisseur, 70, 10, PlaqueDeg::Gauche);
    plaques.emplace_back((864 - 150), 370, &feu_EO, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche);

    plaques.emplace_back(500, (864 - 100), &feu_NS, PlaqueEtat::Ralentisseur, 10, 70, PlaqueDeg::Bas);
    plaques.emplace_back(500, (864 - 150), &feu_NS, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas);

    
    plaquesOrientation.emplace_back(PlaqueOrientation(370, 100, 5, 5, Orientation::Droite));
    plaquesOrientation.emplace_back(PlaqueOrientation(375, 415, 5, 5, Orientation::Turn));

    plaquesOrientation.emplace_back(PlaqueOrientation(764, 370, 5, 5, Orientation::Gauche));
    plaquesOrientation.emplace_back(PlaqueOrientation(460, 400, 5, 5, Orientation::Turn));

    plaquesOrientation.emplace_back(PlaqueOrientation(100, 490, 5, 5, Orientation::Gauche));
    plaquesOrientation.emplace_back(PlaqueOrientation(500, 450, 5, 5, Orientation::Turn));


    plaquesOrientation.emplace_back(PlaqueOrientation(410, 480, 5, 5, Orientation::Turn));

    plaquesOrientation.emplace_back(PlaqueOrientation(500, 764, 5, 5, Orientation::Droite));*/




    vector<unique_ptr<Usager>> usagers;
    usagers.emplace_back(make_unique<Usager>(-150, 340, 2, feu_vehicule, 0.4, path_image + "voiture_1.png", 1, 0, false));
    //usagers.emplace_back(make_unique<Usager>(420, 880, 4, feu_vehicule, 0.5, path_image + "voiture_1.png", 0, -1, false));

    //usagers.emplace_back(make_unique<Usager>(0, 490, 4, feu_vehicule, 0.5, path_image + "voiture_1.png", 1, 0, false));
    //usagers.emplace_back(make_unique<Usager>(880, 414, 4, feu_vehicule, 0.5, path_image + "voiture_1.png", -1, 0, false));

    // Remplir le vecteur de pointeurs
    vector<Usager*> usagersPtrs;
    for (auto& usager : usagers) {
        usagersPtrs.push_back(usager.get());
    }

    // Ajout des threads pour déplacer les usagers
    vector<thread> threads;
    for (auto& usager : usagers) {
        threads.emplace_back(&Usager::deplacer, usager.get(), std::ref(plaques), std::ref(plaquesOrientation), std::ref(usagersPtrs));
    }

    // Résolution cible pour le contenu
    const int targetWidth = 1920;  // Largeur du contenu
    const int targetHeight = 972; // Hauteur du contenu
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
    if (!backgroundImage.loadFromFile(path_image + "large_map_little.png")) {
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

        // Dessiner les usagers
        for (const auto& usager : usagers) {
            usager->dessiner(window);
        }

        for (auto feu : feux) {
            feu->dessiner(window);
        }

        // Dessiner les plaques
        for (const auto& plaque : plaques) {
            plaque.dessiner(window);
        }

        // Dessiner les plaques d'orientation
        for (const auto& plaque : plaquesOrientation) {
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