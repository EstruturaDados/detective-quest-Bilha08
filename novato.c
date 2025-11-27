#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Estruturas ---
typedef struct Comodo
{
    char nome[50];
    struct Comodo *esquerda;
    struct Comodo *direita;
} Comodo;

// --- Criação e gerenciamento ---
Comodo *criarComodo(char *nome);
void ligar(Comodo *origem, Comodo *esq, Comodo *dir);
Comodo *montarMansao();
void liberarArvore(Comodo *no);

// --- Interface ---
void menu(Comodo *atual);
// --- Função principal ---
int main()
{
    Comodo *raizMansao = montarMansao();
    Comodo *atual = raizMansao;

    menu(atual);

    liberarArvore(raizMansao);

    return 0;
}
// ---------------------------------
// Criação de nós da árvore
// ---------------------------------
Comodo *criarComodo(char *nome)
{
    Comodo *novo = (Comodo *)malloc(sizeof(Comodo));
    if (novo == NULL)
    {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(novo->nome, nome);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}
// ---------------------------------
// Conexões entre cômodos
// ---------------------------------
void ligar(Comodo *origem, Comodo *esq, Comodo *dir)
{
    if (origem)
    {
        origem->esquerda = esq;
        origem->direita = dir;
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
         |                 |        |
       NULL              Jardim   Banheiro
    */

    Comodo *hall = criarComodo("Hall de Entrada");
    Comodo *cozinha = criarComodo("Cozinha");
    Comodo *biblioteca = criarComodo("Biblioteca");
    Comodo *quarto = criarComodo("Quarto Master");
    Comodo *escritorio = criarComodo("Escritorio");
    Comodo *salaJantar = criarComodo("Sala de Jantar");
    Comodo *salaEstar = criarComodo("Sala de Estar");
    Comodo *banheiro = criarComodo("Banheiro");
    Comodo *jardim = criarComodo("Jardim");

    ligar(hall, cozinha, biblioteca);
    ligar(cozinha, quarto, escritorio);
    ligar(biblioteca, salaJantar, salaEstar);
    ligar(salaEstar, NULL, banheiro);
    ligar(salaJantar, jardim, NULL);

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
// ---------------------------------
// Menu de navegação da mansão
// ---------------------------------
void menu(Comodo *atual)
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
            if (atual->esquerda != NULL)
            {
                atual = atual->esquerda;
                printf("Você foi para: %s.\n", atual->nome);
            }
            else
            {
                printf("Não há cômodo à esquerda!\n");
            }
            break;

        case 'd':
            if (atual->direita != NULL)
            {
                atual = atual->direita;
                printf("Você foi para: %s.\n", atual->nome);
            }
            else
            {
                printf("Não há cômodo à direita!\n");
            }
            break;

        case 's':
            printf("Saindo do mapa da mansão...\n");
            return;

        default:
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}
