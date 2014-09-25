#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**************************************************************************************/
/*					DICHIARAZIONE DELLE STRUTTURE E DEI PROTOTIPI          					  */
/**************************************************************************************/

/*****************************************
* Definisco la struttura orario e i suoi prototipi
****************************************/

struct orario{
	int ora;
	int minuti;
};
typedef struct orario *Orario;

Orario new_orario(int hh, int mm);
int cmp_orario(Orario f, Orario s);

/*definisco il prototipo di una lista*/
typedef struct lista *Lista;

/*****************************************
* Definisco la struttura testimonianza e i suoi prototipi
******************************************/
struct testimonianza{
	char *testimone;
	Orario arrivo;
	Orario partenza;
	Lista incontri;
	int visitato;/*questo campo serve per la visita in profondita dei grafi*/
};
typedef struct testimonianza *Testimonianza;

/*definisco una stringa come chiave della struttura testimone*/
typedef char* Chiave;
	
Testimonianza new_testimonianza(Chiave n, Orario a, Orario p);
Chiave key(Testimonianza t);
void destroy_testimonianza(Testimonianza t);
int cmp_key(Chiave k1, Chiave k2);

/*****************************************
* Definisco la struttura lista e i suoi prototipi
*****************************************/
struct lista{
	Testimonianza test;
	struct lista *next;
};

Lista add_to_list(Lista l, Testimonianza t);
Lista delete_node(Lista l, Testimonianza t);
int length_list(Lista l);
void destroy_list(Lista l);
void print_list(Lista h);

/*****************************************
* Definisco la struttura Bit_node e i suoi prototipi
****************************************/

struct bit_node{
	Testimonianza test;
	struct bit_node *l,*r;
};
typedef struct bit_node *Bit_node;

/*definisco una variabile che conterrà il numero di nodi presenti nell'albero*/
int num_nodi=0;

Bit_node bit_new(Testimonianza test);
void bit_delete(Bit_node p); 
void bit_destroy(Bit_node p); 
Testimonianza bist_search(Bit_node r, Chiave k);
void bist_insert(Bit_node *r, Testimonianza test);
void bist_delete(Bit_node *r, Chiave k);

/*****************************************
* Definisco la struttura pqueue e i suoi prototipi
*****************************************/
typedef Testimonianza *Heap;

struct pqueue {
	Heap h;
	int dimensione,valore;
};

typedef struct pqueue *Pqueue ;

/*definisco un flag. quando flag è 0 l'inserimento nella coda sarà fatto usando come chiave l'ora di partenza.
quando flag è 1 la chiave sarà invece l'ora di arrivo*/
int flag = 0; 

Pqueue pqueue_new ( int n );
void pqueue_destroy ( Pqueue H);
int pqueue_length ( Pqueue H);
void pqueue_insert ( Pqueue H, Testimonianza t);
Testimonianza pqueue_extractmin ( Pqueue H);
Testimonianza pqueue_min ( Pqueue H);
int father(int i);
int left(int i);
int right(int i);
Orario key_pqueue(Testimonianza t);
Orario key_pqueue2(Testimonianza t);
void swap(Heap h, int i, int j);
void heapify_up(Heap h, int i);
void heapify_down(Heap h, int i, int n);


/*****************************************
* Definisco i prototipi per la costruzione del grafo
*****************************************/

void insert_graph(Bit_node p,Testimonianza t);
void delete_graph(Bit_node p,Testimonianza t);
void visit_graph(Testimonianza t);

/**************************************************************************************/
/*					DICHIARAZIONE DELLE FUNZIONI														  */
/**************************************************************************************/

/*****************************************
* Definisco le procedure per la struttura orario
*****************************************/
/*restituisce un puntatore a una nuova struttura orario*/
Orario new_orario(int hh, int mm){
	Orario new;
	new=malloc(sizeof(struct orario));
	if(new==NULL){
		fprintf (stderr, "Errore di allocazione nella funzione new_orario\n");
		exit(-1);
	}	
	if((hh >24 && hh >0)||(mm>60 && mm<0))
		printf("errore: ora non consentita"); 
	new->ora = hh;
	new->minuti = mm;
	return new;
}

/*restituisce 1,0,-1 a seconda che f sia >,= o < di s*/
int cmp_orario(Orario f, Orario s){
	int i;
	if(f->ora > s->ora)
		i=1;
	else if (f->ora < s->ora)
		i=-1;
	else if(f->ora==s->ora){
		if(f->minuti>s->minuti)
			i=1;
		else if(f->minuti<s->minuti)
			i=-1;
		else if(f->minuti==s->minuti)
			i=0;
	}
	return i;
}		
		
/*****************************************
/ Definisco le procedure per la struttura testimonianza
*****************************************/
/*restituisce un puntatore a una nuova struttura testimonianza*/
Testimonianza new_testimonianza(Chiave n, Orario a, Orario p){
	int i;
	Testimonianza new;
	new = malloc(sizeof(struct testimonianza));
	if(new==NULL){
		fprintf (stderr, "Errore di allocazione nella funzione new_testimonianza\n");
		exit(-1);
	}
	new->testimone=malloc((strlen(n)+1)*sizeof(char));
	if (new->testimone==NULL){
		fprintf (stderr, "Errore di allocazione nella funzione new_testimonianza\n");
		exit(-2);
	}	
	for(i=0;isalpha(n[i]);i++)	/*copio la stringa*/
		new->testimone[i]=n[i];
	new->testimone[i]='\0';			/*metto come ultimo carattere il carattere fine stringa*/
	new->arrivo=a;
	new->partenza=p;
	new->incontri=NULL;
	new->visitato=0;
	return new;
}

/*distrugge una testimonianza*/
void destroy_testimonianza(Testimonianza t){
	free(t->testimone);
	destroy_list(t->incontri);
	t->incontri=NULL;
	free(t);
}

/*restituisce la chiave di testimonianza ovvero il nome del testimone*/
Chiave key(Testimonianza t){
	return t->testimone;
}

/*restituisce 1,0,-1 a seconda che k1 sia >,= o < di k2*/
int cmp_key(Chiave k1, Chiave k2){
	return strcmp(k1,k2);
}

/*****************************************
* Definisco le procedure per la struttura lista
*****************************************/
/*aggiunge una testimonianza alla lista*/
Lista add_to_list(Lista l, Testimonianza t){
	Lista new;
	new = malloc(sizeof(struct lista));
	if (new==NULL){	
		fprintf (stderr, "Errore di allocazione nella funzione add_to_list\n");
		exit(-1);
	}
	new->test = t;
	new->next = l;
	return new;
}

/*stampa il contenuto di una lista*/
void print_list(Lista h){
	Lista p;
	for(p=h; p != NULL; p = p -> next)
		printf("%s ", p->test->testimone);	
}

/*restituisce la lunghezza di una lista*/
int length_list(Lista l){
	Lista p;
	int length=0;
	for(p=l; p != NULL; p = p -> next)
		length=length+1;
	return length;
}

/*cancella una testimonianza dalla lista*/
Lista delete_node(Lista l, Testimonianza t){
	Lista cur,prev;
	for(cur=l, prev=NULL; cur != NULL && cur->test != t;prev=cur,cur=cur->next){}
	if(cur==NULL)
		return l;
	if (prev==NULL)
		l = l->next;
	else
		prev->next = cur->next;
	free(cur);
	return l;
}

/*distrugge tutta la lista*/
void destroy_list(Lista l){
	Lista corr;
		for(corr=l;corr != NULL;corr=l){
			l=l->next;
			free(corr);		
		}	
}

/*****************************************
* Definisco le procedure per la struttura bit_node, ovvero per l'albero binario di ricerca
*****************************************/
/*crea un nuovo nodo da inserire nell'albero*/
Bit_node bit_new(Testimonianza test){
	Bit_node new = malloc(sizeof(struct bit_node));
	if (new== NULL){
		fprintf (stderr, "Errore di allocazione nella funzione bit_new\n");
		exit(-1);
	}
	new->l = new->r= NULL;
	new->test = test;
	return new;
}

/*distrugge il nodo p*/
void bit_delete(Bit_node p){
	if (p == NULL)
		return;
	else{
		destroy_testimonianza(p->test);
		free(p);
		num_nodi=num_nodi-1;
	}
}


/*distrugge l'albero intero dalla radice p*/
void bit_destroy(Bit_node p){
	if (p == NULL)
		return;
	else{
		bit_destroy(p->l);
		bit_destroy(p->r);
		destroy_testimonianza(p->test);
		free(p);
		num_nodi=0;
	}
}

int bist_searchfather(Bit_node r, Chiave k, Bit_node *pf, Bit_node *p){
	int res;
	*pf=NULL;
	*p = r;
	
	if (!r)
		return -1;
	
	while(*p && (res=cmp_key(k,key((*p)->test)))!=0){
		*pf=*p;
		*p = res < 0 ? (*p) ->l : (*p) ->r;
	}
	if(*p == NULL)
		return -1;
	return 0;
}


/*inserisce una nuova testimonianza nell'albero */
void bist_insert(Bit_node *r, Testimonianza test){
	Bit_node qf, q=*r,new=bit_new(test);
	Chiave k = key(test);
	
	if (q==NULL){
		num_nodi=num_nodi+1;
		*r=new;
		return;
	}
	
	if ( bist_searchfather(*r,k,&qf,&q)==0){
		/* non possono esserci presenti testimoni con la stessa chiave perchè li ho cancellati in precedenza*/
	}
	
	if(cmp_key(k, key(qf->test))<0)
		qf->l = new;
	else 
		qf->r=new;
	num_nodi=num_nodi+1;
}

/*cancella un nodo con testimone k dall'albero binario di ricerca r*/
void bist_delete(Bit_node *r, Chiave k){
	Bit_node x, xf, s = NULL;
	
	if (bist_searchfather(*r,k,&xf,&x)==-1)
		return ;
/*		
	if(x->l ==  NULL && x->r==NULL)
		s==NULL;
*/
	else if(x->l ==  NULL || x->r==NULL)
			s=x->l != NULL ? x->l:x->r;
	else{
		Bit_node sf=x;
		s = x->l;
		while (s->r){
			sf = s;
			s=s->r;
		}
		s->r=x->r;
		if(sf->r == s){
			sf->r=s->l;
			s->l=x->l;
		}
	}
	
	if (x==*r)
		*r=s;
	else if (xf->l == x)
		xf->l = s;
	else 
		xf->r=s;
	
	bit_delete(x);

	return ;
}

/*ricerca nell'albero binario di ricerca un nodo con chiave k e restituisce un puntatore alla testimonianza di quel testimone*/
Testimonianza bist_search(Bit_node r, Chiave k){
	Bit_node pf=NULL, p = NULL;
	if(bist_searchfather(r,k,&pf,&p)==0)
		return p->test;
	return NULL;
}

/*****************************************
* Definisco le procedure per la struttura pqueue
*****************************************/
/***********************************************
* saranno create le funzioni per costruire due tipi di code. se flag è settato a 0 la coda avrà come chiave il campo partenza di una testimonianza
* mentre se flag è settato a 1 la coda avrà come chiave il campo arrivo di una testimonianza.
* utilizzo la prima coda per risolvere la funzione interrogatori, massimo e lunghezza mentre la seconda viene utilizzata per
* risolvere la funzione tempo_deserta.
*
*******************************************************/

/* crea una coda di priorità vuota che potrà contenere al massimo n Testimonianze */
Pqueue pqueue_new ( int n ){
	Pqueue H;
	Testimonianza t;
	Orario o;
	o=new_orario(0,0);
	/*crea una testimonianza minima*/
	t=new_testimonianza("a",o,o);
	H = malloc(sizeof(struct pqueue));
	if(H==NULL){
		fprintf (stderr, "Errore di allocazione nella funzione pqueue_new\n");
		exit(-1);
	}
	H->h=malloc((n+1)*sizeof(Testimonianza));
	if(H->h==NULL){
		fprintf (stderr, "Errore di allocazione nella funzione pqueue_new\n");
		exit(-2);
	}
	H->dimensione=n;
	H->valore=0;
	H->h[0]=t;
	return H;
}

/* distrugge la coda di priorità */
void pqueue_destroy ( Pqueue H){
	free(H->h);
	free(H);
	H=NULL;
}


/* restituisce la lunghezza della coda di priorità */
int pqueue_length ( Pqueue H){
	return H->valore;
}

/* inserisce la Testimonianza nella coda di priorità */
void pqueue_insert ( Pqueue H, Testimonianza t){
	if (H->valore == H->dimensione){
		printf("errore non è possibile inserire altri elementi, la coda è piena!\n");
		return;
	}
	H->valore=H->valore+1;
	H->h[H->valore]=t;
	heapify_up(H->h, H->valore);

}

/* estrae dalla coda di priorità la testimonianza con chiave minore */
Testimonianza pqueue_extractmin ( Pqueue H){
	Testimonianza t = pqueue_min(H);
	swap(H->h, H->valore, 1);
	H->valore=H->valore-1;
	/*se c'è solo un elemento nella coda è inutile ordinarla*/
	if(H->valore <=1)
		return t;
	/*riordina la coda in base usando come chiave l'ora di partenza*/
	if (flag==0){
		if(cmp_orario(key_pqueue(H->h[1]), key_pqueue(H->h[father(1)]))<0)
			heapify_up(H->h,1);
		if(cmp_orario(key_pqueue(H->h[1]),key_pqueue(H->h[left(1)]))>0||cmp_orario(key_pqueue(H->h[1]),key_pqueue(H->h[right(1)]))>0)
			heapify_down(H->h,1,H->valore);
	}
	/*riordina la coda in base usando come chiave l'ora di arriva*/
	if (flag==1){
		if(cmp_orario(key_pqueue2(H->h[1]), key_pqueue2(H->h[father(1)]))<0)
			heapify_up(H->h,1);
		if(cmp_orario(key_pqueue2(H->h[1]),key_pqueue2(H->h[left(1)]))>0||cmp_orario(key_pqueue2(H->h[1]),key_pqueue2(H->h[right(1)]))>0)
			heapify_down(H->h,1,H->valore);
	}
	return t;
}

/* restituisce la testimonianza con chiave minima nella coda di priorità */
Testimonianza pqueue_min ( Pqueue H){
	if (H->valore != 0)
		return H->h[1];
	return NULL;
}	

int father(int i){
	return i/2;
}

int left(int i){
	return 2*i;
}

int right(int i){
	return 2*i+1;
}

/*restiruisce la chiave partenza*/
Orario key_pqueue(Testimonianza t){
	return t->partenza;
}

/*restituisce la chiave arrivo*/
Orario key_pqueue2(Testimonianza t){
	return t->arrivo;
}

/*scambia di posto due elementi della coda*/
void swap(Heap h, int i, int j){
	Testimonianza temp;
	temp= h[i];
	h[i]=h[j];
	h[j]=temp;
	return;
}

/*riordina la coda in base alla chiave*/
void heapify_up(Heap h, int i){
		if(i>1){
		int j= father(i);
		if(flag==0){
			/*riordina la coda in base alla chiave partenza*/
			if(cmp_orario(key_pqueue(h[i]),key_pqueue(h[j]))<0){
				swap(h,i,j);
				heapify_up(h,j);
			}
		}
		if (flag==1){
			/*riordina la coda in base alla chiave arrivo*/
			if(cmp_orario(key_pqueue2(h[i]),key_pqueue2(h[j]))<0){
				swap(h,i,j);
				heapify_up(h,j);
			}
		}
	}
}

/*riordina la coda in base alla chiave*/
void heapify_down(Heap h, int i, int n){
	if(2*i<=n){
		int j;
		if(flag==0){
			/*riordina la coda in base alla chiave partenza*/
			if (2*i==n)
				j=2*i;
			else
				j=cmp_orario(key_pqueue(h[2*i]),key_pqueue(h[2*i+1]))<0?2*i:2*i+1;
		
			if(cmp_orario(key_pqueue(h[j]),key_pqueue(h[i]))<0){
				swap(h,i,j);
				heapify_down(h,j,n);
			}
		}
		
		if(flag==1){
			/*riordina la coda in base alla chiave arrivo*/
			if (2*i==n)
				j=2*i;
			else
				j=cmp_orario(key_pqueue2(h[2*i]),key_pqueue2(h[2*i+1]))<0?2*i:2*i+1;
		
			if(cmp_orario(key_pqueue2(h[j]),key_pqueue2(h[i]))<0){
				swap(h,i,j);
				heapify_down(h,j,n);
			}
		}
		
	}
}

/*****************************************
* Definisco le procedure per l'implementazione dei grafi
*****************************************/
/*cancella tutti gli archi del grafo che si collegano alla testimonianza t */
void delete_graph(Bit_node p,Testimonianza t){
	if (p){
		delete_graph(p->l,t);
		
		p->test->incontri=delete_node(p->test->incontri,t);/*cancello tutti i nodi dell'arco che collegano al testimone che sto per cancellare*/
				
		delete_graph(p->r,t);
	}
}

/*inserisce gli archi del grafo che si collegano alla nuova testimonianza t */
void insert_graph(Bit_node p,Testimonianza t){
	if (p){
		insert_graph(p->l,t);
		
		if( ((cmp_orario(t->arrivo,p->test->arrivo)>=0)&&(cmp_orario(t->arrivo,p->test->partenza)<=0))
		||((cmp_orario(t->arrivo,p->test->arrivo)<=0)&&(cmp_orario(t->partenza,p->test->arrivo)>=0))){
			/*vuol dire che le due persone si sono incontrate quindi aggiungo alla lista di adiacenza di entrambe l'incontro con l'altra persona
			praticamente costruisco il grafo*/
			t->incontri=add_to_list(t->incontri,p->test);
			p->test->incontri=add_to_list(p->test->incontri,t);
		}
		
		
		insert_graph(p->r,t);
	}
}

/* visita in profondità il sottografo a partire dal testimone t */
void visit_graph(Testimonianza t){
	Lista l;
	printf("%s ", t->testimone);
	t->visitato=1;
	/*scorro la lista incontri del testimone t*/
	for(l=t->incontri; l != NULL; l = l -> next){
		if(l->test->visitato==0)
			visit_graph(l->test);
	}
}

/*****************************************
/ Procedure del main
*****************************************/
/* legge una stringa da stdin e restituisce il puntatore*/
char *read_string(char ch){
	char *stringa,c;
	int DIM=10;/*sarà la dimensione della variabile stringa*/
	int i;
	
	stringa = malloc(DIM * sizeof(char));/*all'inizio stringa potrà contenere solo 10 caratteri*/
	if(!stringa){
		fprintf (stderr, "Errore di allocazione nella funzione read_string\n");
		exit(-1);
	}
	*stringa = ch;
	scanf("%c",&c);
	for(i=1;(!isspace(c)) && (c!='\n') ;i++){
		if(i>= DIM-1){ /*tengo un carattere per inserire il carattere fine strigna*/
			DIM = DIM +10;
			stringa=realloc(stringa, DIM * sizeof(char)); /*ogni volta che la parola è piu lunga di DIM alloco altri 10 caratteri a stringa*/
			if(!stringa){
				fprintf (stderr, "Errore di allocazione nella funzione read_string\n");
				exit(-2);
			}
		}
		stringa[i]=	c;
		scanf("%c",&c);
	}
	stringa[i]='\0';
	return stringa;
}

/* legge una stringa da stream e restituisce il puntatore*/
char *read_stringfile(FILE *input,char ch){
	char *stringa,c;
	int DIM=10;/*sarà la dimensione della variabile stringa*/
	int i;
	
	stringa = malloc(DIM * sizeof(char));/*all'inizio stringa potrà contenere solo 10 caratteri*/
	if(!stringa){
		fprintf (stderr, "Errore di allocazione nella funzione read_stringfile\n");
		exit(-1);
	}
	*stringa = ch;
	fscanf(input, "%c", &c);
	for(i=1;(!isspace(c)) && (c!='\n') ;i++){
		if(i>= DIM-1){ /*tengo un carattere per inserire il carattere fine strigna*/
			DIM = DIM +10;
			stringa=realloc(stringa, DIM * sizeof(char)); /*ogni volta che la parola è piu lunga di DIM alloco altri 10 caratteri a stringa*/
			if(!stringa){
				fprintf (stderr, "Errore di allocazione nella funzione read_stringfile\n");
				exit(-2);
			}
		}
		stringa[i]=	c;
		fscanf(input, "%c", &c);
	}
	stringa[i]='\0';
	return stringa;
}

/*visita tutto l'albero binario e restiruisce il testimone che ha la lista incontri piu lunga*/
int check_length_test_key(Bit_node p, int max){
	int t=0;
	if(p){
		max=check_length_test_key(p->l,max);
		
		t=length_list(p->test->incontri);
		if(t>max)
			max = t;
		
		max=check_length_test_key(p->r,max);
	}
	return max;
}

/*visita tutto l'albero binario, se il testimone è sulla scena del crimine all'orario o allora lo stampa*/
void check_ora(Bit_node p, Orario o){
	if (p){
		check_ora(p->l,o);
		
		if((cmp_orario(p->test->arrivo,o)<=0)&&(cmp_orario(p->test->partenza,o)>=0))
			printf("%s ",p->test->testimone);
		
		check_ora(p->r,o);
	}
}

/*visita tutto l'albero e inserisco ogni nodo nella coda*/
void tree_in_pqueue(Bit_node p, Pqueue h){
if (p){
		tree_in_pqueue(p->l,h);
		
		pqueue_insert(h,p->test);
		
		tree_in_pqueue(p->r,h);
	}
}

/* visita tutto l'albero e azzero il campo visitato, mi preparo per la visita in profondità del grafo*/
void azzera_visitato(Bit_node p){
	if (p){
		azzera_visitato(p->l);
		
		p->test->visitato=0;
		
		azzera_visitato(p->r);
	}
}

/* separa i testimoni in modo che nessun testimone sia nella stanza con altri testimoni incontrati sulla scena del crimine
e suddivide i testimoni nelle stanze di conseguenza.
Per farlo utilizzo un algoritmo di greedy dove l'elemento di costo minimo è l'elemento con ora di partenza minore.
1 Estraggo il primo elemento della coda H e lo stampa. 
2 Successivamente estraggo tutti gli elementi della coda H fino a quando non è vuota . 
  Se l'elemento estratto non si sovrappone con l'ultimo elemento stampato allora lo stampo, altrimenti lo inserisco in una lista temporanea P
  dove ci sono gli elementi che non ho ancora stampato.
3 Sposto tutti gli elementi di P in H e ripeto dal passo 1 fino a quando non stampo tutti gli elementi  */

void interrogatori(Pqueue H){
	Pqueue P;
	Testimonianza min,temp;
	P = pqueue_new(num_nodi);
	while(pqueue_length(H) != 0)	{
		min = pqueue_extractmin(H);
		printf("%s",min->testimone);
		while(pqueue_length(H)!=0){
			temp=pqueue_extractmin(H);
			if(cmp_orario(temp->arrivo,min->partenza)>0){	
				printf(" %s",temp->testimone);
				min=temp;
			}
			else
				pqueue_insert(P, temp);
		}	
		
		printf(".\n");
		while(pqueue_length(P)!=0){
			temp=pqueue_extractmin(P);
			pqueue_insert(H,temp);
		}
	}
	pqueue_destroy(P);
}

/*stampa per quanti minuti la scena del crimine è rimasta deserta, ovvero non era presente nessun testimone.
Per farlo utilizzo un algoritmo di greedy dove l'elemento di costo minimo è l'elemento con ora di arrivo minore.
1 estraggo il primo elemento della coda lo salvo in una varibile temp e calcolo quanti minuti passano dalle 0:00 all'ora di arrivo dell'elemento 
e li salvo in una variabile mintot.
2 Successivamente estraggo tutti gli elementi della coda H fino a quando non è vuota . 
  Se l'elemento estratto non si sovrappone con l'elemento temp allora calcolo quanti minuti passano dalla partenza di temp all'arrivo dell'elemento estratto 
  e li sommo a mintot, altrimenti salvo l'elemento estratto nella variabile temp.
3 Quando la lista è vuota calcolo quanti minuti sono passati dalla partenza dell'elemento temp alle 24:00 */

void tempo_deserta(Pqueue H){
	int mintot=0;/*contiene per quanti minuti la scena del crimine risulta deserta nel giorno del delitto*/
	int mm;
	Pqueue P;
	Testimonianza min,temp;
	P = pqueue_new(num_nodi);
	if(pqueue_length(H)!=0){
		min = pqueue_extractmin(H);
		mm = (min->arrivo->ora)*60 + (min->arrivo->minuti);
		mintot = mm;
	}
	while(pqueue_length(H)!=0){
		temp=pqueue_extractmin(H);
		if(cmp_orario(temp->arrivo,min->partenza)>0){
			mm=((temp->arrivo->ora)*60+(temp->arrivo->minuti))-((min->partenza->ora)*60 + (min->partenza->minuti));
			mintot=mintot + mm;
		}
		if(cmp_orario(min->partenza,temp->partenza)<0)
			min=temp;
	}
	mm = 1440 - ((min->partenza->ora)*60 + (min->partenza->minuti));
	mintot=mintot + mm;
	
	printf("%d:%02d\n", (mintot/60),(mintot%60));
	
}

/* calcola il numero massimo di testimoni che erano presenti sulla scena del crimine temporaneamente. Per farlo calcolo il numero 
delle stanza utilizzate per suddividere i testimoni, in quanto in ogni stanza è presente almeno un testimone che si è incontrato con un testimone
delle altre stanze.
La procedura è analoga a interrogatori, con la differenza che qui calcolo solo il numero di stanze necessarie invece che stampare
tutti gli elementi.  */

void massimo(Pqueue H){
	int i=0;/*i contiene il numero delle stanze*/
	Pqueue P;
	Testimonianza min,temp;
	P = pqueue_new(num_nodi);
	
	while(pqueue_length(H) != 0)	{
		min = pqueue_extractmin(H);
		while(pqueue_length(H)!=0){
			temp=pqueue_extractmin(H);
			if(cmp_orario(temp->arrivo,min->partenza)>0){	
				min=temp;
			}
			else
				pqueue_insert(P, temp);
		}	

		i=i+1; /*ogni stanza contiene un testimone che si è incontrato con un testimone presente nelle altre stanze, quindi massimo restituisce il numero di stanze*/
		
		while(pqueue_length(P)!=0){
			temp=pqueue_extractmin(P);
			pqueue_insert(H,temp);
		}
	}
	printf("%d\n",i);
	pqueue_destroy(P);
}

/*stampa la lista più lunga di testimoni che si sono susseguiti sulla scena del crimine senza mai incontrarsi.
Per farlo utilizzo un algoritmo di greedy dove l'elemento di costo minimo è l'elemento con ora di partenza minore.
1 estraggo il primo elemento della coda e lo stampo.
2 Successivamente estraggo tutti gli elementi della coda H fino a quando non è vuota . 
  Se l'elemento estratto non si sovrappone con l'ultimo elemento stampato allora lo stampo */

void lunga(Pqueue H){
	Pqueue P;
	Testimonianza min,temp;
	P = pqueue_new(num_nodi);
	if(pqueue_length(H)!=0){
		min = pqueue_extractmin(H);
		printf("%s ",min->testimone);
	}
	while(pqueue_length(H)!=0){
		temp=pqueue_extractmin(H);
		if(cmp_orario(temp->arrivo,min->partenza)>0){	
			printf("%s ",temp->testimone);
			min=temp;
		}
	}
	printf("\n");
	pqueue_destroy(P);
}

/**************************************************************************************/
/*					MAIN																						  */
/**************************************************************************************/
int main(int argc,char *argv[]){
	char c,*stringa;
	FILE *input;
	int h,m,k,max;
	Orario arr,part,ora;
	Testimonianza new,old;
	Pqueue H=NULL;
	Bit_node albero=NULL;
	while ( ( c = getchar() ) != 'q' ){
		switch (c){
			case 'L':
				while(isspace(c=getchar()));/*salto tutti i caratteri di spazio e salvo il primo carattere della stringa*/
				stringa=read_string(c);
				input=fopen(stringa,"r");
				if (input == NULL)
					printf("FILE %s NON TROVATO\n",stringa);
				else {
					if (albero !=NULL){
						bit_destroy(albero);
						albero = NULL;
					}
					while ((fscanf(input, "%c", &c)!= EOF)){
						/* salto tutti i caratteri di spazio e salvo il primo carattere della stringa */
						while(isspace(c))
							fscanf(input, "%c", &c); 
						stringa=read_stringfile(input,c);
						fscanf(input, "%d", &h);
						fscanf(input, "%c", &c);/*salto i due punti*/
						fscanf(input, "%d", &m);
						arr=new_orario(h,m);
						fscanf(input, "%d", &h);
						fscanf(input, "%c", &c);/*salto i due punti*/
						fscanf(input, "%d", &m);
						part=new_orario(h,m);
						new = new_testimonianza(stringa,arr,part);
						old=NULL;
						old=bist_search(albero,key(new));/*cerco se c'è gia nell'albero una testimonianza per quel testimone*/
						if(old!=NULL){					
							delete_graph(albero,old);/*cancello tutti gli archi del grafo collegati alla testimonianza che sto per cancellare*/
							bist_delete(&albero,key(new));/*cancello la testimonianza*/
						}
						insert_graph(albero,new);/*controllo l'albero per vedere se il nuovo testimone ha incontrato testimoni vecchi e modifico le liste incontri*/
						bist_insert(&albero,new);/*inserisco il nuovo testimone*/
						fscanf(input, "%c", &c);
						/* salto tutti i caratteri di spazio e il carattere di fine stringa */
						while(c!='\n')
							fscanf(input, "%c", &c); 
					}
					fclose(input);
				}
				break;
			case 'T':
				while(isspace(c=getchar()));/*salto tutti i caratteri di spazio e salvo il primo carattere della stringa*/
				stringa=read_string(c);
				scanf("%d",&h);
				scanf("%c",&c);/*salto i due punti*/
				scanf("%d",&m);
				arr=new_orario(h,m);
				scanf("%d",&h);
				scanf("%c",&c);/*salto i due punti*/
				scanf("%d",&m);
				part=new_orario(h,m);
				new=new_testimonianza(stringa,arr,part);
				old=NULL;
				old=bist_search(albero,key(new));/*cerco se c'è gia nell'albero una testimonianza per quel testimone*/
				if(old!=NULL){					
					delete_graph(albero,old);/*cancello tutti gli archi del grafo collegati al testimone che sto per cancellare*/
					bist_delete(&albero,key(new));/*cancello il testimone*/
				}
				insert_graph(albero,new);/*controllo l'albero per vedere se il nuovo testimone ha incontrato testimoni vecchi e modifico le liste incontri*/
				bist_insert(&albero,new);/*inserisco il nuovo testimone*/
				break;	
			case 'C':
				while(isspace(c=getchar()));/*salto tutti i caratteri di spazio e salvo il primo carattere della stringa*/
				stringa=read_string(c);
				old = NULL;
				old=bist_search(albero,stringa);
				if(old!=NULL){	
					delete_graph(albero,old);/*cancello tutti gli archi del grafo collegati al testimone che sto per cancellare*/
					bist_delete(&albero,key(old));/*cancello il testimone*/
				}
				break;
			case 'o':
				scanf("%d",&h);
				scanf("%c",&c);/*salto i due punti*/
				scanf("%d",&m);
				ora=new_orario(h,m);
				check_ora(albero, ora);
				printf("\n");
				break;		
			case 'i':
				H = pqueue_new(num_nodi);
				tree_in_pqueue(albero,H);
				printf("(");
				massimo(H);
				tree_in_pqueue(albero,H);
				interrogatori(H);
				printf(")\n");
				pqueue_destroy(H);
				break;
			case 't':
				flag=1;
				H = pqueue_new(num_nodi);
				tree_in_pqueue(albero,H);
				tempo_deserta(H);
				pqueue_destroy(H);
				flag=0;
				break;
			case 'e':	
				while(isspace(c=getchar()));/*salto tutti i caratteri di spazio e salvo il primo carattere della stringa*/
				stringa=read_string(c);
				old = NULL;
				old=bist_search(albero,stringa);
				if(old==NULL)
					printf("%s NON ESISTENTE",stringa);
				else
					print_list(old->incontri);
				printf("\n");	
				break;
			case 'm':
				H = pqueue_new(num_nodi);
				tree_in_pqueue(albero,H);
				massimo(H);
				pqueue_destroy(H);
				break;
			case 'l':
				H = pqueue_new(num_nodi);
				tree_in_pqueue(albero,H);
				lunga(H);
				pqueue_destroy(H);
				break;
			case 'k':	
				while(isspace(c=getchar()));/*salto tutti i caratteri di spazio e salvo il primo carattere della stringa*/
				stringa=read_string(c);
				old = NULL;
				old=bist_search(albero,stringa);
				if(old==NULL)
					printf("%s NON ESISTENTE",stringa);
				else{
					k=length_list(old->incontri);
					max=0;
					max=check_length_test_key(albero,max);
					if(k==max)
						printf("%s: TESTIMONE CHIAVE",stringa);
					else
						printf("%s: TESTIMONE NON CHIAVE", stringa);
				}
				printf("\n");
				break;
			case 'b':
				while(isspace(c=getchar()));/*salto tutti i caratteri di spazio e salvo il primo carattere della stringa*/
				stringa=read_string(c);
				old = NULL;
				old=bist_search(albero,stringa);
				if(old==NULL)
					printf("%s NON ESISTENTE",stringa);
				else{
					azzera_visitato(albero);/*azzero il campo visitato delle testimonianze prima di eseguire la visita in profondità*/
					visit_graph(old);
				}
				printf("\n");
				break;	
		}/*fine switch*/
	 }
	 bit_destroy(albero);
	 return 0;
}
