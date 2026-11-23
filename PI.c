#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define N 9
#define MAX_CAIXAS 3
#define PAREDE 1
#define SEM_CAIXA -1
#define ALVO 4
#define INF 1000000

typedef struct estado {
    int jogador;
    int caixas[MAX_CAIXAS];
    int qtd_caixas;
} Estado;

typedef struct no{
    Estado estado;
    struct no *pai;
    struct no *prox;
    int g;
    int f;
} No;

typedef struct fila{
    No *inicio;
    No *fim;
} Fila;

int tem_prioridade(No *a, No *b){
    if(a->f < b->f){
        return 1;
    }
    else if(a->f == b->f && a->g > b->g){
        return 1;
    }
    return 0;
}

void fila_iniciar(Fila *f){
    f->inicio = NULL;
    f->fim = NULL;
}

int fila_vazia(Fila *f){
    if (f->inicio == NULL){
        return 1;
    }
    return 0;
}

void fila_inserir(Fila *f, No *novo){
    if (fila_vazia(f) == 1){
        f->inicio = novo;
        f->fim = novo;
    } else if(tem_prioridade(novo, f->inicio) == 1){
        novo->prox = f->inicio;
        f->inicio = novo;
    } else if (tem_prioridade(novo, f->fim) == 0){
        f->fim->prox = novo;
        f->fim = novo;
    } else {
        No *atual = f->inicio;
        while(tem_prioridade(novo, atual->prox) == 0){
            atual = atual->prox;
        }
        novo->prox = atual->prox;
        atual->prox = novo;
    }
}

No *fila_remover(Fila *f){
    No *removido = f->inicio;
    if (fila_vazia(f) == 1){
        printf("Fila vazia, nao ha elementos para remover\n");
        return NULL;
    }
    if (f->inicio == f->fim){
        f->inicio = NULL;
        f->fim = NULL;
    } else {
        f->inicio = removido->prox;
    }
    removido->prox = NULL;
    return removido;
}

No *criar_no(Estado e, No *pai, int custo_passo){
    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL){
        printf("Sem memoria para o malloc\n");
        exit(1);
    }
    novo->estado = e;
    novo->pai = pai;
    novo->prox = NULL;
    if (pai != NULL){
        novo->g = pai->g + custo_passo;
    }
    else {
        novo->g = custo_passo;
    }
    novo->f = novo->g;

    return novo;
}

int estados_iguais(Estado a, Estado b){
    if (a.jogador != b.jogador){
        return 0;
    }
    if (a.qtd_caixas != b.qtd_caixas){
        return 0;
    }
    for(int i = 0; i < a.qtd_caixas; i++){
        if (a.caixas[i] != b.caixas[i]){
            return 0;
        }
    }
    return 1;
}

void ordenar_caixas(Estado *e){
    int i, j, temp;
    for (i = 0; i < e->qtd_caixas - 1; i++){
        for (j = 0; j < e->qtd_caixas - i - 1; j++){
            if (e->caixas[j] > e->caixas[j+1]){
                temp = e->caixas[j];
                e->caixas[j] = e->caixas[j+1];
                e->caixas[j+1] = temp;
            }
        }
    }
}

void criar_mapa(int mapa[N][N], int nivel){
    int j, i;

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            mapa[i][j] = 0;

            if(i==4 && j==4){
                mapa[i][j] = 2;
            }
        }
    }

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            if((i==0)&&(j>=2)&&(j<8)){
                mapa[i][j] = PAREDE;
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
            if((i>=1)&&(i<=4)&&(j==7)){
                mapa[i][j] = PAREDE;
            }
            if((i==3)&&(j<=2)){
                mapa[i][j] = PAREDE;
            }
            if((i>=4)&&(i<=5)&&(j==2)){
                mapa[i][j] = PAREDE;
            }
            if((i==4)&&(j==3)){
                mapa[i][j] = PAREDE;
            }
        }
    }

    mapa[3][4] = 3;
    mapa[5][5] = 3;

    mapa[2][1] = 4;
    mapa[6][2] = 4;
}

Estado ler_estado_inicial(int mapa[N][N]){
    Estado e;
    int i, j;

    e.qtd_caixas = 0;

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            if (mapa[i][j] == 2){
                mapa[i][j] = 0;
                e.jogador = i * N + j;
            }
            if (mapa[i][j] == 3){
                mapa[i][j] = 0;
                e.caixas[e.qtd_caixas] = i * N + j;
                e.qtd_caixas++;
            }
        }
    }

    return e;
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

int achar_caixa(Estado e, int posicao){
    int k;
    for(k = 0; k < e.qtd_caixas; k++){
        if(posicao == e.caixas[k]){
            return k;
        }
    }
    return SEM_CAIXA;
}

int tentar_mover(Estado atual, int mapa[N][N], int d, Estado *novo){
    int destino, k;

    *novo = atual;
    destino = atual.jogador + d;

    if (eh_parede(mapa, destino) == PAREDE){
        return 0;
    }

    k = achar_caixa(atual, destino);
    if (k != SEM_CAIXA){
        int atras = destino + d;
        if(eh_parede(mapa, atras) == PAREDE || achar_caixa(atual, atras) != SEM_CAIXA){
            return 0;
        }
        novo->caixas[k] = atras;
        ordenar_caixas(novo);
    }
    novo->jogador = destino;
    return 1;
}

int gerar_sucessores(Estado atual, int mapa[N][N], Estado filhos[4]){
    int desloc[4] = {-N, N, -1, 1};
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

int heuristica(Estado e, int mapa[N][N]){
    // TODO: soma da menor distancia de Manhattan de cada caixa ate um ALVO
    int h = 0;
    int linha_caixa, coluna_caixa, linha_alvo, coluna_alvo;
    for(int i = 0; i < e.qtd_caixas; i++){
        linha_caixa = e.caixas[i] / N;
        coluna_caixa = e.caixas[i] % N;
        int menor_distancia = INF;
        for(int linha = 0; linha < N; linha++){
            for(int coluna = 0; coluna < N; coluna++){
                if(mapa[linha][coluna] == ALVO){
                    linha_alvo = linha;
                    coluna_alvo = coluna;
                    int distancia = abs(linha_caixa - linha_alvo) + abs(coluna_caixa - coluna_alvo);
                    if(distancia < menor_distancia){
                        menor_distancia = distancia;
                    }
                }
            }
        }
        h += menor_distancia;
    }
    return h;
}

int parede_segura(int mapa[N][N], int pos){
    // TODO: 1 se pos estiver fora do mapa ou for parede
    if (pos < 0 || pos >= N * N) {
        return 1; // Fora do mapa
    }
    if (mapa[pos / N][pos % N] == PAREDE) {
        return 1; // É uma parede
    }

    return 0;
}

int eh_deadlock(Estado e, int mapa[N][N]){
    // TODO: 1 se alguma caixa fora de um alvo estiver presa em um canto
    for (int i = 0; i < e.qtd_caixas; i++){
        int pos = e.caixas[i];
        int linha = pos / N;
        int coluna = pos % N;
        if (mapa[linha][coluna] != ALVO){
            // Verifica se a caixa está em um canto
            if ((parede_segura(mapa, pos - 1) && parede_segura(mapa, pos - N)) || // Canto superior esquerdo
                (parede_segura(mapa, pos + 1) && parede_segura(mapa, pos - N)) || // Canto superior direito
                (parede_segura(mapa, pos - 1) && parede_segura(mapa, pos + N)) || // Canto inferior esquerdo
                (parede_segura(mapa, pos + 1) && parede_segura(mapa, pos + N))) { // Canto inferior direito
                return 1; // Deadlock encontrado
            }
        }
    }
    return 0;
}

int fechados_contem(No *fechados, Estado e){
    // TODO: percorrer a lista pelo prox usando estados_iguais
    No *p = fechados;
    for (; p != NULL; p = p->prox){
        if (estados_iguais(p->estado, e) == 1){
            return 1;
        }
    }
    return 0;
}

void resolver_astar(Estado inicial, int mapa[N][N]){
    // TODO: iniciar a fila, criar a raiz com f = g + h e inserir
    Fila fila;
    No *fechados = NULL, *solucao = NULL, *raiz, *atual, *novo, *p;
    Estado filhos[4];
    long expandidos = 0, gerados = 0;
    int qtd, i;
    clock_t t0, t1;
    double ms;
    //relogio começa junto com o algoritmo
    t0 = clock();
    fila_iniciar(&fila);
    raiz = criar_no(inicial, NULL, 0);
    raiz -> f = raiz -> g + heuristica(inicial, mapa);
    fila_inserir(&fila, raiz);
    gerados++;
   while(fila_vazia(&fila) == 0){
        atual = fila_remover(&fila);
        if(fechados_contem(fechados, atual->estado) == 1){
            free(atual);
            continue;
        }
        atual->prox = fechados;
        fechados = atual;
        expandidos++;
        if (eh_objetivo(atual->estado, mapa) == 1){
            solucao = atual;
            break;
        }
        qtd = gerar_sucessores(atual->estado, mapa, filhos);
        for (i = 0; i < qtd ; i++){
            if (eh_deadlock(filhos[i], mapa) == 1){
                continue;
            } else if (fechados_contem(fechados, filhos[i]) == 1){
                continue;
            } else {
                novo = criar_no(filhos[i], atual, 1);
                novo->f = novo->g + heuristica(filhos[i], mapa);
                fila_inserir(&fila, novo);
                gerados++;
            }
        }
   }
   t1 = clock();
   ms = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
   if (solucao != NULL){
        printf("Solução encontrada!\n");
        printf("Tempo de execução: %.2f ms\n", ms);
        printf("Nós expandidos: %ld\n", expandidos);
        printf("Nós gerados: %ld\n", gerados);
        printf("Custo da solução: %d\n", solucao->g);
   } else {
        printf("Solução não encontrada!\n");
        printf("Tempo de execução: %.2f ms\n", ms);
        printf("Nós expandidos: %ld\n", expandidos);
        printf("Nós gerados: %ld\n", gerados);
   }
   for(p = fechados; p != NULL;){
        No *temp = p;
        p = p->prox;
        free(temp);
   }
   free(fila_remover(&fila));
}

int main(){
    // Codigos: 0 = chao | 1 = parede | 2 = jogador | 3 = caixa | 4 = alvo
    int tabuleiro[N][N], i, j;

    criar_mapa(tabuleiro, 1);
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            printf("%d  ", tabuleiro[i][j]);
        }
        printf("\n");
    }

    Estado e = ler_estado_inicial(tabuleiro);
    printf("jogador: %d\n", e.jogador);
    printf("quantidade de caixas: %d\n", e.qtd_caixas);

    resolver_astar(e, tabuleiro);

    return 0;
}