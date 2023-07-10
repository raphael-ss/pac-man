#include <stdio.h>
#include <stdlib.h>

#define LINHAS 40
#define COLUNAS 100
#define MAX_JOGADAS 3000

typedef struct {
    int pos_x;
    int pos_y;
} tPosicao;

typedef struct {
    tPosicao posicao;
    char id;
    int ativo;
    int passou;
} tCelula;

typedef struct {
    char id;
    int colidiu;
    int comeu;
} tJogada;

typedef struct {
    tPosicao posicao;
    int pontos;
    tJogada jogadas[MAX_JOGADAS];
    int n_de_jogadas;
    int colidiu;
} tJogador;

typedef struct {
    char id;
    tPosicao posicao;
    int direcao;
    int ativo;
} tFantasma;

typedef struct {
    int linhas; //horizontal
    int colunas; //vertical
    int lim_movs;
    tCelula celulas[LINHAS][COLUNAS];
    int paredes, tuneis, comidas, fantasmas; 
} tMapa;


typedef struct {
    tMapa mapa;
    tJogador jogador;
    tFantasma fantasmas[4];
    int GameOver;
} tJogo;
    
typedef struct {
    /*
        ID = MOVIMENTO (W, A, S, D)
        STATS[3] = COMIDAS CONSUMIDAS, COLISOES, JOGADAS COM O MOV
    */
    char id;
    int comidas;
    int colisoes;
    int movs;

} tRank;

typedef struct {
    /*
        4 RANKS PARA CADA MOVIMENTO, CADA UM COM SUA RESPECTIVA CONTAGEM DE COMIDAS CONS., COLISOES E JOGADAS, E UM
        ID PARA IDENTIFICAR
    */
    tRank rank[4];
} tRanking;

// Funcoes de tPosicao
tPosicao PegaPosicaoPassada(tJogador jogador);
tPosicao PegaPosicaoAtual(tJogador jogador);
tPosicao PegaPosicaoPassadaFantasma(tFantasma fantasma);
int MesmaPosicao(tPosicao pos1, tPosicao pos2);
int FantasmaNaPosicaoPassada(tFantasma fantasmas[], tJogador jogador);

// Funcoes do tipo tRank e tRanking
tRank DefineRank(char id, int comidas, int colisoes, int movs);
tRanking OrdenaRanking(tRanking ranking);
void ImprimeRanking(tRanking ranking, char diretorio[]);

// Funcoes do tipo tJogada e tJogadas
int Colisoes(tJogador jogador, char id);
int Comidas(tJogador jogador, char id);
int MovsTotais(tJogador jogador, char id);
int MovsSemPontuar(tJogador jogador);
int ColisoesTotais(tJogador jogador);

// Funções do tipo tJogador
tJogador MoveCima(tJogador jogador); // W
tJogador MoveBaixo(tJogador jogador); // S
tJogador MoveDireita(tJogador jogador); // D
tJogador MoveEsquerda(tJogador jogador); // A
tJogador LeJogada(tJogador jogador);
tJogador ExecutaJogada(tJogador jogador, char jogada, tMapa mapa);
int Comeu(tJogador jogador, tMapa mapa);
int Colidiu(tJogador jogador);
char PegaUltimaJogada(tJogador jogador);
char PegaJogada(tJogador jogador, int i);

// Funções do tipo tFantasma
int FantasmaAtivo(tFantasma fantasma);

tFantasma InicializaB(tFantasma fantasma, int pos_y, int pos_x);
tFantasma InicializaP(tFantasma fantasma, int pos_y, int pos_x);
tFantasma InicializaI(tFantasma fantasma, int pos_y, int pos_x);
tFantasma InicializaC(tFantasma fantasma, int pos_y, int pos_x);

tFantasma MudouDirecaoB(tFantasma fantasmas, tMapa mapa);
tFantasma MudouDirecaoP(tFantasma fantasmas, tMapa mapa);
tFantasma MudouDirecaoI(tFantasma fantasmas, tMapa mapa);
tFantasma MudouDirecaoC(tFantasma fantasmas, tMapa mapa);

tFantasma MoveB(tFantasma fantasma);
tFantasma MoveP(tFantasma fantasma);
tFantasma MoveI(tFantasma fantasma);
tFantasma MoveC(tFantasma fantasma);

tFantasma DesativaFantasma(tFantasma fantasma);

// Funções do tipo tMapa:
tMapa LeMapa(char diretorio[]);
void ImprimeMapa(tJogo jogo, char diretorio[]);
int FantasmaBSobreComida(tMapa mapa, tFantasma fantasma);
int FantasmaPSobreComida(tMapa mapa, tFantasma fantasma);
int FantasmaISobreComida(tMapa mapa, tFantasma fantasma);
int FantasmaCSobreComida(tMapa mapa, tFantasma fantasma);
tMapa ZeraMapa(tMapa mapa);
tMapa ContaCelulas(tMapa mapa); // qtd de cada coisa
char PegaCelula(tMapa mapa, int y, int x);
tMapa DefineCelula(tMapa mapa, int y, int x, char cel);

// Funções do Jogo
tJogo PosicaoInicial(tJogo jogo);
tJogo FantasmasBateramNaParede(tJogo jogo);
tJogo MoveFantasmas(tJogo jogo);
int Morreu(tJogo jogo, tPosicao pos); 
void ExibeEstadoJogo(tJogo jogo, char diretorio[]);
int GameOver(tJogo jogo);
int Venceu(tJogo jogo);
tJogo Come(tJogo jogo);
tJogo RealizaJogada(tJogo jogo);
tJogo Passou(tJogo jogo, int y, int x);
void ExibeEstadoFinal(tJogo jogo, char diretorio[]);

// FUNÇÕES PRINCIPAIS:

tJogo Inicializar(tJogo jogo, char entrada[], char saida[]);
tJogo RealizarJogo(tJogo jogo, char diretorio[]);
void GeraResumo(tJogo jogo, char diretorio[]);
void GeraRanking(tJogo jogo, char diretorio[]);
void GeraEstatistica(tJogo jogo, char diretorio[]);
void GeraTrilha(tJogo jogo, char diretorio[]);

int main(int argc, char * argv[]) 
{
    char entrada[1000];
    char saida[1000];
    tJogo jogo;
    if (argc <= 1) {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado");
        exit(1);
    }

    sprintf(entrada, "%s/mapa.txt", argv[1]);
    sprintf(saida, "%s/saida1", argv[1]);

    jogo = Inicializar(jogo, entrada, saida);
    jogo = RealizarJogo(jogo, saida);
    GeraRanking(jogo, saida);
    GeraEstatistica(jogo, saida);
    GeraTrilha(jogo, saida);

    return 0;
}

/*
    Funções do tipo tJogador abaixo:
*/

int FantasmaNaPosicaoPassada(tFantasma fantasmas[], tJogador jogador) {
    if (FantasmaAtivo(fantasmas[0])) {
        if (MesmaPosicao(PegaPosicaoAtual(jogador), PegaPosicaoPassadaFantasma(fantasmas[0]))) {
            return 1;
        }
    }
    if (FantasmaAtivo(fantasmas[1])) {
        if (MesmaPosicao(PegaPosicaoAtual(jogador), PegaPosicaoPassadaFantasma(fantasmas[1]))) {
            return 1;
        }
    }
    if (FantasmaAtivo(fantasmas[2])) {
        if (MesmaPosicao(PegaPosicaoAtual(jogador), PegaPosicaoPassadaFantasma(fantasmas[2]))) {
            return 1;
        }
    }
    if (FantasmaAtivo(fantasmas[3])) {
        if (MesmaPosicao(PegaPosicaoAtual(jogador), PegaPosicaoPassadaFantasma(fantasmas[3]))) {
            return 1;
        }
    }

    return 0;
}

int MesmaPosicao(tPosicao pos1, tPosicao pos2) {
    if ((pos1.pos_x == pos2.pos_x) && (pos1.pos_y == pos2.pos_y)) {
        return 1;
    }
    return 0;
}

tPosicao PegaPosicaoPassadaFantasma(tFantasma fantasma) {
    tPosicao pos;
    if (fantasma.id == 'B') {
        if (fantasma.direcao == 0) {
            pos.pos_x = fantasma.posicao.pos_x+1;
            pos.pos_y = fantasma.posicao.pos_y;
            return pos;
        }
        else if (fantasma.direcao == 1) {
            pos.pos_x = fantasma.posicao.pos_x-1;
            pos.pos_y = fantasma.posicao.pos_y;
            return pos;
        }
    }
    else if (fantasma.id == 'P') {
        if (fantasma.direcao == 0) {
            pos.pos_x = fantasma.posicao.pos_x;
            pos.pos_y = fantasma.posicao.pos_y+1;
            return pos;
        }
        else if (fantasma.direcao == 1) {
            pos.pos_x = fantasma.posicao.pos_x;
            pos.pos_y = fantasma.posicao.pos_y-1;
            return pos;
        }
    }
    else if (fantasma.id == 'I') {
        if (fantasma.direcao == 0) {
            pos.pos_x = fantasma.posicao.pos_x;
            pos.pos_y = fantasma.posicao.pos_y-1;
            return pos;
        }
        else if (fantasma.direcao == 1) {
            pos.pos_x = fantasma.posicao.pos_x;
            pos.pos_y = fantasma.posicao.pos_y+1;
            return pos;
        }
    }
    else if (fantasma.id == 'C') {
        if (fantasma.direcao == 0) {
            pos.pos_x = fantasma.posicao.pos_x-1;
            pos.pos_y = fantasma.posicao.pos_y;
            return pos;
        }
        else if (fantasma.direcao == 1) {
            pos.pos_x = fantasma.posicao.pos_x+1;
            pos.pos_y = fantasma.posicao.pos_y;
            return pos;
        }
    }
}

tPosicao PegaPosicaoPassada(tJogador jogador) {
    tPosicao pos;
    if (PegaUltimaJogada(jogador) == 'w') {
        pos.pos_x = jogador.posicao.pos_x;
        pos.pos_y = jogador.posicao.pos_y+1;
        return pos;
    }
    else if (PegaUltimaJogada(jogador) == 'a') {
        pos.pos_x = jogador.posicao.pos_x+1;
        pos.pos_y = jogador.posicao.pos_y;
        return pos;
    }
    else if (PegaUltimaJogada(jogador) == 's') {
        pos.pos_x = jogador.posicao.pos_x;
        pos.pos_y = jogador.posicao.pos_y-1;
        return pos;
    }
    else if (PegaUltimaJogada(jogador) == 'd') {
        pos.pos_x = jogador.posicao.pos_x-1;
        pos.pos_y = jogador.posicao.pos_y;
        return pos;
    }
    
}

tPosicao PegaPosicaoAtual(tJogador jogador) {
    tPosicao pos;
    pos.pos_x = jogador.posicao.pos_x;
    pos.pos_y = jogador.posicao.pos_y;
    return pos;
}

tMapa DefineCelula(tMapa mapa, int y, int x, char cel) {
    mapa.celulas[y][x].id = cel;
    return mapa;
}

char PegaCelula(tMapa mapa, int y, int x) {
    return mapa.celulas[y][x].id;
    }

char PegaUltimaJogada(tJogador jogador) {
    return jogador.jogadas[jogador.n_de_jogadas].id;
}

char PegaJogada(tJogador jogador, int i) {
    return jogador.jogadas[i].id;
}

tFantasma DesativaFantasma(tFantasma fantasma) {
    fantasma.ativo = 0;
    return fantasma;
}

tJogo PosicaoInicial(tJogo jogo) {
    int i = 0, j = 0, aux = 0;
    tJogador jogador;

    jogo.fantasmas[0] = DesativaFantasma(jogo.fantasmas[0]);
    jogo.fantasmas[1] = DesativaFantasma(jogo.fantasmas[1]);
    jogo.fantasmas[2] = DesativaFantasma(jogo.fantasmas[2]);
    jogo.fantasmas[3] = DesativaFantasma(jogo.fantasmas[3]);

    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            if (PegaCelula(jogo.mapa, i, j) == '>') {

                jogo.mapa = DefineCelula(jogo.mapa, i, j, '\0');
                jogo.mapa.celulas[i][j].ativo = 0;
                jogo.mapa.celulas[i][j].passou = -100;

                jogo.jogador.posicao.pos_x = j;
                jogo.jogador.posicao.pos_y = i;

                jogo.jogador.pontos =  0;

                for (aux = 0; aux < 3000; aux++) {
                    jogo.jogador.jogadas[aux].id = '\0';
                    jogo.jogador.jogadas[aux].comeu = 0;
                    jogo.jogador.jogadas[aux].colidiu = 0;
                }
            }

            else if ((PegaCelula(jogo.mapa, i, j) == 'B') || (PegaCelula(jogo.mapa, i, j) == 'P') || (PegaCelula(jogo.mapa, i, j) == 'I') || (PegaCelula(jogo.mapa, i, j) == 'C')) {
                if (PegaCelula(jogo.mapa, i, j) == 'B') {

                    jogo.mapa = DefineCelula(jogo.mapa, i, j, '\0');
                    jogo.mapa.celulas[i][j].ativo = 0;

                    jogo.fantasmas[0] = InicializaB(jogo.fantasmas[0], i, j);
                }

                else if (PegaCelula(jogo.mapa, i, j) == 'P') {

                    jogo.mapa = DefineCelula(jogo.mapa, i, j, '\0');
                    jogo.mapa.celulas[i][j].ativo = 0;

                    jogo.fantasmas[1] = InicializaP(jogo.fantasmas[1], i, j);
                }

                else if (PegaCelula(jogo.mapa, i, j) == 'I') {

                    jogo.mapa = DefineCelula(jogo.mapa, i, j, '\0');
                    jogo.mapa.celulas[i][j].ativo = 0;

                    jogo.fantasmas[2] = InicializaI(jogo.fantasmas[2], i, j);
                }

                else if (PegaCelula(jogo.mapa, i, j) == 'C') {

                    jogo.mapa = DefineCelula(jogo.mapa, i, j, '\0');
                    jogo.mapa.celulas[i][j].ativo = 0;

                    jogo.fantasmas[3] = InicializaC(jogo.fantasmas[3], i, j);
                }
            }
        }
        
    }
    return jogo;
}

tJogador MoveCima(tJogador jogador) {
    jogador.posicao.pos_y -= 1;
    return jogador;
}

tJogador MoveBaixo(tJogador jogador) {
    jogador.posicao.pos_y += 1;
    return jogador;
}

tJogador MoveDireita(tJogador jogador) {
    jogador.posicao.pos_x += 1;
    return jogador;
}

tJogador MoveEsquerda(tJogador jogador) {
    jogador.posicao.pos_x -= 1;
    return jogador;
}

tJogador LeJogada(tJogador jogador) {
    scanf("%*[^wasd]");
    scanf("%c", &jogador.jogadas[jogador.n_de_jogadas].id);
    return jogador;
}

tJogador ExecutaJogada(tJogador jogador, char jogada, tMapa mapa) {
    int x = jogador.posicao.pos_x;
    int y = jogador.posicao.pos_y;

    if (jogada == 'w' && mapa.celulas[y-1][x].id != '#') {
        jogador = MoveCima(jogador);
    }

    else if (jogada == 'a' && mapa.celulas[y][x-1].id != '#') {
        jogador = MoveEsquerda(jogador);
    }

    else if (jogada == 's' && mapa.celulas[y+1][x].id != '#') {
        jogador = MoveBaixo(jogador);
    }

    else if (jogada == 'd' && mapa.celulas[y][x+1].id != '#') {
        jogador = MoveDireita(jogador);
    }

    else {
        jogador.colidiu = 1;
        jogador.jogadas[jogador.n_de_jogadas].colidiu = 1;
    }

    return jogador;
}

int Comeu(tJogador jogador, tMapa mapa) {
    int x, y;
    x = jogador.posicao.pos_x;
    y = jogador.posicao.pos_y;

    // Se o Pac-Man passar encima da comida, aumente sua pontuação e desative a comida;

    if ((mapa.celulas[y][x].id == '*') && (mapa.celulas[y][x].ativo)) {
        return 1;
    }

    //jogador.jogadas[jogador.n_de_jogadas].comeu++;

    return 0;

}

int Colidiu(tJogador jogador) {
    return jogador.colidiu;
}

int Colisoes(tJogador jogador, char id) {
    int i = 0, cont = 0;
    for (i = 0; i < jogador.n_de_jogadas; i++) {
        if (id == jogador.jogadas[i].id) {
            if (jogador.jogadas[i].colidiu) {
                cont++;
            }
        }
    }
    return cont;
}

int Comidas(tJogador jogador, char id) {
    int i = 0, cont = 0;
    for (i = 0; i < jogador.n_de_jogadas; i++) {
        if (id == jogador.jogadas[i].id) {
            if (jogador.jogadas[i].comeu == 1) {
                cont++;
            }
        }
    }
    return cont;
}

int MovsTotais(tJogador jogador, char id) {
    int i = 0, cont = 0;
    for (i = 0; i < jogador.n_de_jogadas; i++) {
        if (id == jogador.jogadas[i].id) {
            cont++; 
        }
    }
    return cont;
}

int MovsSemPontuar(tJogador jogador) {
    int i = 0, cont = 0;
    for (i = 0; i < jogador.n_de_jogadas; i++) {
        if (!jogador.jogadas[i].comeu) {
            cont++;
        }
    }
    return cont;
}

int ColisoesTotais(tJogador jogador) {
    int i = 0, cont = 0;
    for (i = 0; i < jogador.n_de_jogadas; i++) {
        if (jogador.jogadas[i].colidiu) {
            cont++;
        }
    }
    return cont;
}

/*
    Funções do tipo tFantasma abaixo:
*/

tFantasma InicializaB(tFantasma fantasma, int pos_y, int pos_x) {
    fantasma.ativo = 1;
    fantasma.direcao = 0;
    fantasma.id = 'B';
    fantasma.posicao.pos_x = pos_x;
    fantasma.posicao.pos_y = pos_y;
    return fantasma;
}

tFantasma InicializaP(tFantasma fantasma, int pos_y, int pos_x) {
    fantasma.ativo = 1;
    fantasma.direcao = 0;
    fantasma.id = 'P';
    fantasma.posicao.pos_x = pos_x;
    fantasma.posicao.pos_y = pos_y;
    return fantasma;
}

tFantasma InicializaI(tFantasma fantasma, int pos_y, int pos_x) {
    fantasma.ativo = 1;
    fantasma.direcao = 0;
    fantasma.id = 'I';
    fantasma.posicao.pos_x = pos_x;
    fantasma.posicao.pos_y = pos_y;
    return fantasma;
}

tFantasma InicializaC(tFantasma fantasma, int pos_y, int pos_x) {
    fantasma.ativo = 1;
    fantasma.direcao = 0;
    fantasma.id = 'C';
    fantasma.posicao.pos_x = pos_x;
    fantasma.posicao.pos_y = pos_y;
    return fantasma;
}

int FantasmaAtivo(tFantasma fantasma){
    return fantasma.ativo;

}

tFantasma MudouDirecaoB(tFantasma fantasma, tMapa mapa) {
    // se B estiver ativo
    if (FantasmaAtivo(fantasma)) {
        // trocou de direcao
        if (fantasma.direcao == 1) {
            int x = fantasma.posicao.pos_x;
            int y = fantasma.posicao.pos_y;
            if (mapa.celulas[y][x+1].id == '#') {
                fantasma.direcao = 0;
            }
        }
        // nao trocou de direcao
        else if (fantasma.direcao == 0) {
            int x = fantasma.posicao.pos_x;
            int y = fantasma.posicao.pos_y;
            if (mapa.celulas[y][x-1].id == '#') {
                fantasma.direcao = 1;
            }
        }
    }
    return fantasma;
}

tFantasma MudouDirecaoP(tFantasma fantasma, tMapa mapa) {
    // se P estiver ativo
    if (FantasmaAtivo(fantasma)) {
        // trocou de direcao
        if (fantasma.direcao == 1) {
            int x = fantasma.posicao.pos_x;
            int y = fantasma.posicao.pos_y;
            // se o próximo for parede
            if (mapa.celulas[y+1][x].id == '#') {
                fantasma.direcao = 0;           
            }
        }
        // nao trocou de direcao
        else if (fantasma.direcao == 0) {
            int x = fantasma.posicao.pos_x;
            int y = fantasma.posicao.pos_y;
            // se o próximo for parede
            if (mapa.celulas[y-1][x].id == '#') {
                fantasma.direcao = 1;              
            }
        }
    }
    return fantasma;
}

tFantasma MudouDirecaoI(tFantasma fantasma, tMapa mapa) {
    // se I estiver ativo
    if (FantasmaAtivo(fantasma)) {
        // trocou de direcao
        if (fantasma.direcao == 1) {
            int x = fantasma.posicao.pos_x;
            int y = fantasma.posicao.pos_y;
            // se o próximo for parede
            if (mapa.celulas[y-1][x].id == '#') {
                fantasma.direcao = 0;
            }
        }
        // nao trocou de direcao
        else if (fantasma.direcao == 0) {
            int x = fantasma.posicao.pos_x;
            int y = fantasma.posicao.pos_y;
            // se o próximo for parede
            if (mapa.celulas[y+1][x].id == '#') {
                fantasma.direcao = 1;
            }
        }
    }
    return fantasma;
}

tFantasma MudouDirecaoC(tFantasma fantasma, tMapa mapa) {
    // se I estiver ativo
    if (FantasmaAtivo(fantasma)) {
        // trocou de direcao
        if (fantasma.direcao == 1) {
            int x = fantasma.posicao.pos_x;
            int y = fantasma.posicao.pos_y;
            // se o próximo for parede
            if (mapa.celulas[y][x-1].id == '#') {
                fantasma.direcao = 0;
            }
        }
        // nao trocou de direcao
        else if (fantasma.direcao == 0) {
            int x = fantasma.posicao.pos_x;
            int y = fantasma.posicao.pos_y;
            // se o próximo for parede
            if (mapa.celulas[y][x+1].id == '#') {
                fantasma.direcao = 1;
            }
        }
    }
    return fantasma;
}

tFantasma MoveB(tFantasma fantasma) {

    if (FantasmaAtivo(fantasma)) {

       if (fantasma.direcao == 1) {
            fantasma.posicao.pos_x += 1;
       }

       else if (fantasma.direcao == 0){
            fantasma.posicao.pos_x -= 1;
        }
    }
    return fantasma;
}

tFantasma MoveP(tFantasma fantasma) {

    if (FantasmaAtivo(fantasma)) {

       if (fantasma.direcao == 1) {
            fantasma.posicao.pos_y += 1;
       }

       else if (fantasma.direcao == 0){
            fantasma.posicao.pos_y -= 1;
        }
    }
    return fantasma;
}

tFantasma MoveI(tFantasma fantasma) {

    if (FantasmaAtivo(fantasma)) {

       if (fantasma.direcao == 1) {
            fantasma.posicao.pos_y -= 1;
       }

       else if (fantasma.direcao == 0){
            fantasma.posicao.pos_y += 1;
        }
    }
    return fantasma;
}

tFantasma MoveC(tFantasma fantasma) {

    if (FantasmaAtivo(fantasma)) {

       if (fantasma.direcao == 1) {
            fantasma.posicao.pos_x -= 1;
       }

       else if (fantasma.direcao == 0){
            fantasma.posicao.pos_x += 1;
        }
    }
    return fantasma;
}

/*
    Funções do Tipo tMapa abaixo:
*/

tMapa LeMapa(char diretorio[]) {
    FILE * pFile;
    tMapa mapa;
    int j = 0, i = 0;
    pFile = fopen (diretorio, "r");
    if (!pFile) {
        printf("ERRO: Arquivo %s nao foi encontrado", diretorio);
        exit(1);
    }
    fscanf(pFile, "%d %d %d", &mapa.linhas, &mapa.colunas, &mapa.lim_movs);

    mapa = ZeraMapa(mapa);

    fscanf(pFile, "%*[^\n]");
    fscanf(pFile, "%*c");
    while (!feof(pFile)) {
        for (i = 0; i < mapa.linhas; i++) {
        for (j = 0; j < mapa.colunas; j++) {
            fscanf(pFile, "%c", &mapa.celulas[i][j].id);
            mapa.celulas[i][j].ativo = 1;
            mapa.celulas[i][j].posicao.pos_x = j;
            mapa.celulas[i][j].posicao.pos_y = i;
            mapa.celulas[i][j].passou = 0;
        }
        fscanf(pFile, "%*[^\n]");
        fscanf(pFile, "%*c");
    }
    }
    fclose(pFile);
    mapa = ContaCelulas(mapa);
    return mapa;
}

void ImprimeMapa(tJogo jogo, char diretorio[]) {
    int i = 0, j = 0;
    FILE * pFile;
    char saida[1000];
    sprintf(saida, "%s/inicializacao.txt", diretorio);
    pFile = fopen(saida, "w");

    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            if ((i == jogo.fantasmas[0].posicao.pos_y) && (j == jogo.fantasmas[0].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[0]))) {
                fprintf(pFile, "%c", jogo.fantasmas[0].id);
                continue;
            }

            else if ((i == jogo.fantasmas[1].posicao.pos_y) && (j == jogo.fantasmas[1].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[1]))) {
                fprintf(pFile, "%c", jogo.fantasmas[1].id);
                continue;
            }

            else if ((i == jogo.fantasmas[2].posicao.pos_y) && (j == jogo.fantasmas[2].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[2]))) {
                fprintf(pFile, "%c", jogo.fantasmas[2].id);
                continue;
            }

            else if ((i == jogo.fantasmas[3].posicao.pos_y) && (j == jogo.fantasmas[3].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[3]))) {
                fprintf(pFile, "%c", jogo.fantasmas[3].id);
                continue;
            }

            else if ((i == jogo.jogador.posicao.pos_y) && (j == jogo.jogador.posicao.pos_x)) {
                fprintf(pFile, ">");
                continue;
            }

            else if (jogo.mapa.celulas[i][j].ativo) {
                fprintf(pFile, "%c", PegaCelula(jogo.mapa, i, j));
            }

            else if (!(jogo.mapa.celulas[i][j].ativo)) {
                fprintf(pFile, " ");
            }
        }
        fprintf(pFile, "\n");
    }
    fprintf(pFile, "Pac-Man comecara o jogo na linha %d e coluna %d\n", jogo.jogador.posicao.pos_y+1, jogo.jogador.posicao.pos_x+1);
    fclose(pFile);
    return;
}

int FantasmaBSobreComida(tMapa mapa, tFantasma fantasma) {
    if (FantasmaAtivo(fantasma)) {
        int x = fantasma.posicao.pos_x;
        int y = fantasma.posicao.pos_y;

        if (mapa.celulas[y][x].id == '*') {
            return 1;
        }
    }
    return 0;
}

int FantasmaPSobreComida(tMapa mapa, tFantasma fantasma) {
    if (FantasmaAtivo(fantasma)) {
        int x = fantasma.posicao.pos_x;
        int y = fantasma.posicao.pos_y;

        if (mapa.celulas[y][x].id == '*') {
            return 1;
        }
    }
    return 0;
}

int FantasmaISobreComida(tMapa mapa, tFantasma fantasma) {
    if (FantasmaAtivo(fantasma)) {
        int x = fantasma.posicao.pos_x;
        int y = fantasma.posicao.pos_y;

        if (mapa.celulas[y][x].id == '*') {
            return 1;
        }
    }
    return 0;
}

int FantasmaCSobreComida(tMapa mapa, tFantasma fantasma) {
    if (FantasmaAtivo(fantasma)) {
        int x = fantasma.posicao.pos_x;
        int y = fantasma.posicao.pos_y;

        if (mapa.celulas[y][x].id == '*') {
            return 1;
        }
    }
    return 0;
}

tMapa ZeraMapa(tMapa mapa) {
    int i = 0, j = 0;
    mapa.comidas = 0;
    mapa.tuneis = 0;
    mapa.paredes = 0;
    mapa.fantasmas = 0;
    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            mapa.celulas[i][j].id = '\0';
            mapa.celulas[i][j].ativo = 0;
        }
    }
    return mapa;
}

tMapa ContaCelulas(tMapa mapa) {
    int comidas = 0, tuneis = 0, paredes = 0, fantasmas = 0;
    int i = 0, j = 0;
    for (i = 0; i < mapa.linhas; i++) {
        for (j = 0; j < mapa.colunas; j++) {
            if (mapa.celulas[i][j].id == '*') {
                comidas++;
            }
            else if (mapa.celulas[i][j].id == '@') {
                tuneis++;
            }
            else if (mapa.celulas[i][j].id == '#') {
                paredes++;
            }
            else if (mapa.celulas[i][j].id == 'B' || mapa.celulas[i][j].id == 'C' || mapa.celulas[i][j].id == 'P' || mapa.celulas[i][j].id == 'I') {
                fantasmas++;
            }
        }
    }
    mapa.comidas = comidas;
    mapa.tuneis = tuneis;
    mapa.paredes = paredes;
    mapa.fantasmas = fantasmas;
    return mapa;
}

/*
    Funções do tipo tJogo abaixo:
*/
tJogo Passou(tJogo jogo, int y, int x) {
    jogo.mapa.celulas[y][x].passou = jogo.jogador.n_de_jogadas+1;
    return jogo;
}

tJogo Come(tJogo jogo) {
    tPosicao pos;
    pos = PegaPosicaoAtual(jogo.jogador);

    // Se o Pac-Man passar encima da comida, aumente sua pontuação e desative a comida;

    //if ((PegaCelula(jogo.mapa, pos.pos_y, pos.pos_x) == '*') && (jogo.mapa.celulas[pos.pos_y][pos.pos_x].ativo) && (!GameOver(jogo))) {
    if (Comeu(jogo.jogador, jogo.mapa)) {
        jogo.jogador.pontos++;
        jogo.mapa.celulas[pos.pos_y][pos.pos_x].ativo = 0;
        jogo.jogador.jogadas[jogo.jogador.n_de_jogadas].comeu = 1;
        return jogo;
    }

    jogo.jogador.jogadas[jogo.jogador.n_de_jogadas].comeu = 0;

    return jogo;
}

tJogo FantasmasBateramNaParede(tJogo jogo) {
    jogo.fantasmas[0] = MudouDirecaoB(jogo.fantasmas[0], jogo.mapa);
    jogo.fantasmas[1] = MudouDirecaoP(jogo.fantasmas[1], jogo.mapa);
    jogo.fantasmas[2] = MudouDirecaoI(jogo.fantasmas[2], jogo.mapa);
    jogo.fantasmas[3] = MudouDirecaoC(jogo.fantasmas[3], jogo.mapa);
    return jogo;
}

void ExibeEstadoJogo(tJogo jogo, char diretorio[]) {
    FILE * pFile;
    char saida[1000];
    sprintf(saida, "%s/saida.txt", diretorio);
    pFile = fopen(saida, "a");

    printf("Estado do jogo apos o movimento '%c':\n", jogo.jogador.jogadas[jogo.jogador.n_de_jogadas-1].id);
    fprintf(pFile, "Estado do jogo apos o movimento '%c':\n", jogo.jogador.jogadas[jogo.jogador.n_de_jogadas-1].id);
    int i = 0, j = 0;
    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            if ((i == jogo.fantasmas[0].posicao.pos_y) && (j == jogo.fantasmas[0].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[0]))) {
                printf("%c", jogo.fantasmas[0].id);
                fprintf(pFile, "%c", jogo.fantasmas[0].id);
                continue;
            }

            else if ((i == jogo.fantasmas[1].posicao.pos_y) && (j == jogo.fantasmas[1].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[1]))) {
                printf("%c", jogo.fantasmas[1].id);
                fprintf(pFile, "%c", jogo.fantasmas[1].id);
                continue;
            }

            else if ((i == jogo.fantasmas[2].posicao.pos_y) && (j == jogo.fantasmas[2].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[2]))) {
                printf("%c", jogo.fantasmas[2].id);
                fprintf(pFile, "%c", jogo.fantasmas[2].id);
                continue;
            }

            else if ((i == jogo.fantasmas[3].posicao.pos_y) && (j == jogo.fantasmas[3].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[3]))) {
                printf("%c", jogo.fantasmas[3].id);
                fprintf(pFile, "%c", jogo.fantasmas[3].id);
                continue;
            }

            else if ((i == jogo.jogador.posicao.pos_y) && (j == jogo.jogador.posicao.pos_x)) {
                printf(">");
                fprintf(pFile, ">");
                continue;
            }

            else if (jogo.mapa.celulas[i][j].ativo) {
                printf("%c", PegaCelula(jogo.mapa, i, j));
                fprintf(pFile, "%c", PegaCelula(jogo.mapa, i, j));
            }

            else if (!(jogo.mapa.celulas[i][j].ativo)) {
                printf(" ");
                fprintf(pFile, " ");
            }
        }
        printf("\n");
        fprintf(pFile, "\n");
    }
    fprintf(pFile, "Pontuacao: %d\n\n", jogo.jogador.pontos);
    printf("Pontuacao: %d\n\n", jogo.jogador.pontos);

    fclose(pFile);

    return;
}

int Morreu(tJogo jogo, tPosicao pos) {

    if (((pos.pos_x == jogo.fantasmas[0].posicao.pos_x) && FantasmaAtivo(jogo.fantasmas[0]))) {
        if (pos.pos_y == jogo.fantasmas[0].posicao.pos_y) {
            return 1;
        }
    }

    if ((pos.pos_x == jogo.fantasmas[1].posicao.pos_x) && FantasmaAtivo(jogo.fantasmas[1])) {
        if (pos.pos_y == jogo.fantasmas[1].posicao.pos_y) {
            return 1;
        }
    }

    if ((pos.pos_x == jogo.fantasmas[2].posicao.pos_x) && FantasmaAtivo(jogo.fantasmas[2])) {
        if (pos.pos_y == jogo.fantasmas[2].posicao.pos_y) {
            return 1;
        }
    }

    if ((pos.pos_x == jogo.fantasmas[3].posicao.pos_x) && FantasmaAtivo(jogo.fantasmas[3])) {
        if (pos.pos_y == jogo.fantasmas[3].posicao.pos_y) {
            return 1;
        }
    }

    return 0;
}

int GameOver(tJogo jogo) {
    if (Morreu(jogo, PegaPosicaoAtual(jogo.jogador))) {
        return 1;
    }
/*
    if (jogo.jogador.n_de_jogadas >= jogo.mapa.lim_movs) {
        return 1;
    }
*/
    return 0;

}

tJogo MoveFantasmas(tJogo jogo) {
// fazer move B e substituir por espaço
    jogo = FantasmasBateramNaParede(jogo);

    jogo.fantasmas[0] = MoveB(jogo.fantasmas[0]);
    jogo.fantasmas[1] = MoveP(jogo.fantasmas[1]);
    jogo.fantasmas[2] = MoveI(jogo.fantasmas[2]);
    jogo.fantasmas[3] = MoveC(jogo.fantasmas[3]);

    if (Morreu(jogo, PegaPosicaoAtual(jogo.jogador))) {
        jogo.GameOver += 1;
    }
    return jogo;
}

int Venceu(tJogo jogo) {
    if (jogo.jogador.pontos == jogo.mapa.comidas) {
        return 1;
    }

    return 0;
}

tJogo RealizaJogada(tJogo jogo) {
    jogo.jogador = LeJogada(jogo.jogador);
    char jogada = PegaUltimaJogada(jogo.jogador);
    jogo.jogador = ExecutaJogada(jogo.jogador, jogada, jogo.mapa);
    if (Morreu(jogo, PegaPosicaoPassada(jogo.jogador)) && (FantasmaNaPosicaoPassada(jogo.fantasmas, jogo.jogador))) {
        jogo.GameOver += 1;
        return jogo;
    }
    int x = jogo.jogador.posicao.pos_x;
    int y = jogo.jogador.posicao.pos_y;
    if (!GameOver(jogo)) {
        jogo = Passou(jogo, y, x);
    }
    return jogo;
}

void ExibeEstadoFinal(tJogo jogo, char diretorio[]) {
    FILE * pFile;
    char saida[1000];
    sprintf(saida, "%s/saida.txt", diretorio);
    pFile = fopen(saida, "a");

    printf("Estado do jogo apos o movimento '%c':\n", jogo.jogador.jogadas[jogo.jogador.n_de_jogadas-1].id);
    fprintf(pFile, "Estado do jogo apos o movimento '%c':\n", jogo.jogador.jogadas[jogo.jogador.n_de_jogadas-1].id);
    int i = 0, j = 0;
    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            if ((i == jogo.fantasmas[0].posicao.pos_y) && (j == jogo.fantasmas[0].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[0]))) {
                printf("%c", jogo.fantasmas[0].id);
                fprintf(pFile, "%c", jogo.fantasmas[0].id);
                continue;
            }

            else if ((i == jogo.fantasmas[1].posicao.pos_y) && (j == jogo.fantasmas[1].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[1]))) {
                printf("%c", jogo.fantasmas[1].id);
                fprintf(pFile, "%c", jogo.fantasmas[1].id);
                continue;
            }

            else if ((i == jogo.fantasmas[2].posicao.pos_y) && (j == jogo.fantasmas[2].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[2]))) {
                printf("%c", jogo.fantasmas[2].id);
                fprintf(pFile, "%c", jogo.fantasmas[2].id);
                continue;
            }

            else if ((i == jogo.fantasmas[3].posicao.pos_y) && (j == jogo.fantasmas[3].posicao.pos_x) && (FantasmaAtivo(jogo.fantasmas[3]))) {
                printf("%c", jogo.fantasmas[3].id);
                fprintf(pFile, "%c", jogo.fantasmas[3].id);
                continue;
            }

            else if (jogo.mapa.celulas[i][j].ativo) {
                printf("%c", PegaCelula(jogo.mapa, i, j));
                fprintf(pFile, "%c", PegaCelula(jogo.mapa, i, j));
            }

            else if (!(jogo.mapa.celulas[i][j].ativo)) {
                printf(" ");
                fprintf(pFile, " ");
            }
        }
        printf("\n");
        fprintf(pFile, "\n");
    }
    fprintf(pFile, "Pontuacao: %d\n\n", jogo.jogador.pontos);
    printf("Pontuacao: %d\n\n", jogo.jogador.pontos);

    fclose(pFile);
    
    return;
}

/*
    Funções Principais:
*/

tJogo Inicializar(tJogo jogo, char entrada[], char saida[]) {
    jogo.mapa = LeMapa(entrada);
    jogo = PosicaoInicial(jogo);
    ImprimeMapa(jogo, saida);
    jogo.GameOver = 0;
    return jogo;
}

tJogo RealizarJogo(tJogo jogo, char diretorio[]) {
    int i = 0, j = 0;

    jogo.jogador.n_de_jogadas = 0;
    for (i = 0; i < jogo.mapa.lim_movs; i++) {
        jogo.GameOver = 0;
        if (Venceu(jogo)) {
            printf("Voce venceu!\nPontuação final: %d\n", jogo.jogador.pontos);
            break;
        }
        jogo = FantasmasBateramNaParede(jogo);
        jogo = MoveFantasmas(jogo);
        jogo.jogador.colidiu = 0;
        jogo = RealizaJogada(jogo);
        GeraResumo(jogo, diretorio);
        if ((jogo.GameOver == 2) || GameOver(jogo)) {
            jogo.jogador.n_de_jogadas++;
            if (jogo.GameOver == 2) {
                ExibeEstadoFinal(jogo, diretorio);
            }
            else if (GameOver(jogo)) {
                ExibeEstadoJogo(jogo, diretorio);
            }
            printf("Game over!\nPontuação final: %d\n", jogo.jogador.pontos);
            return jogo;
        }

        jogo = Come(jogo);
        jogo.jogador.n_de_jogadas++;
        ExibeEstadoJogo(jogo, diretorio);
    }
    
    if (i == jogo.mapa.lim_movs) {
        printf("Game over!\nPontuação final: %d\n", jogo.jogador.pontos);
    }
    return jogo;
}

void GeraResumo(tJogo jogo, char diretorio[]) {
    char jogada = jogo.jogador.jogadas[jogo.jogador.n_de_jogadas].id;

    FILE * pFile;
    char saida[1000];
    sprintf(saida, "%s/resumo.txt", diretorio);
    pFile = fopen(saida, "a");

    if ((jogo.GameOver == 2) || (GameOver(jogo))) {
        fprintf(pFile, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", jogo.jogador.n_de_jogadas+1, jogada);
    }

    else if (Comeu(jogo.jogador, jogo.mapa)) {
        fprintf(pFile, "Movimento %d (%c) pegou comida\n", jogo.jogador.n_de_jogadas+1, jogada);
    }

    else if (Colidiu(jogo.jogador) == 1) {
        fprintf(pFile, "Movimento %d (%c) colidiu na parede\n", jogo.jogador.n_de_jogadas+1, jogada);
    }

    fclose(pFile);

    return;
}

tRank DefineRank(char id, int comidas, int colisoes, int movs) {
    tRank rank;
    rank.id = id;
    rank.comidas = comidas;
    rank.colisoes = colisoes;
    rank.movs = movs;
    return rank;
}

tRanking OrdenaRanking(tRanking ranking) {
    int i = 0, sorted = 1;
    tRank aux;
    while (sorted != 4) {
        sorted = 1;
        for (i = 1; i < 4; i++) {
            // se a proxima for maior, troque
            if (ranking.rank[i-1].comidas < ranking.rank[i].comidas) {
                aux = ranking.rank[i-1];
                ranking.rank[i-1] = ranking.rank[i];
                ranking.rank[i] = aux;
            }
            else if (ranking.rank[i-1].comidas == ranking.rank[i].comidas) {
                if (ranking.rank[i-1].colisoes > ranking.rank[i].colisoes) {
                    aux = ranking.rank[i-1];
                    ranking.rank[i-1] = ranking.rank[i];
                    ranking.rank[i] = aux;
                }
                else if (ranking.rank[i-1].colisoes == ranking.rank[i].colisoes) {
                    if (ranking.rank[i-1].movs < ranking.rank[i].movs) {
                        aux = ranking.rank[i-1];
                        ranking.rank[i-1] = ranking.rank[i];
                        ranking.rank[i] = aux;
                    }
                    else if (ranking.rank[i-1].movs == ranking.rank[i].movs) {
                        if (ranking.rank[i-1].id > ranking.rank[i].id) {
                            aux = ranking.rank[i-1];
                            ranking.rank[i-1] = ranking.rank[i];
                            ranking.rank[i] = aux;
                        }
                        else {
                            sorted++;
                            continue;
                        }
                    }
                    else {
                        sorted++;
                        continue;
                    }
                }
                else {
                    sorted++;
                    continue;
                }
            }
            else {
                sorted++;
                continue;
            }
        }
    }
    return ranking;
}

void ImprimeRanking(tRanking ranking, char diretorio[]) {
    FILE * pFile;
    char saida[1000];
    sprintf(saida, "%s/ranking.txt", diretorio);
    pFile = fopen(saida, "w");
    fprintf(pFile, "%c,%d,%d,%d\n", ranking.rank[0].id, ranking.rank[0].comidas, ranking.rank[0].colisoes, ranking.rank[0].movs);
    fprintf(pFile, "%c,%d,%d,%d\n", ranking.rank[1].id, ranking.rank[1].comidas, ranking.rank[1].colisoes, ranking.rank[1].movs);
    fprintf(pFile, "%c,%d,%d,%d\n", ranking.rank[2].id, ranking.rank[2].comidas, ranking.rank[2].colisoes, ranking.rank[2].movs);
    fprintf(pFile, "%c,%d,%d,%d\n", ranking.rank[3].id, ranking.rank[3].comidas, ranking.rank[3].colisoes, ranking.rank[3].movs);
    fclose(pFile);
    return;
}

void GeraRanking(tJogo jogo, char diretorio[]) {
    tRanking ranking;
    ranking.rank[0] = DefineRank('w', Comidas(jogo.jogador, 'w'), Colisoes(jogo.jogador, 'w'), MovsTotais(jogo.jogador, 'w'));
    ranking.rank[1] = DefineRank('a', Comidas(jogo.jogador, 'a'), Colisoes(jogo.jogador, 'a'), MovsTotais(jogo.jogador, 'a'));
    ranking.rank[2] = DefineRank('s', Comidas(jogo.jogador, 's'), Colisoes(jogo.jogador, 's'), MovsTotais(jogo.jogador, 's'));
    ranking.rank[3] = DefineRank('d', Comidas(jogo.jogador, 'd'), Colisoes(jogo.jogador, 'd'), MovsTotais(jogo.jogador, 'd'));

    ranking = OrdenaRanking(ranking);

    ImprimeRanking(ranking, diretorio);

    return;
}

void GeraEstatistica(tJogo jogo, char diretorio[]) {
    FILE * pFile;
    char saida[1000];
    sprintf(saida, "%s/estatisticas.txt", diretorio);
    pFile = fopen(saida, "w");
    fprintf(pFile, "Numero de movimentos: %d\n", jogo.jogador.n_de_jogadas);
    fprintf(pFile, "Numero de movimentos sem pontuar: %d\n", MovsSemPontuar(jogo.jogador));
    fprintf(pFile, "Numero de colisoes com parede: %d\n", ColisoesTotais(jogo.jogador));
    fprintf(pFile, "Numero de movimentos para baixo: %d\n", MovsTotais(jogo.jogador, 's'));
    fprintf(pFile, "Numero de movimentos para cima: %d\n", MovsTotais(jogo.jogador, 'w'));
    fprintf(pFile, "Numero de movimentos para esquerda: %d\n", MovsTotais(jogo.jogador, 'a'));
    fprintf(pFile, "Numero de movimentos para direita: %d\n", MovsTotais(jogo.jogador, 'd'));
    fclose(pFile);
    return;
}

void GeraTrilha(tJogo jogo, char diretorio[]) {
    int i = 0, j = 0;

    FILE * pFile;
    char saida[1000];
    sprintf(saida, "%s/trilha.txt", diretorio);
    pFile = fopen(saida, "w");

    for (i = 0; i < jogo.mapa.linhas; i++) {
        for (j = 0; j < jogo.mapa.colunas; j++) {
            if (j > 0) {
                fprintf(pFile, " ");    
            }
            if (jogo.mapa.celulas[i][j].passou == 0) {
                fprintf(pFile, "#");
            }
            else if (jogo.mapa.celulas[i][j].passou > 0) {
                fprintf(pFile, "%d", jogo.mapa.celulas[i][j].passou);
            }
            else {
                fprintf(pFile, "0");
            }
        }
        fprintf(pFile, "\n");
    }
    fclose(pFile);
    return;
}
