#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Constantes que eu uso para evitar números e textos soltos pelo código. */
#define TOTAL_TERRITORIOS 5
#define TOTAL_MISSOES 2
#define TAMANHO_NOME 50
#define COR_JOGADOR "Azul"
#define COR_ALVO "Verde"

/* Cada território guarda as informações principais do mapa do jogo. */
typedef struct {
    char nome[TAMANHO_NOME];
    char cor[TAMANHO_NOME];
    int tropas;
} Territorio;

/* Protótipos das funções para organizar melhor o programa. */
Territorio *alocarMapa(int quantidade);
void inicializarTerritorios(Territorio *mapa, int quantidade);
void liberarMemoria(Territorio *mapa);

void exibirMenuPrincipal(void);
void exibirMapa(const Territorio *mapa, int quantidade);
void exibirMissao(int missao);

void faseDeAtaque(Territorio *mapa, int quantidade);
void simularAtaque(Territorio *atacante, Territorio *defensor);

int sortearMissao(void);
int verificarVitoria(const Territorio *mapa, int quantidade, int missao, const char *corJogador);
int contarTerritoriosPorCor(const Territorio *mapa, int quantidade, const char *cor);
int lerInteiro(const char *mensagem);
void limparBufferEntrada(void);

int main(void) {
    /* Essas variáveis controlam o mapa, a missão sorteada e o loop do menu. */
    Territorio *mapa;
    int missao;
    int opcao;
    int venceu = 0;

    /* Inicializo a aleatoriedade para os dados das batalhas e para a missão. */
    srand((unsigned int) time(NULL));

    /* O mapa é alocado dinamicamente, como foi pedido no nível aventureiro/mestre. */
    mapa = alocarMapa(TOTAL_TERRITORIOS);
    if (mapa == NULL) {
        printf("Erro: nao foi possivel alocar memoria para o mapa.\n");
        return 1;
    }

    /* Preparo os territórios iniciais e sorteio a missão secreta do jogador. */
    inicializarTerritorios(mapa, TOTAL_TERRITORIOS);
    missao = sortearMissao();

    printf("========================================\n");
    printf("        WAR ESTRUTURADO - MISSOES       \n");
    printf("========================================\n");
    printf("Seu exercito: %s\n", COR_JOGADOR);
    exibirMissao(missao);

    /* O jogo continua até o jogador sair ou cumprir a missão. */
    do {
        exibirMapa(mapa, TOTAL_TERRITORIOS);
        exibirMenuPrincipal();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, TOTAL_TERRITORIOS);
                if (verificarVitoria(mapa, TOTAL_TERRITORIOS, missao, COR_JOGADOR)) {
                    printf("\nParabens! Voce cumpriu sua missao e venceu o jogo.\n");
                    venceu = 1;
                }
                break;
            case 2:
                if (verificarVitoria(mapa, TOTAL_TERRITORIOS, missao, COR_JOGADOR)) {
                    printf("\nMissao cumprida! Voce venceu o jogo.\n");
                    venceu = 1;
                } else {
                    printf("\nMissao ainda nao cumprida. Continue conquistando territorios.\n");
                }
                break;
            case 0:
                printf("\nEncerrando o jogo...\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0 && !venceu);

    /* No final eu libero a memória que foi alocada com calloc. */
    liberarMemoria(mapa);
    return 0;
}

Territorio *alocarMapa(int quantidade) {
    /* calloc já inicia os campos zerados e retorna o ponteiro para o vetor. */
    return (Territorio *) calloc((size_t) quantidade, sizeof(Territorio));
}

void inicializarTerritorios(Territorio *mapa, int quantidade) {
    /* Deixei o mapa inicial pronto para o jogador já começar pelo menu. */
    const Territorio iniciais[TOTAL_TERRITORIOS] = {
        {"Brasil", "Azul", 5},
        {"Argentina", "Verde", 4},
        {"Chile", "Verde", 3},
        {"Peru", "Vermelho", 4},
        {"Uruguai", "Amarelo", 2}
    };
    int i;

    /* Copio os territórios padrão para o vetor que foi alocado dinamicamente. */
    for (i = 0; i < quantidade && i < TOTAL_TERRITORIOS; i++) {
        mapa[i] = iniciais[i];
    }
}

void liberarMemoria(Territorio *mapa) {
    /* free evita vazamento de memória depois que o jogo termina. */
    free(mapa);
}

void exibirMenuPrincipal(void) {
    /* Menu principal com as ações que o jogador pode escolher. */
    printf("\n----------- MENU -----------\n");
    printf("1 - Atacar\n");
    printf("2 - Verificar missao\n");
    printf("0 - Sair\n");
}

void exibirMapa(const Territorio *mapa, int quantidade) {
    int i;

    /* Uso const porque essa função só mostra o mapa, sem alterar nada. */
    printf("\n================ MAPA ATUAL ================\n");
    printf("%-4s %-15s %-12s %s\n", "ID", "Territorio", "Exercito", "Tropas");
    printf("--------------------------------------------\n");

    for (i = 0; i < quantidade; i++) {
        printf("%-4d %-15s %-12s %d\n", i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

void exibirMissao(int missao) {
    /* A missão é identificada por um número sorteado no início do jogo. */
    printf("\nSua missao secreta:\n");

    switch (missao) {
        case 1:
            printf("- Destruir o exercito %s.\n", COR_ALVO);
            break;
        case 2:
            printf("- Conquistar pelo menos 3 territorios.\n");
            break;
        default:
            printf("- Missao desconhecida.\n");
            break;
    }
}

void faseDeAtaque(Territorio *mapa, int quantidade) {
    /* Aqui eu leio os IDs escolhidos pelo jogador e valido antes da batalha. */
    int origem;
    int destino;

    origem = lerInteiro("\nInforme o territorio atacante (1 a 5): ");
    destino = lerInteiro("Informe o territorio defensor (1 a 5): ");

    if (origem < 1 || origem > quantidade || destino < 1 || destino > quantidade) {
        printf("\nAtaque invalido: escolha IDs entre 1 e %d.\n", quantidade);
        return;
    }

    if (origem == destino) {
        printf("\nAtaque invalido: atacante e defensor devem ser diferentes.\n");
        return;
    }

    /* Passo os territórios por ponteiro para a batalha conseguir alterar tropas e dono. */
    simularAtaque(&mapa[origem - 1], &mapa[destino - 1]);
}

void simularAtaque(Territorio *atacante, Territorio *defensor) {
    /* Os dados decidem a batalha. Empate favorece o atacante, como pede o README. */
    int dadoAtaque;
    int dadoDefesa;

    /* O jogador só pode atacar usando territórios do próprio exército. */
    if (strcmp(atacante->cor, COR_JOGADOR) != 0) {
        printf("\nAtaque invalido: voce so pode atacar usando territorios do exercito %s.\n", COR_JOGADOR);
        return;
    }

    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("\nAtaque invalido: nao e possivel atacar um territorio aliado.\n");
        return;
    }

    if (atacante->tropas <= 1) {
        printf("\nAtaque invalido: o atacante precisa ter mais de 1 tropa.\n");
        return;
    }

    if (defensor->tropas <= 0) {
        printf("\nAtaque invalido: o territorio defensor ja esta sem tropas.\n");
        return;
    }

    /* Sorteio um valor de 1 a 6 para cada lado, simulando dados comuns. */
    dadoAtaque = (rand() % 6) + 1;
    dadoDefesa = (rand() % 6) + 1;

    printf("\nBatalha: %s (%s) ataca %s (%s)\n",
           atacante->nome, atacante->cor, defensor->nome, defensor->cor);
    printf("Dado do atacante: %d\n", dadoAtaque);
    printf("Dado do defensor: %d\n", dadoDefesa);

    if (dadoAtaque >= dadoDefesa) {
        /* Se o ataque vence, o defensor perde uma tropa. */
        defensor->tropas--;
        printf("Resultado: atacante venceu. %s perdeu 1 tropa.\n", defensor->nome);

        if (defensor->tropas == 0) {
            /* Quando as tropas acabam, o território muda para a cor do atacante. */
            strcpy(defensor->cor, atacante->cor);
            defensor->tropas = 1;
            atacante->tropas--;
            printf("Conquista! %s agora pertence ao exercito %s.\n", defensor->nome, defensor->cor);
        }
    } else {
        /* Se a defesa vence, o atacante perde uma tropa. */
        atacante->tropas--;
        printf("Resultado: defensor venceu. %s perdeu 1 tropa.\n", atacante->nome);
    }
}

int sortearMissao(void) {
    /* Sorteia uma das missões disponíveis, usando IDs de 1 até TOTAL_MISSOES. */
    return (rand() % TOTAL_MISSOES) + 1;
}

int verificarVitoria(const Territorio *mapa, int quantidade, int missao, const char *corJogador) {
    /* Cada missão tem uma regra própria para saber se o jogador venceu. */
    switch (missao) {
        case 1:
            return contarTerritoriosPorCor(mapa, quantidade, COR_ALVO) == 0;
        case 2:
            return contarTerritoriosPorCor(mapa, quantidade, corJogador) >= 3;
        default:
            return 0;
    }
}

int contarTerritoriosPorCor(const Territorio *mapa, int quantidade, const char *cor) {
    /* Essa função ajuda tanto na missão de destruir quanto na de conquistar. */
    int i;
    int total = 0;

    for (i = 0; i < quantidade; i++) {
        if (strcmp(mapa[i].cor, cor) == 0) {
            total++;
        }
    }

    return total;
}

int lerInteiro(const char *mensagem) {
    /* Centralizei a leitura para tratar entradas inválidas em um lugar só. */
    int valor;
    int lidos;

    printf("%s", mensagem);
    lidos = scanf("%d", &valor);
    limparBufferEntrada();

    while (lidos != 1) {
        printf("Entrada invalida. Digite um numero: ");
        lidos = scanf("%d", &valor);
        limparBufferEntrada();
    }

    return valor;
}

void limparBufferEntrada(void) {
    /* Limpo o restante da linha para a próxima leitura não pegar lixo do teclado. */
    int caractere;

    do {
        caractere = getchar();
    } while (caractere != '\n' && caractere != EOF);
}
