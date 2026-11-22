#include<stdio.h>
#include<stdlib.h>
#define N 9 
#define MAX_CAIXAS 3
#define PAREDE 1
#define SEM_CAIXA -1
#define ALVO 4

typedef struct estado {
    int jogador;
    int caixas[MAX_CAIXAS];
    int qtd_caixas;
} Estado;

typedef struct no{
    Estado estado;
    struct no *pai;
    int g;
    int f;
}No;

typedef struct posicao {
    int linha;
    int coluna;
} Posicao;

No *criar_no(Estado e, No *pai, int custo_passo){
    No *novo = (No*) malloc(sizeof(No));
   
    if (novo == NULL){
        printf("Sem memoria para o malloc\n");
        exit(1);
    }

    novo -> estado = e;
    novo -> pai = pai;

    if (pai != NULL){
        novo -> g = pai -> g + custo_passo;
    }
    else {
        novo -> g = custo_passo;    
    }
    novo -> f = novo -> g; 

    return novo;
}



Estado ler_estado_inicial(int mapa [N][N]){
    Estado e;
    int i, j;

    // zerar o contador de caixas
    e.qtd_caixas = 0;

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            // se mapa[i][j] for 2: ...
            if (mapa[i][j] == 2){
                mapa[i][j] = 0;
                e.jogador = i * N +j;
                
            }
            // se mapa[i][j] for 3: ...
            if (mapa[i][j] == 3){
                mapa[i][j] = 0;
                e.caixas[e.qtd_caixas] = i * N + j;
                e.qtd_caixas ++;
                
            }

        }
    }

    return e;
}
int achar_caixa(Estado e, int posicao){
    int k;
    for(k = 0; k < e.qtd_caixas; k++){
        if(posicao == e.caixas[k]){
            return k;
        }
    }
    return SEM_CAIXA;
}



int eh_parede(int mapa[N][N], int destino){
    int linha, coluna;
    linha = destino / N;
    coluna = destino % N;
    if (mapa[linha][coluna] == PAREDE){
        return PAREDE;
    }
    return 0;
}



int tentar_mover (Estado atual, int mapa[N][N], int d, Estado *novo){
    int destino; 
    
    *novo = atual;
    destino = atual.jogador + d;
    //caso 1: parede 
    if (eh_parede(mapa, destino) == PAREDE){
        return 0; 
    }
    //caso 2: tem caixa no destino 
    int k = achar_caixa(atual, destino);
    if (k != SEM_CAIXA){
        int atras;
        atras = destino + d;
        //nao permite empurrar duas caixas ou mais e nao permite empurrar paredes
        if(eh_parede(mapa, atras) == PAREDE || achar_caixa(atual, atras) != SEM_CAIXA ){
            return 0;
        }
        novo->caixas[k] = atras;
    }
    novo->jogador = destino;
    return 1;
}

int gerar_sucessores(Estado atual, int mapa[N][N], Estado filhos[4]){
    int desloc[4] = {-N, N, -1, 1};   // cima, baixo, esquerda, direita
    int qtd = 0, i;

    for(i = 0; i < 4; i++){
       if(tentar_mover(atual, mapa, desloc[i], &filhos[qtd]) == 1){
            qtd++;
        }
       
    }
    return qtd;    
}

int eh_objetivo(Estado e, int mapa[N][N]){
    for(int i = 0; i < e.qtd_caixas; i++){
        int posicao = e.caixas[i];
        int linha = posicao / N;
        int coluna = posicao % N;
        if (mapa[linha][coluna] != ALVO){
            return 0; 
        }
    }
    return 1;
 
}

void criar_mapa(int mapa[N][N], int nivel){
    
    int j, i;
     for(i=0; i<9; i++){
        for(j=0; j<9; j++){
            mapa[i][j] = 0;

            if(i==4 && j==4){
                mapa[i][j] = 2;
            }
        }
    }

    
    for(i=0; i<9; i++){
        for(j=0; j<9; j++){
            if((i==0)&&(j>=2)&&(j<8)){
                mapa[i][j] = PAREDE ;
            }
            if((i==1)&&(j<=2)){
                mapa[i][j] = PAREDE;
            }
            if((i>=2)&&(i<=8)&&(j==0)){
                mapa[i][j] = PAREDE;
            }
            if((i==8)&&(j<=8)){
                mapa[i][j] = PAREDE;
            }
            if((i>=4)&&(i<=8)&&(j==8)){
                mapa[i][j] = PAREDE;
            }
            if((i>=1)&&(i<=4 )&&(j==7)){
                mapa[i][j] = PAREDE;
            }
            if((i==3)&&(j<=2 )){
                mapa[i][j] = PAREDE;
            }
            if((i>=4)&&(i<=5)&&(j==2)){
                mapa[i][j] = PAREDE;
            }
            if((i==4)&&(j==3))
                mapa[i][j] = PAREDE;
        }
    }

    
    mapa[3][4] = 3;
    mapa[5][5] = 3;

    
    mapa[2][1] = 4;
    mapa[6][2] = 4;

}

int main(){
    // Codigos: 0 = chao | 1 = parede | 2 = jogador | 3 = caixa | 4 = alvo
    int tabuleiro[N][N], i, j;

    No *raiz  = criar_no(inicial, NULL, 0);     // custo do passo 0
    No *filho = criar_no(sucessor, atual, 1);   // so o passo, a funcao soma o g do pai

    // Inicializando o mapa com a func
    criar_mapa(tabuleiro, 1);
    // Imprimindo a matriz
    for(i=0; i<9; i++){
        for(j=0; j<9; j++){
            printf("%d  ", tabuleiro[i][j]);
        }
        printf("\n");
    }
    Estado e = ler_estado_inicial(tabuleiro);
    printf("jogador: %d\n", e.jogador);
    // imprima também as caixas e a quantidade
    printf("quantidade de caixas: %d\n", e.qtd_caixas);

    return 0;
}