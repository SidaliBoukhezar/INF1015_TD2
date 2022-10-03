#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "Developpeur.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"  //NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.

using namespace std;
using namespace iter;
using namespace gsl;

#pragma region "Fonctions de base pour vous aider"
template <typename T>
T lireType(istream& fichier)
{
	T valeur{};
	fichier.read(reinterpret_cast<char*>(&valeur), sizeof(valeur));
	return valeur;
}
#define erreurFataleAssert(message) assert(false&&(message)),terminate()
static const uint8_t enteteTailleVariableDeBase = 0xA0;
size_t lireUintTailleVariable(istream& fichier)
{
	uint8_t entete = lireType<uint8_t>(fichier);
	switch (entete) {
	case enteteTailleVariableDeBase+0: return lireType<uint8_t>(fichier);
	case enteteTailleVariableDeBase+1: return lireType<uint16_t>(fichier);
	case enteteTailleVariableDeBase+2: return lireType<uint32_t>(fichier);
	default:
		erreurFataleAssert("Tentative de lire un entier de taille variable alors que le fichier contient autre chose à cet emplacement.");
	}
}

string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUintTailleVariable(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}
gsl::span<Jeu*> spanListeJeux(const ListeJeux& liste)
{
	return gsl::span(liste.elements, liste.nElements);
}
gsl::span<Concepteur*> spanListeConcepteurs(const ListeConcepteurs& liste)
{
	return gsl::span(liste.elements, liste.nElements);
}
#pragma endregion

//TODO: Fonction qui cherche un concepteur par son nom dans une ListeJeux.
// Cette fonction renvoie le pointeur vers le concepteur si elle le trouve dans
// un des jeux de la ListeJeux. En cas contraire, elle renvoie un pointeur nul.

Concepteur* chercherConcepteurs(ListeJeux &listeJeux, string nom_concepteur) {
	for (int i = 0; i < listeJeux.nElements; i++) {
		for (int j = 0; j < listeJeux.elements[i]->concepteurs.nElements; j++) {
			if ( listeJeux.elements[i]->concepteurs.elements[j]->nom == nom_concepteur) 
				return listeJeux.elements[i]->concepteurs.elements[j];
		}
	}
	return nullptr;
}


Concepteur* lireConcepteur(ListeJeux listeJeux, istream& fichier, Jeu* pJeu)
{
	Concepteur concepteur = {}; // On initialise une structure vide de type Concepteur.
	concepteur.nom = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays = lireString(fichier);
	// Rendu ici, les champs précédents de la structure concepteur sont remplis
	// avec la bonne information.

	//TODO: Ajouter en mémoire le concepteur lu. Il faut revoyer le pointeur créé.
	
	Concepteur* pConcepteur = chercherConcepteurs(listeJeux, concepteur.nom);

	if (pConcepteur == nullptr){
		Concepteur* ajoutConcepteur = new Concepteur(concepteur);
		cout << "L'allocation du concepteur est réussite" << endl;
		return ajoutConcepteur;
	}
	pConcepteur->jeuxConcus.elements[pConcepteur->jeuxConcus.nElements++];
	pJeu->concepteurs.elements[pJeu->concepteurs.nElements] = pConcepteur;
	pJeu->concepteurs.nElements++;
	// Attention, valider si le concepteur existe déjà avant de le créer, sinon
	// on va avoir des doublons car plusieurs jeux ont des concepteurs en commun
	// dans le fichier binaire. Pour ce faire, cette fonction aura besoin de
	// la liste de jeux principale en paramètre.
	// Afficher un message lorsque l'allocation du concepteur est réussie.
	  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.
	return nullptr; //TODO: Retourner le pointeur vers le concepteur crée.
}

//TODO: Fonction qui change la taille du tableau de jeux de ListeJeux.
// Cette fonction doit recevoir en paramètre la nouvelle capacité du nouveau
// tableau. Il faut allouer un nouveau tableau assez grand, copier ce qu'il y
// avait dans l'ancien, et éliminer l'ancien trop petit. N'oubliez pas, on copie
// des pointeurs de jeux. Il n'y a donc aucune nouvelle allocation de jeu ici !

void changeTailleTableau(ListeJeux listeJeux, size_t nouvelleCapacite) {

	int i = 0;
	Jeu** nouveauTableau;
	nouveauTableau = new Jeu * [nouvelleCapacite];

	for (; i < listeJeux.nElements; i++) {
		nouveauTableau[i] = listeJeux.elements[i];
	}

	listeJeux.capacite = nouvelleCapacite;
	delete[] listeJeux.elements;
	listeJeux.elements = nouveauTableau;
}

//TODO: Fonction pour ajouter un Jeu à ListeJeux.
// Le jeu existant déjà en mémoire, on veut uniquement ajouter le pointeur vers
// le jeu existant. De plus, en cas de saturation du tableau elements, cette
// fonction doit doubler la taille du tableau elements de ListeJeux.
// Utilisez la fonction pour changer la taille du tableau écrite plus haut.

void ajouterJeu(ListeJeux& listeJeux, Jeu& jeu) {
	if (listeJeux.capacite == 0) {
		changeTailleTableau(listeJeux, 1);
	}

	if (listeJeux.nElements == listeJeux.capacite) {
		changeTailleTableau(listeJeux, (2 * listeJeux.capacite));
	}

	listeJeux.elements[listeJeux.nElements++] = &jeu;
}

//TODO: Fonction qui enlève un jeu de ListeJeux.
// Attention, ici il n'a pas de désallocation de mémoire. Elle enlève le
// pointeur de la ListeJeux, mais le jeu pointé existe encore en mémoire.
// Puisque l'ordre de la ListeJeux n'a pas être conservé, on peut remplacer le
// jeu à être retiré par celui présent en fin de liste et décrémenter la taille
// de celle-ci.

void enleverJeu(ListeJeux& listeJeux, Jeu& jeu) {
	for (int i = 0; i < listeJeux.nElements; i++) {
		if (listeJeux.elements[i]->titre == jeu.titre) {
			listeJeux.elements[i] = listeJeux.elements[--listeJeux.nElements];
		}
	}
}



void enleverConcepteur(ListeConcepteurs& listeConcepteur, Concepteur& concepteur) {
	
	for (int i = 0; i < listeConcepteur.nElements; i++) {
		if (listeConcepteur.elements[i]->nom == concepteur.nom) {
			listeConcepteur.nElements--;
			listeConcepteur.elements[i] = listeConcepteur.elements[listeConcepteur.nElements];
		}
	}
}


Jeu* lireJeu(istream& fichier, ListeJeux& listeJeux)
{
	Jeu jeu = {}; // On initialise une structure vide de type Jeu
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = int(lireUintTailleVariable(fichier));
	jeu.developpeur = lireString(fichier);
	jeu.concepteurs.nElements = lireUintTailleVariable(fichier);
	// Rendu ici, les champs précédents de la structure jeu sont remplis avec la
	// bonne information.

	//TODO: Ajouter en mémoire le jeu lu. Il faut revoyer le pointeur créé.
	// Attention, il faut aussi créer un tableau dynamique pour les concepteurs
	// que contient un jeu. Servez-vous de votre fonction d'ajout de jeu car la
	// liste de jeux participé est une ListeJeu. Afficher un message lorsque
	// l'allocation du jeu est réussie.

	Jeu * pJeu = new Jeu(jeu);
	pJeu->concepteurs.elements = new Concepteur* [pJeu->concepteurs.nElements];
	pJeu->concepteurs.nElements = 0;

	for ([[maybe_unused]] size_t i : iter::range(jeu.concepteurs.nElements)) {
		Concepteur* pConcepteur = lireConcepteur(listeJeux, fichier, pJeu);  //TODO: Mettre le concepteur dans la liste des concepteur du jeu.

		//TODO: Ajouter le jeu à la liste des jeux auquel a participé le concepteur.
		if (pConcepteur == nullptr)
		{
			pConcepteur->jeuxConcus.elements = new Jeu * [20];
			pConcepteur->jeuxConcus.elements[pConcepteur->jeuxConcus.nElements++] = pJeu;
			pJeu->concepteurs.elements[i] = pConcepteur;
			pJeu->concepteurs.nElements++;
		}
	}
	pJeu->concepteurs.capacite = pJeu->concepteurs.nElements;
	return pJeu; //TODO: Retourner le pointeur vers le nouveau jeu.
}

ListeJeux creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	size_t nElements = lireUintTailleVariable(fichier);
	ListeJeux listeJeux = {};
	listeJeux.capacite = nElements;
	listeJeux.nElements = 0;
	listeJeux.elements = new Jeu * [nElements];
	for([[maybe_unused]] size_t n : iter::range(nElements))
	{
		Jeu * pJeu = lireJeu(fichier, listeJeux); //TODO: Ajouter le jeu à la ListeJeux.
		listeJeux.elements[n] = pJeu;
		listeJeux.nElements++;
	}

	return listeJeux; //TODO: Renvoyer la ListeJeux.
}


//TODO: Fonction pour détruire un concepteur (libération de mémoire allouée).
// Lorsqu'on détruit un concepteur, on affiche son nom pour fins de débogage.

void detruireConcepteur(Concepteur* concepteur) {
	
	string nom = concepteur->nom;
	delete [] concepteur->jeuxConcus.elements;
	delete concepteur;
	cout << nom << "a été détruit." << endl;
}

//TODO: Fonction qui détermine si un concepteur participe encore à un jeu.
bool concepteurParticipe(Concepteur* concepteur) {
	if (concepteur->jeuxConcus.elements != 0) {
		return true;
	}

	return false;	
}

//TODO: Fonction pour détruire un jeu (libération de mémoire allouée).
// Attention, ici il faut relâcher toute les cases mémoires occupées par un jeu.
// Par conséquent, il va falloir gérer le cas des concepteurs (un jeu contenant
// une ListeConcepteurs). On doit commencer par enlever le jeu à détruire des jeux
// qu'un concepteur a participé (jeuxConcus). Si le concepteur n'a plus de
// jeux présents dans sa liste de jeux participés, il faut le supprimer.  Pour
// fins de débogage, affichez le nom du jeu lors de sa destruction.



void detruireJeu(Jeu* jeu) {
	for (auto concepteur : spanListeConcepteurs(jeu->concepteurs)) {
		for (auto j : spanListeJeux(concepteur->jeuxConcus)) {
			if (j->titre == jeu->titre) {
				enleverJeu(concepteur->jeuxConcus, *j);
			}
		}

		enleverConcepteur(jeu->concepteurs, *concepteur);

		if (!concepteurParticipe(concepteur)) {
			detruireConcepteur(concepteur);
		}
	}

	string nomJeu = jeu->titre;
	delete[] jeu->concepteurs.elements;
	delete jeu;
	cout << nomJeu << endl;
}	

//TODO: Fonction pour détruire une ListeJeux et tous ses jeux.
void detruireListeJeux(ListeJeux& listeJeux) {
	for (auto jeu : spanListeJeux(listeJeux)) {
		detruireJeu(jeu);
	}

	delete[] listeJeux.elements;
}

void afficherConcepteur(const Concepteur& d)
{
	cout << "\t" << d.nom << ", " << d.anneeNaissance << ", " << d.pays
			  << endl;
}

//TODO: Fonction pour afficher les infos d'un jeu ainsi que ses concepteurs.
// Servez-vous de la fonction afficherConcepteur ci-dessus.

void aficherInfoJeu(Jeu * jeu) {


	cout << "titre" << jeu->titre << " Date: " << jeu->anneeSortie << " Developpeur : " << jeu->developpeur << endl;
	cout << "Concepteurs : " << endl;

	for (auto concepteur : spanListeConcepteurs(jeu->concepteurs)) {
		afficherConcepteur(*concepteur);
	}
}

//TODO: Fonction pour afficher tous les jeux de ListeJeux, séparés par un ligne.
// Servez-vous de la fonction d'affichage d'un jeu crée ci-dessus. Votre ligne
// de séparation doit être différent de celle utilisée dans le main.

void afficherTousJeux(ListeJeux* liste) {
	for (auto jeu : spanListeJeux(*liste)){
		aficherInfoJeu(jeu);
	}
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	#pragma region "Bibliothèque du cours"
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi(); 
	#pragma endregion

	int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	ListeJeux liste = creerListeJeux("jeux.bin"); //TODO: Appeler correctement votre fonction de création de la liste de jeux.

	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	cout << ligneSeparation << endl;
	cout << "Premier jeu de la liste :" << endl;
	//TODO: Afficher le premier jeu de la liste (en utilisant la fonction).  Devrait être Chrono Trigger.
	cout << liste.elements[0]->titre << endl;
	cout << ligneSeparation << endl;

	//TODO: Appel à votre fonction d'affichage de votre liste de jeux.
	afficherTousJeux(&liste);
	
	//TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.
	Developpeur dev = Developpeur("Square");
	dev.updateListeJeux(liste);
	dev.afficherListeJeux(liste);
	string nom = dev.getName();
	//TODO: Détruire tout avant de terminer le programme.  Devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	detruireListeJeux(liste);
	delete fuite;
}
