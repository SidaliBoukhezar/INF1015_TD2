#include "Developpeur.hpp";
#include "Jeu.hpp";
#include<iostream>

Developpeur::Developpeur(string nom) {
	paireNomJeux_.first = nom;
	paireNomJeux_.second.capacite = 0;
	paireNomJeux_.second.elements = nullptr;
	paireNomJeux_.second.nElements = 0;
}

string Developpeur:: getName() {
	return paireNomJeux_.first;
}

int Developpeur::compterNombreJeux(ListeJeux &listeJeux) {
	int nbJeux = 0;

	for (int i = 0; i < listeJeux.nElements; i++) {
		if (listeJeux.elements[i]->developpeur == paireNomJeux_.first) {
			nbJeux++;
		}
	}

	return nbJeux;
}

void Developpeur::updateListeJeux(ListeJeux &listeJeux) {
	int nbJeux = this->compterNombreJeux(listeJeux);

	paireNomJeux_.second.capacite = nbJeux;
	paireNomJeux_.second.nElements = nbJeux;
	paireNomJeux_.second.elements = new Jeu * [nbJeux];

	for (int i = 0; i < listeJeux.nElements; i++) {
		if (listeJeux.elements[i]->developpeur == paireNomJeux_.first) {
			paireNomJeux_.second.elements[i] = listeJeux.elements[i];
		}
	}
}

void Developpeur::afficherListeJeux() {

	cout << "-------- List Jeux Dev ---------\n";
	for (int i = 0; i < paireNomJeux_.second.nElements; i++) {
			cout << paireNomJeux_.second.elements[i]->titre + "\n";
	}
}

Developpeur:: ~Developpeur() {
	for (int i = 0; i < paireNomJeux_.second.nElements; i++) {
		delete paireNomJeux_.second.elements[i];
	}
	delete[] paireNomJeux_.second.elements;
}