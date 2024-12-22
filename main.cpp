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
#include <chrono>
#include <sstream> // Inclure pour utiliser std::ostringstream


using namespace std;
using namespace sf;

#include "usagerVoiture.cpp"
#include "potentiometre.cpp"

int main() {

    std::srand(std::time(0));

    float timeSpeed = 1.0f;
    // Créer un potentiomètre
    Potentiometer potentiometer(30, 870, 200, 10, 0.1f, 1.0f, &timeSpeed);

    FeuCirculation feu_1(Vector2f(245, 315), 0.5, FeuEtat::Vert);
    FeuCirculation feu_2(Vector2f(470, 310), 0.5, FeuEtat::Vert);
    FeuCirculation feu_3(Vector2f(435, 475), 0.5);
    FeuCirculation feu_4(Vector2f(690, 315), 0.5, FeuEtat::Vert);
    FeuCirculation feu_5(Vector2f(955, 310), 0.5, FeuEtat::Vert);
    FeuCirculation feu_6(Vector2f(920, 475), 0.5);
    FeuCirculation feu_7(Vector2f(730, 170), 0.5);
    FeuCirculation feu_8(Vector2f(730, 805), 0.5);
    FeuCirculation feu_9(Vector2f(915, 970), 0.5);
    FeuCirculation feu_10(Vector2f(970, 840), 0.5, FeuEtat::Vert);
    FeuCirculation feu_11(Vector2f(1390, 935), 0.5, FeuEtat::Vert);
    FeuCirculation feu_12(Vector2f(1700, 840), 0.5, FeuEtat::Vert);
    FeuCirculation feu_13(Vector2f(1390, 315), 0.5, FeuEtat::Vert);
    FeuCirculation feu_14(Vector2f(1680, 310), 0.5, FeuEtat::Vert);
    FeuCirculation feu_15(Vector2f(1640, 475), 0.5);
    FeuCirculation* feu_vehicule[15] = { &feu_1, &feu_2, &feu_3, &feu_4, &feu_5, &feu_6, &feu_7, &feu_8, &feu_9, &feu_10, &feu_11, &feu_12, &feu_13, &feu_14, &feu_15};
    vector<FeuCirculation*> feux = { &feu_1, &feu_2, &feu_3, &feu_4, &feu_5, &feu_6, &feu_7, &feu_8, &feu_9, &feu_10, &feu_11, &feu_12, &feu_13, &feu_14, &feu_15 };
    thread controleThread(&FeuCirculation::controleFeux, std::ref(feux), std::ref(timeSpeed));
    vector<Plaque> plaques;
    vector<PlaqueOrientation> plaquesOrientation;

    plaques.emplace_back(205, 340, &feu_1, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite);
    plaques.emplace_back(245, 340, &feu_1, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite);

    plaques.emplace_back(510, 285, &feu_2, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche);
    plaques.emplace_back(470, 285, &feu_2, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche);

    plaques.emplace_back(360, 515, &feu_3, PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Bas);
    plaques.emplace_back(360, 475, &feu_3, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas);

    plaques.emplace_back(650, 340, &feu_4, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite);
    plaques.emplace_back(690, 340, &feu_4, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite);

    plaques.emplace_back(995, 285, &feu_5, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche);
    plaques.emplace_back(955, 285, &feu_5, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche);

    plaques.emplace_back(860, 515, &feu_6, PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Bas);
    plaques.emplace_back(860, 475, &feu_6, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas);

    plaques.emplace_back(795, 130, &feu_7, PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Haut);
    plaques.emplace_back(795, 170, &feu_7, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Haut);

    plaques.emplace_back(795, 765, &feu_8, PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Haut);
    plaques.emplace_back(795, 805, &feu_8, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Haut);

    plaques.emplace_back(860, 1010, &feu_9, PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Bas);
    plaques.emplace_back(860, 970, &feu_9, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas);

    plaques.emplace_back(1010, 870, &feu_10, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche);
    plaques.emplace_back(970, 870, &feu_10, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche);

    plaques.emplace_back(1350, 910, &feu_11, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite);
    plaques.emplace_back(1390, 910, &feu_11, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite);

    plaques.emplace_back(1740, 870, &feu_12, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche);
    plaques.emplace_back(1700, 870, &feu_12, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche);

    plaques.emplace_back(1350, 340, &feu_13, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite);
    plaques.emplace_back(1390, 340, &feu_13, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite);

    plaques.emplace_back(1720, 285, &feu_14, PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche);
    plaques.emplace_back(1680, 285, &feu_14, PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche);

    plaques.emplace_back(1575, 515, &feu_15, PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Bas);
    plaques.emplace_back(1575, 475, &feu_15, PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas);

    

    /*
        
    plaquesOrientation.emplace_back(PlaqueOrientation(370, 100, 5, 5, Orientation::Droite));
    plaquesOrientation.emplace_back(PlaqueOrientation(375, 415, 5, 5, Orientation::Turn));

    plaquesOrientation.emplace_back(PlaqueOrientation(764, 370, 5, 5, Orientation::Gauche));
    plaquesOrientation.emplace_back(PlaqueOrientation(460, 400, 5, 5, Orientation::Turn));

    plaquesOrientation.emplace_back(PlaqueOrientation(100, 490, 5, 5, Orientation::Gauche));
    plaquesOrientation.emplace_back(PlaqueOrientation(500, 450, 5, 5, Orientation::Turn));


    plaquesOrientation.emplace_back(PlaqueOrientation(410, 480, 5, 5, Orientation::Turn));

    plaquesOrientation.emplace_back(PlaqueOrientation(500, 764, 5, 5, Orientation::Droite));*/


    plaquesOrientation.emplace_back(PlaqueOrientation(360, 475, 5, 5, Orientation::GaucheDroite, 0, -1, true));
    plaquesOrientation.emplace_back(PlaqueOrientation(360, 315, 5, 5, Orientation::Turn, 1, 0));
    plaquesOrientation.emplace_back(PlaqueOrientation(360, 260, 5, 5, Orientation::Turn, -1, 0));

    plaquesOrientation.emplace_back(PlaqueOrientation(970, 870, 5, 5, Orientation::GaucheDroite, -1, 0, true));
    plaquesOrientation.emplace_back(PlaqueOrientation(830, 870, 5, 5, Orientation::Turn, 0, -1));
    plaquesOrientation.emplace_back(PlaqueOrientation(775, 870, 5, 5, Orientation::Turn, 0, 1));



    vector<unique_ptr<Usager>> usagers;
    usagers.emplace_back(make_unique<Usager>(501, 133, 2, feu_vehicule, 0.4, path_image + "voiture_1.png", 1, 0, false));
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
        threads.emplace_back(&Usager::deplacer, usager.get(), std::ref(plaques), std::ref(plaquesOrientation), std::ref(usagersPtrs), std::ref(timeSpeed));
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

    // Création de la boîte en bas à gauche
    RectangleShape box_2(Vector2f(230, 70));
    box_2.setFillColor(Color(200, 200, 200, 255)); // Couleur gris clair
    box_2.setOutlineColor(Color::Black);          // Bordure noire
    box_2.setOutlineThickness(2);
    box_2.setPosition(20, 860); // En bas à droite

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
    backgroundSprite.setPosition(window.getSize().x / 2.0, window.getSize().y / 2.0);
    backgroundSprite.setScale(1, 1);
    // Définir la transparence à 50 %
    sf::Color transparentColor = backgroundSprite.getColor(); // Récupère la couleur actuelle
    transparentColor.a = 255; // 128 correspond à 50 % de transparence (255/2)
    backgroundSprite.setColor(transparentColor);

    Texture backgroundImage2;
    Sprite backgroundSprite2;
    if (!backgroundImage2.loadFromFile(path_image + "Buildings.png")) {
        cerr << "Erreur pendant le chargement des images" << endl;
        return EXIT_FAILURE;
    }
    backgroundSprite2.setTexture(backgroundImage2);
    sf::FloatRect bounds2 = backgroundSprite2.getLocalBounds();
    backgroundSprite2.setOrigin(bounds2.width / 2, bounds2.height / 2);
    backgroundSprite2.setPosition(window.getSize().x / 2.0, window.getSize().y / 2.0);
    backgroundSprite2.setScale(0.5, 0.5);
    // Définir la transparence à 50 %
    sf::Color transparentColor2 = backgroundSprite2.getColor(); // Récupère la couleur actuelle
    transparentColor2.a = 255; // 128 correspond à 50 % de transparence (255/2)
    backgroundSprite2.setColor(transparentColor2);


    // Création de l'aléatoire
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(2000, 4000); // Intervalle entre 2000ms et 4000ms
    sf::Clock creationClock; // Horloge pour mesurer le temps écoulé
    int nextCreationTime = distrib(gen); // Temps jusqu'à la prochaine création de voiture
    int entityNum = 0;

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

            // Gérer les événements du potentiomètre
            potentiometer.handleEvent(event, window);
        }

        // Vérifier si le délai est écoulé pour créer une nouvelle voiture
        if (creationClock.getElapsedTime().asMilliseconds() >= nextCreationTime && entityNum < 20) {
            // Créer une nouvelle voiture
            int xPosition = distrib(gen) % 1920; // Exemple de position aléatoire en x
            int yPosition = distrib(gen) % 972; // Exemple de position aléatoire en y
            usagers.emplace_back(make_unique<Usager>(xPosition, yPosition, 2, feu_vehicule, 0.4, path_image + "voiture_1.png", 1, 0, false));
            usagersPtrs.push_back(usagers.back().get());

            // Ajouter un thread pour la nouvelle voiture
            threads.emplace_back(&Usager::deplacer, usagers.back().get(), std::ref(plaques), std::ref(plaquesOrientation), std::ref(usagersPtrs), std::ref(timeSpeed));

            // Réinitialiser l'horloge et recalculer le délai
            creationClock.restart();
            nextCreationTime = distrib(gen);
            entityNum++;
        }

        // Mettre à jour le potentiomètre
        potentiometer.update(window);

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


        window.draw(backgroundSprite2);

        // Récupération des coordonnées de la souris
        Vector2i mousePos = Mouse::getPosition(window);
        text.setString("X: " + to_string(mousePos.x) + "\nY: " + to_string(mousePos.y));

        // Affichage
        window.draw(box);  // Dessiner la boîte
        window.draw(text); // Dessiner le texte

        // Dessiner le potentiomètre
        window.draw(box_2);  // Dessiner la boîte
        potentiometer.draw(window);

        // Afficher la valeur actuelle de timeSpeed
        Text text;
        text.setFont(font);
        std::ostringstream oss;
        oss.precision(1); // Nombre de chiffres après la virgule
        oss << std::fixed << (1.0 / timeSpeed); // Formater le nombre
        text.setString("Vitesse de simulation : " + oss.str());
        text.setCharacterSize(18);
        text.setFillColor(Color::Black);
        text.setPosition(30, 900);
        window.draw(text);

        window.display();
    }

    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }

    controleThread.join();
    return 0;
}