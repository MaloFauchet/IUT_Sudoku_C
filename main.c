/**
 * \brief Programme de jeu de sudoku
 * 
 * \author FAUCHET Malo
 * \version 1.0
 * \date 2023-11-26
 * 
 * Ce programme permet de jouer au sudoku en selectionnant une
 * grille au lancement du programme. Le programme s arrete lorsque
 * la grille est pleine.
*/


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


/**
 * \def CELLULE_VIDE
 * \brief Caractere representant une cellule vide
*/
const char CELLULE_VIDE =  '.' ;

/**
 * \def n
 * \brief Taille d un bloc de la grille
*/
#define n 3

/**
 * \def TAILLE
 * \brief Taille de la grille (n*n)
*/
#define TAILLE n*n

/**
 * \typedef tGrille
 * \brief Tableau a deux dimensions de taille TAILLE
 *
 * Le type tGrille est un tableau a deux dimensions servant a
 * stocker les valeurs de la grille de jeu du sudoku.
 */
typedef int tGrille[TAILLE][TAILLE];

/**
 * \typedef tRetourEnArriere
 * \brief Tableau de taille TAILLE contenant les positions des coups réalisé par le joueur
*/
typedef int tRetourEnArriere[TAILLE];

void chargerGrille(tGrille grille);
void genererNbAleatoire(int *num, int maximum);
void afficherGrille(tGrille grille);
int nombreChiffre(int nombre);
void afficherEspaces(int nombreEspaces);
void afficherLigneSeparatrice(int nombre_espaces);
void saisir(int *valeur, int minimum, int maximum);
bool possible(tGrille grille, int numLigne, int numColonne, int valeur);
bool grilleEstPleine(tGrille grille);
void saisirChoix(char *valeur);


/*****************************************************
 *                PROGRAMME PRINCIPAL                *
 *****************************************************/

/**
 * \fn int main()
 * \brief Programme principal
 * \return Code de sortie du programme (0: sortie normale).
 * 
 * Le programme principal permet de jouer une partie de sudoku en selectionnant
 * une grille au lancement du programme. Le programme s arrete lorsque
 * la grille est pleine.
*/
int main() {
    tGrille grille1;  // grille de jeu
    tRetourEnArriere pile_retour;  // pile contenant les positions jouées par le joueur
    int nb_retour;  // contient l'indice du dernier x (ou numLigne) de pile_retour (x = nb_retour, y = nb_retour + 1)
    int numLigne, numColonne, valeur;  // entrées de l'utilisateur
    char choix_ajout_arriere[2];  // contient le choix fait par l utilisateur

    nb_retour = -2;
    

    /** Charge la grille et check si celui-ci s est bien passe. Sinon, arret du programme avec le code 1 */
    chargerGrille(grille1);
    
    system("clear"); // Efface ce qui est affiche dans le terminal

    /** Boucle principale */
    while (grilleEstPleine(grille1) == false) {
        afficherGrille(grille1);

        // saisie du choix
        saisirChoix(choix_ajout_arriere);

        // si utilisateur veut ajouter une valeur
        if (!strcmp(choix_ajout_arriere, "A")) {
            printf("Saisie de la case... \n");
            printf("Numero de ligne ? ");
            saisir(&numLigne, 1, TAILLE);
            printf("Numero de colonne ? ");
            saisir(&numColonne, 1, TAILLE);

            numColonne -= 1; // Permet de retrouver les bons indices afin d acceder a la bonne case.
            numLigne -= 1;   // Entre 0 et 8 au lieu de 1 et 9.

            // Check si la case selectionnee est libre
            if (grille1[numLigne][numColonne] != 0) {
                system("clear");
                printf("\nIMPOSSIBLE, la case sélectionnée n'est pas libre\n\n");
            } else {
                printf("Valeur à inserer (entre 1 et %d) : ", TAILLE);
                saisir(&valeur, 1, TAILLE);
                system("clear");
                if (possible(grille1, numLigne, numColonne, valeur) == true) {
                    grille1[numLigne][numColonne] = valeur;

                    nb_retour += 2;
                    pile_retour[nb_retour] = numLigne;
                    pile_retour[nb_retour+1] = numColonne;
                }
            }
        } // sinon l utilisateur veut et peut supprimer son dernier coup
        else  if (nb_retour != -2) {
            grille1[pile_retour[nb_retour]][pile_retour[nb_retour+1]] = 0;
            nb_retour -= 2;
            system("clear");
        } // sinon si l utilisateur ne peut pas supprimer son dernier coup (deja sup ou premier coup)
        else {
            system("clear");
        }
    }
    afficherGrille(grille1);
    printf("\nGrille pleine, fin de partie\n");
    
    return EXIT_SUCCESS;
}


/*****************************************************
 *                     FONCTIONS                     *
 *****************************************************/


/**
 * \fn void chargerGrille(tGrille grille)
 * \brief Charge une grille de jeu a partir d un fichier
 * \param grille Grille de jeu a initialiser
 * 
 * La fonction charge une grille de jeu a partir d un fichier
 * dont le nom est saisi au clavier.
 * Cette fonction arrete le programme avec le code de sortie 1 
 * si le dossier 'grilles/' n est pas present ou si le fichier n est pas trouve
*/
void chargerGrille(tGrille grille){
    FILE * f;
    char nomFichier[30];  /** \var nomFichier chaine de caracteres contenant le nom du fichier de grille a initialiser */
    int numFic;  /** \var numFic le numero du fichier de grille a initialiser */

    // test si le dossier ./grilles est present
    if (access("./grilles", 0) == 0) {
        
        // choix du fichier a utiliser afin d initialiser la grille de jeu
        printf("Choisissez un numéro de grille entre 1 et 10 (0 si vous voulez laisser l'aléatoire décider) : ");
        saisir(&numFic, 0, 10);

        // Si l utilisateur a choisi 0, le fichier sera choisi aleatoirement
        if (numFic == 0)
            genererNbAleatoire(&numFic, 10);

        sprintf(nomFichier, "grilles/Grille%d.sud", numFic);

        // utilisation du fichier
        f = fopen(nomFichier, "rb");
        if (f==NULL){
            printf("ERREUR lors du chargement du fichier.\n");
            printf("\tAssurez-vous que le dossier \'grilles/\' soit présent au même endroit que le programme,\n");
            printf("\tet qu'il contienne les fichier \'Grille_.sud\', où \'_\' est un nombre entre de 1 à 10.\n");
            exit(EXIT_FAILURE);
        } else {
            fread(grille, sizeof(int), TAILLE*TAILLE, f);
        }
        fclose(f);
    } 
    else {
        printf("ERREUR. Le dossier \'grilles/\' est introuvable.\n");
        printf("\tAssurez-vous que le dossier \'grilles/\' soit présent au même endroit que le programme,\n");
        printf("\tet qu'il contienne les fichier \'Grille_.sud\', où \'_\' est un nombre entre de 1 à 10.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * \fn void genererNbAleatoire(int *num, int maximum)
 * \brief Genere un nombre aleatoire entre 1 et maximum (inclus)
 * \param num la variable dans laquelle stocker le nombre aleatoire
 * \param maximum borne maximale pour la generation du nombre aleatoire
*/
void genererNbAleatoire(int *num, int maximum) {
    srand(time(NULL));
    *num = rand() % maximum+1;
}

/**
 * \fn void afficherGrille(tGrille grille)
 * \brief Affiche la grille de jeu de maniere lisible en fonction de TAILLE
 * \param grille Grille de jeu a afficher
 * 
 * Cette fonction affiche la grille de jeu de maniere lisible en
 * affichant les numeros des lignes et des colonnes, ainsi que les
 * lignes et colonnes de separation entre les blocs de la grille.
 * Les cellules vides sont representees par un point.
 * La taille de la grille est adaptee en fonction de TAILLE.
*/
void afficherGrille(tGrille grille) {
    int i, j, num_espaces; 

    printf("\n");

    // determine le nombre de chiffres dans le nombre le plus grand de la grille
    num_espaces = nombreChiffre(TAILLE) + 1; /** \var nombre d espaces avant le debut de la grille */

    // numeros des colonnes
    afficherEspaces(num_espaces+1);
    for (i = 0; i < TAILLE; i++) {
        // check si i a atteint la fin d une region
        if ((i%n == 0) && (i != 0)){
            printf("  ");
        }
        printf("%3d", i+1);
    }
    printf("\n");

    //affichage de la premiere ligne de separation
    afficherLigneSeparatrice(num_espaces);


    // corps de la grille + cote gauche
    for (i=0; i < TAILLE; i++) {
        //check si i a atteint la fin d une region
        if ((i%n == 0) && (i != 0)){
            afficherLigneSeparatrice(num_espaces);
        }

        // numeros des lignes
        printf("%d", i+1);
        afficherEspaces(num_espaces - nombreChiffre(i+1));
        printf("%c", '|');

        for (j=0; j < TAILLE; j++) {
            // check si j a atteint la fin d une region
            if ((j%n == 0) && (j != 0)){
                printf("%2c",  '|' );
            }

            // affiche '.' au lieu de 0 pour les cellules vides
            if (grille[i][j] == 0) {
                printf("%3c", CELLULE_VIDE);
            } else {
                printf("%3d", grille[i][j]);
            }
        }
        printf("%2c",  '|' );
        printf("\n");
    }
    // derniere ligne
    afficherLigneSeparatrice(num_espaces);
}

/**
 * \fn int nombreChiffre(int nombre)
 * \brief Determine le nombre de chiffres d un nombre
 * \param nombre Nombre dont on veut connaitre le nombre de chiffres
 * 
 * \return Nombre de chiffres du nombre
 * 
 * Cette fonction calcule le nombre de chiffres d un nombre en
 * divisant le nombre par 10 jusqu a ce que le nombre soit egal a 0.
*/
int nombreChiffre(int nombre) {
    int nombre_chiffre_tmp = 0;
    while (nombre != 0) {
        nombre /= 10;
        nombre_chiffre_tmp++;
    }
    return nombre_chiffre_tmp;
}

/**
 * \fn void afficherEspaces(int nombre_espaces)
 * \brief Affiche un nombre d espaces
 * \param nombre_espaces Nombre d espaces a afficher
*/
void afficherEspaces(int nombre_espaces) {
    for (int i=0; i < nombre_espaces; i++) {
        printf(" ");
    }
}

/**
 * \fn void afficherLigneSeparatrice(nombre_espaces)
 * \brief Affiche une ligne separatrice
 * \param nombre_espaces Nombre d espaces a afficher
 * 
 * Affiche une ligne separatrice dont la taille est adaptative en fonction de la taille de la grille.
 * Taille minimum de la grille : 1
*/
void afficherLigneSeparatrice(int nombre_espaces) {
    afficherEspaces(nombre_espaces);
    for (int i=0; i < n; i++) {
        printf("+----");
        for (int j=0; j < n-1; j++) {
            printf("---");
        }
    }
    printf("+\n");
}


/**
 * \fn void saisir(int *valeur, int minimum, int maximum)
 * \brief Saisie securisee d une valeur
 * \param valeur variable dans laquelle stocker la valeur saisie
 * \param minimum variable contenant la valeur minimum (incluse) a saisir
 * \param maximum variable contenant la valeur maximum (incluse) a saisir
 * 
 * La fonction permet de lire au clavier une valeur. 
 * La valeur lue doit etre un entier compris entre 'minimum' et 'maximum'.
 * La saisie se repete tant que la valeur n est pas valide.
*/
void saisir(int *valeur, int minimum, int maximum){
    char ch[10];  /** \var variable temporaire, comprend l input a convertir en entier*/
    scanf(" %s", ch);
    while (!sscanf(ch, " %d", valeur) || (*valeur < minimum || *valeur > maximum)) {
        printf("Erreur, la valeur doit être un entier compris entre %d et %d inclus.\nVeuillez réessayer : ", minimum, maximum);
        scanf(" %s", ch);
    }
}

/**
 * \fn bool possible(tGrille grille, int numLigne, int numColonne, int valeur)
 * \brief Verifie si une valeur peut etre inseree dans une case
 * \param grille Grille de jeu
 * \param numLigne Numero de la ligne de la case selectionnee
 * \param numColonne Numero de la colonne de la case selectionnee
 * \param valeur Valeur a inserer dans la case selectionnee
 * 
 * \return true si la valeur peut etre inseree, false sinon
 * 
 * Cette fonction verifie si la valeur peut etre inseree dans la case
 * selectionnee en verifiant si la valeur n est pas deja presente dans
 * la ligne, la colonne ou le bloc de la case selectionnee.
*/
bool possible(tGrille grille, int numLigne, int numColonne, int valeur) {
    int i, j;
    bool possible = true;
    
    // check colonne
    i = 0;
    while (i < TAILLE && possible) {
        if (grille[numLigne][i] == valeur) {
            printf("La valeur %d ne peut pas être placée dans la ligne %d\ncar elle est déjà présente dans la même ligne à la colonne %d\n", valeur, numLigne+1, i+1);
            possible = false;
        }
        i++;
    }

    // check ligne
    i = 0;
    while (i < TAILLE && possible) {
        if (grille[i][numColonne] == valeur) {
            printf("La valeur %d ne peut pas être placée dans la colonne %d\ncar elle est déjà présente dans la même colonne à la ligne %d\n", valeur, numColonne+1, i+1);
            possible = false;
        }
        i++;
    }

    // check region
    int startingRow = numLigne - (numLigne%n);
    int startingCol = numColonne - (numColonne%n);

    i = startingRow;
    while (i < startingRow + n && possible) {
        j = startingCol;
        while (j < startingCol + n && possible) {
            if (grille[i][j] == valeur) {
                printf("La valeur %d ne peut pas être placée dans cette région\ncar elle est déjà présente dans la même région\n", valeur);
                possible = false;
            }
            j++;
        }
        i++;
    }    
    return possible;
}

/**
 * \fn bool grilleEstPleine(tGrille grille)
 * \brief Verifie si la grille est pleine
 * \param grille Grille de jeu
 * 
 * \return true si la grille est pleine, false sinon
 * 
 * Cette fonction verifie si la grille est pleine en verifiant si
 * toutes les cases sont remplies.
*/
bool grilleEstPleine(tGrille grille) {
    int i, j;
    bool est_possible = true;

    i = 0;
    while (i < TAILLE && est_possible == true) {
        j = 0;
        while (j < TAILLE && est_possible == true) {
            if (grille[i][j] == 0) {
                est_possible = false;
            }
            j++;
        }
        i++;
    }    
    return est_possible;
}

void saisirChoix(char *valeur) {
    printf("Action (A ou Z) ? ");
    scanf(" %s", valeur);
    while (strcmp(valeur, "A") && strcmp(valeur, "Z")) {
        printf("Erreur, la valeur doit être 'A' ou 'Z'.\nVeuillez réessayer : ");
        scanf(" %s", valeur);
    }
}