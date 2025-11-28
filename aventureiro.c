#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Estruturas ---
typedef struct Comodo
{
    char nome[50];
    char pista[100];
    struct Comodo *esquerda;
    struct Comodo *direita;
} Comodo;

// --- Árvore Binária de Pistas ---
typedef struct NoBST
{
    char pista[100];
    struct NoBST *esquerda;
    struct NoBST *direita;
} NoBST;

// --- Criação e gerenciamento ---
Comodo *criarComodo(char *nome, char *pista);
void ligar(Comodo *origem, Comodo *esq, Comodo *dir);
void distribuirPistas(Comodo *comodos[], int qtdComodos, char *pistas[], int totalPistas);
Comodo *montarMansao();
void liberarArvore(Comodo *no);
// BST
NoBST *criarNoBST(char *pista);
NoBST *inserirBST(NoBST *raiz, char *pista);
void mostrarPistasBST(NoBST *raiz);
void liberarBST(NoBST *raiz);

// --- Interface ---
void menu(Comodo *atual, NoBST **pistaBST);

// --- Função principal ---
int main()
{
    srand(time(NULL));

    NoBST *pistasEncontradas = NULL;

    Comodo *raizMansao = montarMansao();
    Comodo *atual = raizMansao;

    menu(raizMansao, &pistasEncontradas);

    if (pistasEncontradas == NULL) 
    {
        printf("\nNenhuma pista foi encontrada.");
        liberarArvore(raizMansao);
        return 0;
    }

    printf("\n===== Pistas Encontradas =====\n");
    mostrarPistasBST(pistasEncontradas);

    liberarBST(pistasEncontradas);
    liberarArvore(raizMansao);

    return 0;
}

// Arvore da Mansão

// ---------------------------------
// Criação de nós da árvore
// ---------------------------------
Comodo *criarComodo(char *nome, char *pista)
{
    Comodo *novo = (Comodo *)malloc(sizeof(Comodo));
    if (novo == NULL)
    {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }

    strcpy(novo->nome, nome); // Atribui o nome ao cômodo

    if (pista) // Se uma pista for fornecida
        strcpy(novo->pista, pista);
    else
        strcpy(novo->pista, ""); // Inicializa pista vazia

    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}
// ---------------------------------
// Conexões entre cômodos
// ---------------------------------
void ligar(Comodo *origem, Comodo *esq, Comodo *dir)
{
    // Liga os cômodos na árvore
    if (origem)
    {
        origem->esquerda = esq;
        origem->direita = dir;
    }
}
// ---------------------------------
// Distribuição aleatória de pistas
// ---------------------------------
void distribuirPistas(Comodo *comodos[], int qtdComodos, char *pistas[], int totalPistas)
{
    // funcao para distribuir pistas aleatoriamente nos comodos
    for (int i = 0; i < qtdComodos; i++)
    {
        // 50% de chance de ter pista
        if (rand() % 2 == 0)
        {
            strcpy(comodos[i]->pista,
                   pistas[rand() % totalPistas]); // Atribui uma pista aleatória
        }
    }
}
// ---------------------------------
// Montagem da mansão (árvore pronta)
// ---------------------------------
Comodo *montarMansao()
{
    /* Estrutura da Mansão (Árvore Binária):
                    Hall
                  /     \
             Cozinha   Biblioteca
             /   \        /     \
       Quarto  Escr.  SalaJ    SalaE
                         |        |
                      Jardim   Banheiro
    */

    // Criação dos cômodos sem pistas inicialmente
    Comodo *hall = criarComodo("Hall de Entrada", NULL);
    Comodo *cozinha = criarComodo("Cozinha", NULL);
    Comodo *biblioteca = criarComodo("Biblioteca", NULL);
    Comodo *quarto = criarComodo("Quarto Master", NULL);
    Comodo *escritorio = criarComodo("Escritorio", NULL);
    Comodo *salaJantar = criarComodo("Sala de Jantar", NULL);
    Comodo *salaEstar = criarComodo("Sala de Estar", NULL);
    Comodo *banheiro = criarComodo("Banheiro", NULL);
    Comodo *jardim = criarComodo("Jardim", NULL);

    // Ligações
    ligar(hall, cozinha, biblioteca);
    ligar(cozinha, quarto, escritorio);
    ligar(biblioteca, salaJantar, salaEstar);
    ligar(salaEstar, NULL, banheiro);
    ligar(salaJantar, jardim, NULL);

    // Vetor contendo todos os cômodos para sorteio de pistas
    Comodo *todos[] = {
        hall, cozinha, biblioteca, quarto, escritorio,
        salaJantar, salaEstar, banheiro, jardim};

    // Lista de pistas disponíveis
    char *pistas[] = {
        "A luz está apagada.",
        "Há pegadas de lama.",
        "Um objeto foi derrubado.",
        "Uma janela está aberta.",
        "Um cheiro estranho vem daqui."};

    int totalPistas = sizeof(pistas) / sizeof(pistas[0]);

    // Distribuir pistas aleatoriamente
    distribuirPistas(todos, 9, pistas, totalPistas);

    return hall;
}
// ---------------------------------
// Liberação da árvore
// ---------------------------------
void liberarArvore(Comodo *no)

{
    if (!no)
        return;

    liberarArvore(no->esquerda);
    liberarArvore(no->direita);

    free(no);
}

// BST das Pistas

//---------------------------------
// Criação de nós da BST
// ---------------------------------
NoBST *criarNoBST(char *pista)
{
    NoBST *novo = (NoBST *)malloc(sizeof(NoBST));
    if (novo == NULL)
    {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }

    strcpy(novo->pista, pista);
    novo->esquerda = novo->direita = NULL;
    return novo;
}
// ---------------------------------
// Inserção na BST
// ---------------------------------
NoBST *inserirBST(NoBST *raiz, char *pista)
{
    if (raiz == NULL)
        return criarNoBST(pista);

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirBST(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirBST(raiz->direita, pista);

    // Se for igual, não insere duplicado
    return raiz;
}
// ---------------------------------
// Mostrar pistas na ordem alfabetica
// ---------------------------------
void mostrarPistasBST(NoBST *raiz)
{
    if (!raiz)
        return;

    mostrarPistasBST(raiz->esquerda);
    printf(" - %s\n", raiz->pista);
    mostrarPistasBST(raiz->direita);
}
// ---------------------------------
// Liberação da BST
// ---------------------------------
void liberarBST(NoBST *raiz)
{
    if (!raiz)
        return;
    liberarBST(raiz->esquerda);
    liberarBST(raiz->direita);
    free(raiz);
}
// ---------------------------------
// Menu de navegação da mansão
// ---------------------------------
void menu(Comodo *atual, NoBST **pistaBST)
{
    char opcao[5];

    while (1)
    {
        printf("\n===== Mapa da Mansão =====\n");
        printf("Você está em: %s\n\n", atual->nome);

        printf("e. Ir para esquerda  [%s]\n",
               atual->esquerda ? atual->esquerda->nome : "Nenhum");

        printf("d. Ir para direita   [%s]\n",
               atual->direita ? atual->direita->nome : "Nenhum");

        printf("s. Sair\n");
        printf("===========================\n");

        printf("Escolha uma opção: ");
        fgets(opcao, sizeof(opcao), stdin);
        printf("\n");

        char escolha = opcao[0];

        switch (escolha)
        {
        case 'e':
            if (atual->esquerda)
            {
                atual = atual->esquerda;
                printf("Você foi para: %s.\n", atual->nome);

                if (strlen(atual->pista) > 0)
                {
                    printf("Pista encontrada: %s\n", atual->pista);
                    *pistaBST = inserirBST(*pistaBST, atual->pista); // Insere na BST
                }
                else
                    printf("Nenhuma pista aqui.\n");
            }
            else
                printf("Não há cômodo à esquerda!\n");
            break;

        case 'd':
            if (atual->direita)
            {
                atual = atual->direita;
                printf("Você foi para: %s.\n", atual->nome);

                if (strlen(atual->pista) > 0)
                {
                    printf("Pista encontrada: %s\n", atual->pista);
                    *pistaBST = inserirBST(*pistaBST, atual->pista); // Insere na BST
                }
                else
                    printf("Nenhuma pista aqui.\n");
            }
            else
                printf("Não há cômodo à direita!\n");
            break;

        case 's':
            printf("Saindo do mapa da mansão...\n");
            return;

        default:
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}
