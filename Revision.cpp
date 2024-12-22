#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

void thread1(int& valeur, bool& continuer, bool& valeur_changee)
{
    while (continuer)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!valeur_changee)
        {
            valeur = rand() % 101; // Génère une valeur entre 0 et 100
            std::cout << "Envoyé : " << valeur << std::endl;
            valeur_changee = true;
        }
    }
}

void thread2(int& valeur, bool& continuer, bool& valeur_changee)
{
    while (continuer)
    {
        if (valeur_changee)
        {
            std::cout << "Reçu : " << valeur << std::endl;
            if (valeur == 10)
            {
                continuer = false;
                break;
            }
            valeur_changee = false;
        }
    }
}

int carrote()
{
    srand(static_cast<unsigned>(time(NULL))); // Initialisation de la graine aléatoire

    int valeur = rand() % 101; // Valeur initiale entre 0 et 100
    bool continuer = true;    // Indique si les threads doivent continuer
    bool valeur_changee = false; // Indique si la valeur a été changée

    std::thread premier_thread(thread1, std::ref(valeur), std::ref(continuer), std::ref(valeur_changee));
    std::thread second_thread(thread2, std::ref(valeur), std::ref(continuer), std::ref(valeur_changee));

    premier_thread.join();
    second_thread.join();

    std::cout << "Programme terminé." << std::endl;
    return 0;
}