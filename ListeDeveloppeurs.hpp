#pragma once
#include <cstddef>
#include "Developpeur.hpp"
using namespace std;

class ListeDeveloppeurs
{
	//TODO: En faire une classe qui suit les principes OO.
	//TODO: On veut pouvoir ajouter et enlever un Developpeur* de la liste, avec réallocation dynamique tel que faite pour ListeJeux.
	//NOTE: Le code sera principalement copié de certaines fonctions écrites pour la partie 1, mais mises dans une classe.
public:
	ListeDeveloppeurs();
	~ListeDeveloppeurs();

	void afficherDev();

	void ajouterDev(Developpeur &dev);
	void retirerDev(Developpeur &dev);


private:
	size_t nElements, capacite;
	Developpeur** elements;
	void doublerCapacite();
	bool devExist(Developpeur& dev);

};
