#include <stdbool.h> // bool, true, false
#include <stdlib.h> // malloc
#include <stdio.h> //scanf
#include "dedalus_explorer.h"


// Put your name below
const char * monome="Esteban Cormier";

/* Cette fonction permettait de lire un mouvement donné sur la ligne de commande et ensuite de le renvoyer. Elle n'est plus utilisée car elle servait de fonction de test au début du projet.
Move scanf_move(Move move){
  int x;
  scanf("%d",&x);
  move=x;
  return move;
}
*/
/* Cette fonction permettait de signaler quand on est dans un cul-de-sac et est devenue obsolete avec l'implémentation de la fonction always_south.
bool walk_back(bool north, bool east,bool south, bool west,int value){ 
  if(value==1 && !north && !south && !east){ // Quand on vient de l'ouest si on ne peut pas aller dans d autres direction on retourne TRUE
    return true;
  }
  else if(value==0 && !east && !north && !west){// Quand on vient du sud si on ne peut pas aller dans d autres direction on retourne TRUE
    return true;
  }
  else if(value==2 && !south && !east && !west){ // Quand on vient du nord si on ne peut pas aller dans d autres direction on retourne TRUE
    return true;
  }
  else if(value==3 && !north && !south && !west){ // Quand on vient de l'est si on ne peut pas aller dans d autres direction on retourne TRUE
    return true;
  }
  else {
    return false; // Sinon on retourne FALSE
  }
}
 */
// Cette fonction controle le mouvement de Thésée, elle fait en sorte qu'il favorise le mouvement au sud.
Move always_south(bool north,bool east,bool south, bool west, ExpTree pos,Move move,bool* boucle){
  if(*boucle==false){ // On teste le booléen si on est dans une boucle ou non
    if(pos->m == South){  // Si on vient du sud
      if(pos->east==0 && east) return East; // Si l'est n'est pas exploré et que on peut y aller on y va
      if(pos->south==0 && south) return South; // Si le sud n'est pas exploré et que on peut y aller on y va
      if(pos->west==0 && west) return West; // Si l'ouest n'est pas exploré et que on peut y aller on y va
      if(north) return North; // Sinon on va au nord
    }
    else if(pos->m == West){ 
      if(pos->south==0 && south) return South;
      if(pos->west==0 && west) return West;
      if(pos->north==0 && north) return North;
      if(east) return East;
    }
    else if(pos->m == North){ 
      if(pos->west==0 && west) return West;
      if(pos->north==0 && north) return North;
      if(pos->east==0 && east) return East;
      if(south) return South;
    }
    else if(pos->m == East ){
      if(pos->south==0 && south) return South;
      if(pos->north==0 && north) return North;
      if(pos->east==0 && east) return East;
      if(west) return West;
    }	
    else if (pos->m==None){
      if(pos->south==0 && south) return South;
      if(pos->west==0 && west) return West;
      if(pos->north==0 && north) return North;
      if(pos->east==0 && east) return East;
    }
    return None;
  }else if (*boucle==true && pos->m==None){ // Si on est dans une boucle mais qu'on est a la position de départ, on retourne None
    return None;
  }
  return (pos->m+2)%4; //Sinon on est dans une boucle et on fait demi-tour
}

// On va remplir le fil d'ariane avec le mouvement donné et préparer le fil pour le mouvement suivant
void remplir_fil_d_ariane(Move move, string* ariane){ 
  string maillon;
  maillon=(string)malloc(sizeof(string));
  maillon->m=move;
  maillon->next=*ariane;
  *ariane=maillon;
}
// Fonction de debug qui permet d'afficher le fil d'ariane récursivement
void afficher_ariane(string ariane){ 
  if(ariane!=NULL){
    printf(" - %d ", ariane->m);
    afficher_ariane(ariane->next);
  }
}

void fil_d_ariane(ExpTree map,ExpTree pos, string* ariane, bool* valide){
  if(!(*valide)){ // Si on ne s'est pas trouvé alors on continue
    if(map!=NULL){ // Si l'endroit ou on est n'est pas null 
      if(map==pos){ // On regarde si on match la map et notre position
        remplir_fil_d_ariane(map->m,ariane); // On remplit le fil d'ariane avec notre position
        *valide=true; // On passe le booléen comme quoi on s'est trouvé à TRUE
      }else{ 
        fil_d_ariane(map->south,pos,ariane,valide); // Sinon on parcours l'arbre en descendant d'abord au sud
        if ((*valide)){ // On check le booléen si on s'est trouvé ou non, et on continue sinon.
          remplir_fil_d_ariane(map->m,ariane);
        }else{
          fil_d_ariane(map->west,pos,ariane,valide);
          if ((*valide)){
            remplir_fil_d_ariane(map->m,ariane);
          }else{
            fil_d_ariane(map->north,pos,ariane,valide);
            if ((*valide)){
              remplir_fil_d_ariane(map->m,ariane);
            }else{
              fil_d_ariane(map->east,pos,ariane,valide);
              if ((*valide)){
                remplir_fil_d_ariane(map->m,ariane);
              }
            }
          }
        }
      }
    }
  }
}
//Cette procédure à pour but de retourner le sens de lecture du fil d ariane
void  retourner_ariane(string ariane, string* ariane_retournee){
  if(ariane!=NULL){
    string maillon;
    maillon=(string)malloc(sizeof(string));
    maillon->m=ariane->m;
    maillon->next=*ariane_retournee;
    *ariane_retournee=maillon;
    retourner_ariane(ariane->next,ariane_retournee);
  }
}
// Cette procédure detecte si on est dans un boucle grace aux nombres de nord sud est west dans le fil d'ariane
void detecteur_boucle(string ariane,int* x,int* y,bool* boucle){
  if(ariane!=NULL){ // Si le fil n'est pas null
    if(ariane->m==North){ // Si cest un nord on ajoute 1 à x
      *x+=1;
    }else if(ariane->m==East){ // Si cest un est on ajoute 1 à y
      *y+=1;
    }else if(ariane->m==South){ // Si cest un sud on enleve 1 à x
      *x-=1;
    }else if(ariane->m==West){ // Si cest un west on enleve 1 à x
      *y-=1;
    }
    if(*x==0 && *y==0){ // Si x et y valent 0 alors on est dans une boucle
      *boucle=true;
    }else{
      detecteur_boucle(ariane->next,x,y,boucle); // On rappelle la procédure pour passer à l'élément suivant dans le fil d'ariane
    }
  }
}

Move theseus(
    ExpTree map, //current exploration tree
    ExpTree pos, // current position in the map exploration tree
    bool north, // can i go North?
    bool east, // can i go East?
    bool south, // can i go South?
    bool west // can i go West?
) {
  Move move=4; // Mouvement que l'on va retourner à la fin de la fonction.
  bool valide=false; // Le booleen qui va servir pour controler le fil d'ariane
  string filariane=NULL; // Initialisation du fil d'ariane
  string r_filariane=NULL; // Initialisation du fil d'ariane retourné
  int x,y; // x représente le nombre de coups vers le nord ou vers le sud, y représente le nombre de coup vers l'est ou l'ouest
  bool boucle=false; // Booléen qui signale si l'on se trouve dans une boucle ou non
  fil_d_ariane(map,pos,&filariane,&valide); // On appelle la procédures qui lance le fil d'ariane
  //afficher_ariane(filariane); On appelle la fonction qui affiche le fil d'ariane
  retourner_ariane(filariane,&r_filariane); //  On appelle la procédure qui retourne le fil d'ariane
  detecteur_boucle(r_filariane,&x,&y,&boucle); //  On appelle la procédure qui detecte les boucles 

  /* Ces lignes servaient au début du projet a tester si l'on était dans un cul de sac ou non
  bool cds=false;
  cds=walk_back(north,east,south,west,pos->m);
  if(cds==true){
    move=(pos->m+2)%4; 
  }
  else{*/
  
    move=always_south(north,east,south,west,pos,move,&boucle); // On appelle la fonction de mouvement de Thésée
  //}
  // my chosen move is:
  return move;
}



