#include "ListeDeveloppeurs.hpp";
#include <iostream>

ListeDeveloppeurs::ListeDeveloppeurs() {
	nElements = 0;
	capacite = 0;
	elements = nullptr;
}

ListeDeveloppeurs::	~ListeDeveloppeurs() {
	for (int i = 0; i < nElements; i++) {
		delete elements[i];
	}
	delete[] elements;
	elements = nullptr;
}

void ListeDeveloppeurs::afficherDev() {
	cout << "-------- List Dev ---------\n";
	for (int i=0; i < nElements; i++) {
		cout << "-------- aficher Dev ---------\n";
		cout << elements[i]->getName() + "\n";
		elements[i]->afficherListeJeux();
	}
}

void ListeDeveloppeurs::ajouterDev(Developpeur &dev) {

	if (devExist(dev)) {
		return;
	}
	if (nElements == capacite) {
		doublerCapacite();
	}
	elements[nElements++] = &dev;
}

void ListeDeveloppeurs::doublerCapacite() {

	capacite = capacite * 2 + 1;
	Developpeur** listTemp = new Developpeur * [capacite];

	for (int i = 0; i < nElements; i++) {
		listTemp[i] = elements[i];
		delete elements[i];
	}
	elements = listTemp;

}

bool ListeDeveloppeurs::devExist(Developpeur &dev) {

	for (int i = 0; i < nElements; i++) {
		if (elements[i]->getName() == dev.getName()) {
			return true;
		}
	}
	return false;
}

void ListeDeveloppeurs::retirerDev(Developpeur& dev) {

	if (devExist(dev)) {

		Developpeur** listTemp = new Developpeur * [capacite];

		for (int i = 0; i < nElements; i++) {
			if (elements[i]->getName() != dev.getName()) {
				listTemp[i] = elements[i];
				delete elements[i];
			}
		}
		elements = listTemp;
		nElements--;
	}
}
