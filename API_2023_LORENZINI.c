// v4.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define N   8  // lunghezza iniziale vettore input     (minimo 1)
#define ST  8 // lunghezza iniziale vettori stazioni  (minimo 2)
#define AU  8  // lunghezza iniziale vettori auto      (minimo 2)

typedef int*    aut;
typedef struct  Stazione_di_servizio{
    int         d;
    int         max_auto;
    int         ragg;       // serve a pianifica_percorso per marcare chi può raggiungere
    aut         A;
}staz;
typedef staz*   dist;

const char del[2] = " ";

// mini funzioni che aiutano il programma ***********
int        ricerca(dist T, int arg, int type);
int         set_cmd(char* s);
int         passata(dist p, int I, int v);
int         f_atoi(const char* s_int);
int         find_max_auto(aut A);

dist       list_array(dist T, dist start, dist end);

void        check_max_auto(staz* S, int aut);
void        print_path(dist p, int R, int D, int v);

char*       in_str();

// maxi funzioni del programma **********************
dist        exe_cmd(dist T, int i);
dist        agg_staz(dist T, int type);
dist        dem_staz(dist T, int type);

int        pianifica_percorso(dist T, dist p, dist START, dist END);

void stampa(dist T){
    for(int i = 2; i < T[1].d; i++)
        printf("%d(%d) ", T[i].d, T[i].max_auto);
}

int main(){

    dist IN = (dist) malloc(N * sizeof(staz));
    IN[0].d = N;
    IN[1].d = 2;

    while(feof(stdin) == 0){ // legge input finché non finisce il file

        char *str = in_str(); // stringa stdin
        
        if(str[0] != 0)
            IN = exe_cmd(IN, set_cmd(strtok(str, del)) ); // *** si esegue il comando della riga ***
    }
    // stampa(IN);

    return 0;
}

char* in_str(){
    int i = 0, n = N;
    char ch = 'A', *s = (char *) malloc(n * sizeof(char));

    while(ch != '\n' && ch != EOF){
        
        if(i == n){
            n = 2*n;
            s = (char *) realloc(s, n*sizeof(char));
        }
        
        ch = getc(stdin);
        
        if(ch == '\r')
            continue;

        s[i] = ch;
        
        i++;
    }

    // s = (char *) realloc(s, i*sizeof(char));
    s[i-1] = '\0';
    //printf("%s\n", s);

    return s;
}
int f_atoi(const char* s_int){
    int num = 0;
    while(*s_int)
        num = num*10 + (*s_int++ - '0');

    return num;
}

int set_cmd(char* s){
    if(strcmp(s, "aggiungi-stazione") == 0)
        return 0;
    else if(strcmp(s, "demolisci-stazione") == 0)
        return 1;
    else if(strcmp(s, "aggiungi-auto") == 0)
        return 2;
    else if(strcmp(s, "rottama-auto") == 0)
        return 3;
    else if(strcmp(s, "pianifica-percorso") == 0)
        return 4;

    return -1; // in teoria l'input è assunto giusto quindi è inutile 
}
dist exe_cmd(dist T, int i){

    if(i == 0)
        T = agg_staz(T, 0);
    else if(i == 1)
        T = dem_staz(T, 0);
    else if(i == 2)
        T = agg_staz(T, 1);
    else if(i == 3)
        T = dem_staz(T, 1);
    else if(i == 4){
        int I = 0, v = 0, arg = f_atoi(strtok(NULL, del));
        dist START = NULL, END = NULL;

        if( (I = ricerca(T, arg, 0)) != 0){
            v = 1;
            START = &T[I];
        }
        if(!v){
            puts("nessun percorso");

            return T;
        }
        else    arg = f_atoi(strtok(NULL, del));

            v = 0;
            I = 0;

        if( (I = ricerca(T,arg,0)) != 0){
            v = 1;
            END = &T[I];
        }
        if(v){
            // l è il numero di stazioni tra START e END (compresi)
            dist p = list_array(T, START, END);

            // int l0 = l;
            
            if(pianifica_percorso(T, p, START, END) == -1)
                puts("nessun percorso");
            else{
                /*puts("Tutte le stazioni:");
                for(int j = 0; j < l0; j++)
                    printf("%d\t", p[l0 - j - 1]->curr_staz->d);
                puts("");
                for(int j = 0; j < l0; j++)
                    printf("%d\t", p[l0 - j - 1]->curr_staz->max_auto);
                puts("");*/
            }
            free(p);
        }
        else    puts("nessun percorso");
    }

    return T;
}

dist list_array(dist T, dist start, dist end){
    int a = start->d, b = end->d;

    if(a > b){ // si pone b > a a prescindere
        int temp = a;
        a = b;
        b = temp;
    }

    dist v = (dist) malloc(T[1].d * sizeof(staz) / 2); // crea vettore di stazioni

    v[0].d = T[1].d / 2; // dimensione vettore
    v[1].d = 2;

    for(int i = 2; i < T[1].d; i++){
        if( (T[i].max_auto > 0 && T[i].d >= a && T[i].d <= b) || end == &T[i]){

            if(v[0].d == v[1].d){
                v[0].d = 2 * v[0].d;
                v = (dist) realloc(v, v[0].d * sizeof(staz));
            }

            v[v[1].d] = T[i];
            (v[1].d)++;
            // impostazione corretta di START e END
            if      (start->d == v[i].d)  start = &v[i];
            else if (end  ->d == v[i].d)  end   = &v[i];
        }
    }

    return v;
}
void print_path(dist p, int R, int D, int v){  // R indica che ragg deve stampare
                                               // D indica la distanza precedente stampata
    int min = INT_MAX;

    for(int i = 2; i < p[1].d; i++){
        if(R == 0)
            return;
        if(p[i].ragg == R && p[i].d < min && v * p[i].d + p[i].max_auto >= v * D)
                min = p[i].d;
    }

    print_path(p, R-1, min, v);
   
    printf(" %d", min);
}
int passata(dist p, int I, int v){
    int m = -1, min = INT_MAX, max = INT_MIN;
    
    for(int i = 2; i < p[1].d; i++){
        if(p[i].ragg == -1 && v * p[I].d + p[I].max_auto >= v * p[i].d){
            p[i].ragg = p[I].ragg + 1;

            if(v == 1 && p[i].d + p[i].max_auto > max){
                max = p[i].d + p[i].max_auto;
                m = i;
            }
            else if (v == -1 && p[i].d - p[i].max_auto < min){
                min = p[i].d - p[i].max_auto;
                m = i;
            }
        }
    }
    // printf("\n\n%d_%d\n\n", p[I].ragg, I);
    return m;
}
int pianifica_percorso(dist T, dist p, dist START, dist END){
    int v = 1, I = 0;

    if(START->d > END->d)
        v = -1;

    if(START->max_auto <= 0 && START != END) // caso particolare
        return -1;

    if(p[1].d - 2 == 2){ // caso in cui lunghezza uguale a 2
        if(v * START->d + START->max_auto >= v * END->d)
            printf("%d %d\n", START->d, END->d);
        else
            return -1;

        return 0;
    }
    // si settano finalmente START e END
    for(int i = 0; I < 2; i++){
        if(START->d == p[i].d){
            START = &p[i];
            I++;
        }
        else if(END->d == p[i].d){
            END = &p[i];
            I++;
        }
    }

    START->ragg = 0;
    
    for(int i = 2; i < p[1].d; i++){ // per trovare l'indice dello START
        if(p[i].ragg == 0){
            I = i;
            break;
        }
    }

    while(I != -1 && END->ragg == -1)
        I = passata(p, I, v);

    /*puts("");
    for(int i = 2; i < p[1].d; i++)
        printf("%d\t(%d_%d):\t%d\n", p[i].d, i, p[i].max_auto, p[i].ragg);
    */

    if(I == -1) // se non trova un percorso
        return -1;
    else{ // sezione di stampa percorso
        START->ragg = -1;

        printf("%d", START->d);
        print_path(p, END->ragg - 1, END->d, v);
        printf(" %d\n", END->d);
    }

    for(int i = 2; i < p[1].d; i++)  //ripristino ragg
        p[i].ragg = -1;
    
    return 0;
}

dist agg_staz(dist T, int type){
    int I = 0, v = 0, arg = f_atoi(strtok(NULL, del));

    if((I = ricerca(T, arg, 0)) != 0)   v = 1;
    if(v-type != 0){ // vede se ne esista già una
        puts("non aggiunta");

        return T;
    }

    if(!type){ // aggiunta della stazione

        if(T[1].d == T[0].d){
            T[0].d = 2 * T[0].d;
            T = (dist) realloc(T, T[0].d * sizeof(staz));
        }

        T[T[1].d].d         = arg;
        T[T[1].d].max_auto  = -1;
        T[T[1].d].A         = (aut) malloc(AU * sizeof(int));
        T[T[1].d].A[0]      = AU;
        T[T[1].d].A[1]      =  2;
        T[T[1].d].ragg      = -1;
        
        T[1].d++;
    }

    char *endl   = NULL;
    int   n_auto = 0;

    if(!type){
        endl   = strtok(NULL, del);
        n_auto = f_atoi(endl);
    }

    if(!type && n_auto > 512)  v = -1;
    while(v != -1 && (endl = strtok(NULL, del)) != NULL){
        arg = f_atoi(endl);

        if(!type){
            // aggiunta auto
            if(T[T[1].d - 1].A[1] == T[T[1].d - 1].A[0]){
                T[T[1].d - 1].A[0] = 2 * T[T[1].d - 1].A[0];
                T[T[1].d - 1].A = (aut) realloc(T[T[1].d - 1].A, T[T[1].d - 1].A[0] * sizeof(int));
            }

            T[T[1].d - 1].A[T[T[1].d - 1].A[1]] = arg;
                
            check_max_auto(&T[T[1].d - 1], arg);

            T[T[1].d - 1].A[1]++;
        }
        else{
            if(T[I].A[1] == 512 + 2){
                v = -1;
                break;
            }
            
            // aggiunta auto
            if(T[I].A[1] == T[I].A[0]){
                T[I].A[0] = 2 * T[I].A[0];
                T[I].A = (aut) realloc(T[I].A, T[I].A[0] * sizeof(int));
            }
            
            T[I].A[T[I].A[1]] = arg;

            check_max_auto(&T[I], arg);

            T[I].A[1]++;
        }
    }

    if(v == -1){
        puts("non aggiunta");
        
        return T;
    }
    
    puts("aggiunta");
    
    return T;
}

dist dem_staz(dist T, int type){
    int I = 0, v = 0, arg = f_atoi(strtok(NULL, del));
    char out[11], non[15] = "non ";
    staz *T0;

    if(!type)   strcpy(out, "demolita");
    else        strcpy(out, "rottamata");

    if((I = ricerca(T, arg, 0)) != 0)    v = 1;
    
    if(!v){ // se non la trovi entra qui e non esce più
        puts(strcat(non, out));
        
        //if(type)    free();

        return T;
    }

    if(!type){
        for(int i = I; i < T[1].d - 1; i++)
            T[i] = T[i+1];

        (T[1].d)--;

        // ridimensionamento se ci sono poche stazioni e lungo vettore
        if(T[0].d == 4 * T[1].d && T[0].d % 2 == 0 && T[0].d > 6){
            T[0].d = T[0].d / 2;
            T = (dist) realloc(T, T[0].d * sizeof(staz));
        }
    }
    else{
        v = 0;
        T0 = &T[I];

        arg = f_atoi(strtok(NULL, del));

        if((I = ricerca(T0,arg,1)) == 0){
            puts("non rottamata");
                
            return T;
        }

        for(int i = I; i < T0->A[1] - 1; i++)
            T0->A[i] = T0->A[i+1];
        //printf("\n\n%d\n\n", T0->A[0]);
        // ridimensionamento se ci sono poche stazioni e lungo vettore
        if(T0->A[0] == 4 * T0->A[1] && T0->A[0] % 2 == 0 && T0->A[0] > 6){
            T0->A[0] = T0->A[0] / 2;
            T0->A = (aut) realloc(T0->A, T0->A[0] * sizeof(int));
        }

        (T0->A[1])--;
        T0->max_auto = find_max_auto(T0->A);
    }

    puts(out);

    return T;
}
int ricerca(dist T, int arg, int type){
    if(!type){
        for(int i = 2; i < T[1].d; i++)
            if(T[i].d == arg)         return i;
    }
    else{
        for(int i = 2; i < T->A[1]; i++)
            if(T->A[i] == arg)      return i;
    }

    return 0;
}

int find_max_auto(aut A){
    int max = -1;

    for(int i = 2; i < A[1]; i++){
        if(A[i] > max)
            max = A[i];
    }

    return max;
}
void check_max_auto(staz *S, int aut){
    if(S->max_auto < aut)
        S->max_auto = aut;
}
