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
#include <stdexcept>


using namespace std;
using namespace sf;

#include "usagerVoiture.cpp"
#include "potentiometre.cpp"

int main() {

    std::srand(std::time(NULL));

    float timeSpeed = 1.0f;
    int entityNum = 0;
    int entityNumBus = 0;
    int entityNumPieton = 0;
    // Créer un potentiomètre
    Potentiometer potentiometer(30, 870, 200.0f, 10.0f, 0.1f, 1.0f, &timeSpeed);

    // Feux de circulation (allocation dynamique)
    std::vector<std::unique_ptr<FeuCirculation>> feux;
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(245, 315), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(470, 310), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(435, 475), 0.5f));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(690, 315), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(955, 310), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(920, 475), 0.5f));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(730, 170), 0.5f));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(730, 805), 0.5f));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(915, 970), 0.5f));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(970, 840), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(1390, 935), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(1700, 840), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(1390, 315), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(1680, 310), 0.5f, FeuEtat::Vert));
    feux.emplace_back(std::make_unique<FeuCirculation>(Vector2f(1640, 475), 0.5f));


    std::vector<std::shared_ptr<FeuCirculation>> feuxPtrs;
    for (auto& feu : feux) {
        feuxPtrs.push_back(std::move(feu));
    }

    thread controleThread(&FeuCirculation::controleFeux, std::ref(feuxPtrs), std::ref(timeSpeed));

    // Plaques (allocation dynamique)
    std::vector<std::shared_ptr<Plaque>> plaques;
    // VOITURES
    plaques.emplace_back(std::make_unique<Plaque>(205, 340, feuxPtrs[0], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(245, 340, feuxPtrs[0], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(510, 285, feuxPtrs[1], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(470, 285, feuxPtrs[1], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(360, 515, feuxPtrs[2], PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(360, 475, feuxPtrs[2], PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(650, 340, feuxPtrs[3], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(690, 340, feuxPtrs[3], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(995, 285, feuxPtrs[4], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(955, 285, feuxPtrs[4], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(860, 515, feuxPtrs[5], PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(860, 475, feuxPtrs[5], PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(795, 130, feuxPtrs[6], PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(795, 170, feuxPtrs[6], PlaqueEtat::Stop, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(795, 765, feuxPtrs[7], PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(795, 805, feuxPtrs[7], PlaqueEtat::Stop, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(860, 1010, feuxPtrs[8], PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(860, 970, feuxPtrs[8], PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(1010, 870, feuxPtrs[9], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(970, 870, feuxPtrs[9], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(1350, 910, feuxPtrs[10], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1390, 910, feuxPtrs[10], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1740, 870, feuxPtrs[11], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(1700, 870, feuxPtrs[11], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(1350, 340, feuxPtrs[12], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1390, 340, feuxPtrs[12], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1720, 285, feuxPtrs[13], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(1680, 285, feuxPtrs[13], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(1575, 515, feuxPtrs[14], PlaqueEtat::Ralentisseur, 10, 40, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(1575, 475, feuxPtrs[14], PlaqueEtat::Stop, 10, 5, PlaqueDeg::Bas));

    // Pietons
    plaques.emplace_back(std::make_unique<Plaque>(740, 210, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(910, 210, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(740, 410, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(910, 410, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(740, 830, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(910, 830, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(740, 940, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(910, 940, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(1460, 830, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1630, 830, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(1460, 410, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1630, 410, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(290, 410, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(425, 410, feuxPtrs[3], PlaqueEtat::Stop_P, 5, 10, PlaqueDeg::Gauche));

    plaques.emplace_back(std::make_unique<Plaque>(720, 220, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(720, 400, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(930, 220, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(930, 400, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(930, 845, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(930, 930, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(1440, 220, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(1440, 400, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(1650, 220, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(1650, 400, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(1440, 845, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(1440, 930, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(1650, 845, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(1650, 930, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));

    plaques.emplace_back(std::make_unique<Plaque>(450, 220, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(450, 400, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));
    plaques.emplace_back(std::make_unique<Plaque>(270, 220, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Bas));
    plaques.emplace_back(std::make_unique<Plaque>(270, 400, feuxPtrs[2], PlaqueEtat::Stop_P, 10, 5, PlaqueDeg::Haut));



    // BUS
    plaques.emplace_back(std::make_unique<Plaque>(205, 380, feuxPtrs[0], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(245, 380, feuxPtrs[0], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1720, 235, feuxPtrs[13], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(1680, 235, feuxPtrs[13], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(510, 235, feuxPtrs[1], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(470, 235, feuxPtrs[1], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(995, 235, feuxPtrs[4], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(955, 235, feuxPtrs[4], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Gauche));
    plaques.emplace_back(std::make_unique<Plaque>(650, 380, feuxPtrs[3], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(690, 380, feuxPtrs[3], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1350, 380, feuxPtrs[12], PlaqueEtat::Ralentisseur, 40, 10, PlaqueDeg::Droite));
    plaques.emplace_back(std::make_unique<Plaque>(1390, 380, feuxPtrs[12], PlaqueEtat::Stop, 5, 10, PlaqueDeg::Droite));

    std::vector<PlaqueOrientation> plaquesOrientation;
    // Carrefour 1

    plaquesOrientation.emplace_back(860, 475, 5, 5, Orientation::GaucheDroite, 0, -1);
    plaquesOrientation.emplace_back(860, 315, 5, 5, Orientation::Turn, 1, 0);
    plaquesOrientation.emplace_back(860, 260, 5, 5, Orientation::Turn, -1, 0);

    plaquesOrientation.emplace_back(790, 165, 5, 5, Orientation::GaucheDroite, 0, 1);
    plaquesOrientation.emplace_back(790, 360, 5, 5, Orientation::Turn, 1, 0);
    plaquesOrientation.emplace_back(790, 305, 5, 5, Orientation::Turn, -1, 0);

    plaquesOrientation.emplace_back(685, 335, 5, 5, Orientation::GaucheDroite, 1, 0);
    plaquesOrientation.emplace_back(810, 335, 5, 5, Orientation::Turn, 0, 1);
    plaquesOrientation.emplace_back(875, 335, 5, 5, Orientation::Turn, 0, -1);

    plaquesOrientation.emplace_back(950, 290, 5, 5, Orientation::GaucheDroite, -1, 0);
    plaquesOrientation.emplace_back(765, 290, 5, 5, Orientation::Turn, 0, 1);
    plaquesOrientation.emplace_back(830, 290, 5, 5, Orientation::Turn, 0, -1);

    // Carrefour 2

    plaquesOrientation.emplace_back(1580, 475, 5, 5, Orientation::GaucheDroite, 0, -1, true);
    plaquesOrientation.emplace_back(1580, 315, 5, 5, Orientation::Turn, 1, 0);
    plaquesOrientation.emplace_back(1580, 260, 5, 5, Orientation::Turn, -1, 0);


    plaquesOrientation.emplace_back(1390, 335, 5, 5, Orientation::Gauche, 1, 0);
    plaquesOrientation.emplace_back(1540, 335, 5, 5, Orientation::Turn, 0, 1);

    plaquesOrientation.emplace_back(1680, 290, 5, 5, Orientation::Gauche, -1, 0);
    plaquesOrientation.emplace_back(1490, 290, 5, 5, Orientation::Turn, 0, 1);

    // Carrefour 3
    int a = 550;
    plaquesOrientation.emplace_back(1520, 845, 5, 5, Orientation::GaucheDroite, 0, 1, true);
    plaquesOrientation.emplace_back(1520, 885, 5, 5, Orientation::Turn, -1, 0);
    plaquesOrientation.emplace_back(1520, 925, 5, 5, Orientation::Turn, 1, 0);

    
    plaquesOrientation.emplace_back(1390, 900, 5, 5, Orientation::Droite, 1, 0);
    plaquesOrientation.emplace_back(1595, 900, 5, 5, Orientation::Turn, 0, -1);

    plaquesOrientation.emplace_back(1700, 880, 5, 5, Orientation::Gauche, -1, 0);
    plaquesOrientation.emplace_back(1550, 880, 5, 5, Orientation::Turn, 0, -1);

    // Autre

    plaquesOrientation.emplace_back(865, 970, 5, 5, Orientation::Droite, 0, -1);
    plaquesOrientation.emplace_back(865, 885, 5, 5, Orientation::Turn, 1, 0);

    plaquesOrientation.emplace_back(790, 800, 5, 5, Orientation::Gauche, 0, -1);
    plaquesOrientation.emplace_back(790, 925, 5, 5, Orientation::Turn, 1, 0);

    plaquesOrientation.emplace_back(360, 475, 5, 5, Orientation::GaucheDroite, 0, -1, true);
    plaquesOrientation.emplace_back(360, 315, 5, 5, Orientation::Turn, 1, 0);
    plaquesOrientation.emplace_back(360, 260, 5, 5, Orientation::Turn, -1, 0);

    plaquesOrientation.emplace_back(970, 870, 5, 5, Orientation::GaucheDroite, -1, 0, true);
    plaquesOrientation.emplace_back(830, 870, 5, 5, Orientation::Turn, 0, -1);
    plaquesOrientation.emplace_back(775, 870, 5, 5, Orientation::Turn, 0, 1);

    

    // Usagers (allocation dynamique)
    std::vector<std::unique_ptr<Usager>> usagers;
    usagers.emplace_back(std::make_unique<Usager>(501, 133, 2, feuxPtrs, 0.4f, path_image, 1, 0, UsagerEtat::Voiture));

    std::vector<Usager*> usagersPtrs;
    for (auto& usager : usagers) {
        usagersPtrs.push_back(usager.get());
    }

    // Threads pour les déplacements des usagers
    std::vector<std::thread> threads;
    for (auto& usager : usagers) {
        //threads.emplace_back([usager = usager.get(), &plaques, &plaquesOrientation, &usagersPtrs, &timeSpeed, &entityNum]() {
            //usager->deplacer(plaques, plaquesOrientation, usagersPtrs, timeSpeed, entityNum);
            //});
        threads.emplace_back(&Usager::deplacer, usager.get(), std::ref(plaques), std::ref(plaquesOrientation), std::ref(usagersPtrs), std::ref(timeSpeed), std::ref(entityNum));
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
    sf::Clock creationClockBus; // Horloge pour mesurer le temps écoulé
    sf::Clock creationClockPieton; // Horloge pour mesurer le temps écoulé
    int nextCreationTime = distrib(gen); // Temps jusqu'à la prochaine création de voiture
    int nextCreationTimeBus = distrib(gen);
    int nextCreationTimePieton = distrib(gen);
    

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
            int xPosition = distrib(gen) % 1920;
            int yPosition = distrib(gen) % 972;
            usagers.emplace_back(std::make_unique<Usager>(xPosition, yPosition, 2, feuxPtrs, 0.4f, path_image, 1, 0, UsagerEtat::Voiture));
            usagersPtrs.push_back(usagers.back().get());
            // Ajouter un thread pour la nouvelle voiture
            threads.emplace_back(&Usager::deplacer, usagers.back().get(), std::ref(plaques), std::ref(plaquesOrientation), std::ref(usagersPtrs), std::ref(timeSpeed), std::ref(entityNum));
            // Réinitialiser l'horloge et recalculer le délai
            creationClock.restart();
            nextCreationTime = distrib(gen);
            entityNum++;
        }

        if (creationClockBus.getElapsedTime().asMilliseconds() >= nextCreationTimeBus && entityNumBus < 4) {
            int xPosition = distrib(gen) % 1920;
            int yPosition = distrib(gen) % 972;
            usagers.emplace_back(std::make_unique<Usager>(xPosition, yPosition, 1.5, feuxPtrs, 0.4f, path_image, 1, 0, UsagerEtat::Bus));
            usagersPtrs.push_back(usagers.back().get());
            // Ajouter un thread pour la nouvelle voiture
            threads.emplace_back(&Usager::deplacer, usagers.back().get(), std::ref(plaques), std::ref(plaquesOrientation), std::ref(usagersPtrs), std::ref(timeSpeed), std::ref(entityNum));
            // Réinitialiser l'horloge et recalculer le délai
            creationClockBus.restart();
            nextCreationTimeBus = distrib(gen);
            entityNumBus++;
        }

        if (creationClockPieton.getElapsedTime().asMilliseconds() >= nextCreationTimePieton && entityNumPieton < 15) {
            int xPosition = distrib(gen) % 1920;
            int yPosition = distrib(gen) % 972;
            usagers.emplace_back(std::make_unique<Usager>(xPosition, yPosition, 1.0, feuxPtrs, 0.8f, path_image, 1, 0, UsagerEtat::Pieton));
            usagersPtrs.push_back(usagers.back().get());
            // Ajouter un thread pour la nouvelle voiture
            threads.emplace_back(&Usager::deplacer, usagers.back().get(), std::ref(plaques), std::ref(plaquesOrientation), std::ref(usagersPtrs), std::ref(timeSpeed), std::ref(entityNum));
            // Réinitialiser l'horloge et recalculer le délai
            creationClockPieton.restart();
            nextCreationTimePieton = distrib(gen);
            entityNumPieton++;
        }

        // Mettre à jour le potentiomètre
        potentiometer.update(window);

        window.clear(Color::Black);
        window.draw(backgroundSprite);

        // Dessiner les usagers
        for (const auto& usager : usagers) {
            usager->dessiner(window);
        }

        for (const auto& feu : feuxPtrs) {
            feu->dessiner(window);
        }
        for (const auto& plaque : plaques) {
            plaque->dessiner(window);
        }
        for (const auto& plaqueOrientation : plaquesOrientation) {
            plaqueOrientation.dessiner(window);
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
        text.setPosition(30.0f, 900.0f);
        window.draw(text);

        window.display();
    }

    // Attente des threads
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    controleThread.join();

    return 0;
}