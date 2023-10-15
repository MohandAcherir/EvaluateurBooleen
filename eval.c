#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Structures de données
enum token_type{
    TOKEN_CONST,	/** Constante 0 ou 1 */
    TOKEN_OP,  		/** Opérateur + . => <=> NON*/
    TOKEN_PAR		/** Parenthèses ( ou )*/
};

struct token{
	enum token_type type;
	union{
		int value;		/** Si le type de \a est TOKEN_CONST */
		char op[4];		/** Si le type de \a est TOKEN_OP */
		char par;		/** Si le type de \a est TOKEN_PAR */
	} content;
	struct token *next;
};
typedef struct token *liste_token;

struct noeud{
	liste_token elt;
		 	
	struct noeud* left; 
	struct noeud* right;
	struct noeud* middle;	/** Fils unique dans le cas d'un NON */

	struct token* prevs;	/** Liste de token précédent l'opérateur courant */
	struct token* nexts;	/** Liste de token suivant l'opérateur courant */
};
typedef struct noeud *arbre_token;

//Question 1
/** Creer un token */	
liste_token creer_token(int type, char *op, char par, int value){
	liste_token lt = malloc(sizeof(struct token));
	if(!lt){
        fprintf(stderr, "ERREUR : sur l'allocation du struct token");
        return NULL;
	}
	lt->type = type;
	if(type == TOKEN_CONST) 
		lt->content.value = value;
	else if(type == TOKEN_OP) 
		strcpy(lt->content.op,op);
	else if(type == TOKEN_PAR) 
		lt->content.par = par;
	lt->next = NULL;
	return lt;
}

/** Ajout d'un token dans le champs next */
liste_token ajouter_fin_liste(liste_token current, liste_token add){
	//Cas liste vide
	if (current==NULL) return add;
	//Cas général
	liste_token deb = current;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = add;
	return deb;
}

/** Creation de la liste en fonction du type de token */
liste_token creer_liste(char *string){
	liste_token suiv;
	liste_token lt = NULL;
	int i = 0;
	while(string[i] != '\0'){
		switch(string[i]){
			case '0' : 
				suiv = creer_token(TOKEN_CONST,NULL,'\0',0);
				lt = ajouter_fin_liste(lt,suiv);
				i++;
				break;
			case '1' : 
				suiv = creer_token(TOKEN_CONST,NULL,'\0',1);
				lt = ajouter_fin_liste(lt,suiv);
				i++;
				break;
			case '+' : 
				suiv = creer_token(TOKEN_OP,"+",'\0',-1);
				lt = ajouter_fin_liste(lt,suiv);
				i++;
				break;
			case '.' : 
				suiv = creer_token(TOKEN_OP,".",'\0',-1);
				lt = ajouter_fin_liste(lt,suiv);
				i++;
				break;
			case '=' :
				if(string[i+1] == '>'){ 
				suiv = creer_token(TOKEN_OP,"=>",'\0',-1);
				lt = ajouter_fin_liste(lt,suiv);
				i=i+2;
				}
				else return NULL;
				break;
			case '<' : 
				if(string[i+1] == '=' && string[i+2] == '>'){ 
				suiv = creer_token(TOKEN_OP,"<=>",'\0',-1);
				lt = ajouter_fin_liste(lt,suiv);
				i=i+3;
				}
				else return NULL;
				break;
			case 'N' :
				if(string[i+1] == 'O' && string[i+2] == 'N'){ 
				suiv = creer_token(TOKEN_OP,"NON",'\0',-1);
				lt = ajouter_fin_liste(lt,suiv);
				i=i+3;
				}
				else return NULL;
				break;
			case '(' : 
				suiv = creer_token(TOKEN_PAR,NULL,'(',-1);
				lt = ajouter_fin_liste(lt,suiv);
				i++;
				break;
			case ')' : 
				suiv = creer_token(TOKEN_PAR,NULL,')',-1);
				lt = ajouter_fin_liste(lt,suiv);
				i++;
				break;
			default :
				return NULL;
			}
		}
		return lt;
}

/** Suppression des espaces de la chaine de caractère et création de la liste de token */
liste_token string_to_token (char *string){
	liste_token lt;
	//On retire les espaces de la chaine de caractère
	int taille = strlen(string);
	char copy[taille];
	int j = 0;
	for(int i=0; i < strlen(string); i++){
		if(string[i] != ' '){
			copy[j] = string[i];
			j++;
		}
		copy[j] = '\0';
	}
	strcpy(string,copy);
	
	//On crée la liste de token
	lt = creer_liste(string);
		
	return lt;
}

/** Affichage de la liste pour vérifier */
void affiche_liste (liste_token lt) {
	printf("LISTE : ");
	while(lt != NULL){
		if (lt->type == TOKEN_PAR) printf("%c",lt->content.par);
		if (lt->type == TOKEN_OP)    printf("%s",lt->content.op);
		if (lt->type == TOKEN_CONST)   printf("%d",lt->content.value);
		lt = lt->next;
	}
	printf("\n");
}

/** Libère la mémoire allouée pour la liste */
void libere_liste(liste_token lt){
	if(!lt) return;
	libere_liste(lt->next);
	free(lt);
}

//Question 3
/** Vérifie si l'expression appartient au langage, retourne 1 si oui, 0 sinon */
int est_valide(liste_token lt){
    int compt_par = 0;
   
    //La condition de l'état initial
    if(lt->content.par != '(' && lt->type != TOKEN_CONST && strcmp(lt->content.op,"NON"))
        return 0;

    while(lt != NULL){
        if(lt->content.par == '(' || lt->type == TOKEN_OP){
            //l'exp finit par '(' ou un op, '(' ou un op n'est pas suivie d'une constante
            //(0 ou 1), '(' ou un op n'est pas suivie d'une autre '(', '(' ou un op n'est
            //pas suivie de l'operateur "NON"
            if(!lt->next || (lt->next->type != TOKEN_CONST &&
            lt->next->content.par != '(' && strcmp(lt->next->content.op,"NON")))
                return 0;
            if(lt->content.par == '(')
                compt_par++;
        }

        //La condition de l'état final
        if(lt->content.par == ')' || lt->type == TOKEN_CONST){
            //')' ou une constane est suivie d'une constante,
            //')' ou une constante est suivie de l'opérateur "NON"
            if(lt->next && (lt->next->type == TOKEN_CONST ||
            lt->next->content.par == '(' || !strcmp(lt->next->content.op, "NON"))){
                return 0;
            }
            if(lt->content.par == ')')
                compt_par--;
        }

        //Pour vérifier que les parenthèses sont dans le bon ordre
        if(compt_par < 0)
            return 0;

        lt = lt->next;
    }

    return ((compt_par == 0)?1:0);
}

//Question 4
/** Copie un token src dans un token dest*/
void copy(liste_token dest, liste_token src){
	dest->type = src->type;

	switch(dest->type){
		case TOKEN_CONST : dest->content.value = src->content.value;
				   break;
		case TOKEN_OP : strcpy(dest->content.op, src->content.op);
				   break;
		case TOKEN_PAR : dest->content.par = src->content.par;
		 		   break;
	      	default : return;			 
	}
}

/** Retire le premier et le dernier token de la liste en argument */
liste_token remove_token(liste_token exp, int deb){
	if(!exp || !exp->next) return NULL;
	
	int size = 1;

	liste_token new = malloc(sizeof(struct token));
	liste_token begin = exp;
	liste_token head = new;

	while(exp->next){
		size++;
		exp = exp->next;
	}
	size--;

	exp = begin->next;

	for(int i = 0 ; i < size-deb-1 ; i++){
		if(exp) copy(new, exp);
		
		if(i+1 < size-1-deb){
			new->next = malloc(sizeof(struct token));
			new->next->next = NULL;
			exp = exp->next;
			new = new->next;
		}
	}
	return head;
}

/** Recherche l'opérateur le moins prioritaire */
liste_token chrch_opp(liste_token lt, int nb){
	liste_token opp = NULL;
	liste_token curr = lt;
	int entre_par = 0, par_inutiles = nb;
	int newSize = 0;
	
	//On calcule la taille de l'expression
	while(curr){
		newSize++;
		curr = curr->next;
	}
	
	curr = lt;

	while(nb > 0){
		curr = curr->next;
		nb--;
	}

	//On parcourt la taille d'une expression entre parenthèses inultiles 
	//(ex : ((1.0)) -> taille : 3)
	int i = newSize - par_inutiles - 1;

	while(curr && i > 0){
		if(curr->type == TOKEN_OP && (entre_par == 0)){
			//L'opérateur le moins prioritaire est le dernier lu sauf dans le cas du NON
			if(opp == NULL || strcmp(curr->content.op, "NON")){
				opp = curr;
			}
		}

		else if(curr->type == TOKEN_PAR){
			//Si l'opérateur est entre parenthèse, il est prioritaire
			if(curr->content.par == '('){
				entre_par++;
			}
			else{
				entre_par--;
			}
		}
		curr = curr->next;
		i--;
	}
	
	return opp;
}


/** Si une constante est entourée de parenthèses, on cherche à atteindre directement 
le token de la constante*/
liste_token get_number(liste_token lt){
	while(lt->type != TOKEN_CONST){
		lt = lt->next;
	}
	
	return lt;
}

/** Libère la mémoire allouée pour l'arbre */
void libere_arbre(arbre_token root){
	if(root == NULL)
		return;

	libere_arbre(root->left);
	libere_arbre(root->right);
	libere_arbre(root->middle);

	libere_liste(root->prevs);
	libere_liste(root->nexts);

	free(root);
}

/** Récupère la partie de la liste précédant l'opérateur courant */
liste_token get_prev(liste_token lt, liste_token t, int nb){
	liste_token head = malloc(sizeof(struct token));
	head->next = NULL;
	liste_token ret = head;

	while(nb > 0){
		lt = lt->next;
		nb--;
	}

	while(lt != t){
		copy(ret, lt);
		if(lt->next != t){
			ret->next = malloc(sizeof(struct token));
			ret->next->next = NULL;
			ret = ret->next;
		}	
		lt = lt->next;
	}

	return head;
}

/** Récupère la partie de la liste suivant l'opérateur courant */
liste_token get_next(liste_token lt, liste_token t, int nb){
	int size = 0;
	if(!t) return NULL;
	
	while(lt != t)
		lt = lt->next;
	lt = lt->next;
	
	if(lt)
		size = 1;
	else
		return NULL;

	liste_token begin = lt;
	liste_token head = malloc(sizeof(struct token));
	head->next = NULL;
	liste_token ret = head;

	while(lt->next){
		lt = lt->next;
		size++;
	}

	lt = begin;
	int i = size - nb - 1;

	while(i >= 0){
		copy(ret, lt);
		
		if(i-1>=0){
			ret->next = malloc(sizeof(struct token));
			ret->next->next = NULL;
			ret = ret->next;
		}
		
		lt = lt->next;
		i--;
	}
	return head;
}

/** Crée l'arbre correspondant à la liste de token */
arbre_token creer_arbre(liste_token lt){
	if(!lt) return NULL;
	
	int nb = 0;
	liste_token r = lt;

	//On compte le nombre de parenthèses inutiles
	while(r && r->type == TOKEN_PAR && r->content.par == '('){
		liste_token tmp = remove_token(r, nb);

		//Tant que récursive, l'expréssion entre le token de début et le token de fin
		//est valide, on compte un couple de parenthèses inutiles
		if(est_valide(tmp)){
			nb++;
			r = r->next;
			libere_liste(tmp);
		}
		else{
			libere_liste(tmp);
			break;
		}	
	}

	//on stocke l'opérateur qui sera racine d'un arbre
	liste_token t = chrch_opp(lt, nb);

	//s'il ne reste pas d'opérateur, le token est stocké dans une feuille est une constante
	if(t == NULL){
		arbre_token new = malloc(sizeof(struct noeud));
		new->elt = get_number(lt);
		new->left = NULL;
		new->right = NULL;
		new->middle = NULL;
		new->prevs = NULL;
		new->nexts = NULL;

		return new;
	}

	//sinon on stocke l'opérateur, et les expressions à gauches et à droites de 
	//l'opérateur, dans les champs correspondants du noeud 
	liste_token prev = get_prev(lt, t, nb);
	liste_token next = get_next(lt, t, nb);

	arbre_token root = malloc(sizeof(struct noeud));

	root->elt = t;
	root->prevs = prev;
	root->nexts = next;

	if(!strcmp(t->content.op, "NON")){
		root->middle = creer_arbre(next);
		root->left = NULL;
		root->right = NULL;
	}
	else{
		//On effectue ce processus récursivement jusqu'à la fin de la liste de token
		root->left = creer_arbre(prev);
		root->right = creer_arbre(next);
		root->middle = NULL;
	}

	return root;
}

//Question 5
/** Evalue l'expression en remontant dans l'arbre */
int arbre_to_int(arbre_token at) {
    if (at==NULL) return 0;
    if (at->elt->type == TOKEN_CONST) return at->elt->content.value;

    int valg = arbre_to_int(at->left);
    int vald = arbre_to_int(at->right);
    int valm = arbre_to_int(at->middle);
    
    if(!strcmp(at->elt->content.op,"+"))
        return valg | vald;
    if(!strcmp(at->elt->content.op,"NON"))
        return 1 - valm;
    if(!strcmp(at->elt->content.op,"."))
        return valg & vald;
    if(!strcmp(at->elt->content.op,"=>")){
        if(valg == 1 && vald == 0)
            return 0;
        else
            return 1;
    }
    if(!strcmp(at->elt->content.op,"<=>")){
        if(valg != vald)
            return 0;
        else
            return 1;
    }
    
    fprintf(stderr,"Un mauvais opérateur est passé entre les mailles du filets\n");
    exit(15);
    return 0;
}

/** Affiche le parcours infixe de l'arbre */
void affiche_arbre(arbre_token root){
	if(root == NULL)
		return;

	affiche_arbre(root->left);
	affiche_arbre(root->middle);
	affiche_arbre(root->right);
	
	if(root->elt->type == TOKEN_CONST)
		printf("%d", root->elt->content.value);
	if(root->elt->type == TOKEN_OP)
		printf("%s", root->elt->content.op);
	if(root->elt->type == TOKEN_PAR)
		printf("%c", root->elt->content.par);
}

int main(int argc, char **argv){
	
	if(argc != 2){
		fprintf(stderr,
                "ERREUR usage : %s exp\n"
                "%d exp -- expression booléenne à évaluer\n",
                argv[0],argc);
		return 1;
	}
	
	liste_token lt = string_to_token(argv[1]);
	if(lt) 
		affiche_liste(lt);
	else{
		printf("Expression incorrecte : mauvais alphabet, pas de liste\n");
		return 1;
	}

	if(est_valide(lt)){
		arbre_token root = creer_arbre(lt);
		printf("ARBRE postfixe : ");
		affiche_arbre(root);
		printf("\n");
	
		int ans = arbre_to_int(root);
		if(ans == 0) printf("FAUX\n");
		else if(ans == 1) printf("VRAI\n");

		libere_arbre(root);
	}
	else
		printf("Expression incorrecte\n");
		
	libere_liste(lt);
	printf("\n");
	
	return 0;
}
