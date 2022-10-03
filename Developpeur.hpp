#pragma once
#include <string>
#include "ListeJeux.hpp"
using namespace std;

class Developpeur
{
	//TODO: En faire une classe qui suit les principes OO.
	//TODO: La destruction d'un Developpeur doit s'assurer que la désallocation de ListeJeux est faite.
	//TODO: Les méthodes à faire...

public:
	Developpeur(string nomDev);
	~Developpeur();

	string getName();
	int compterNombreJeux(ListeJeux &listeJeux);
	void updateListeJeux(ListeJeux &listeJeux);
	void afficherListeJeux();

private:
	pair<string, ListeJeux> paireNomJeux_;
};
