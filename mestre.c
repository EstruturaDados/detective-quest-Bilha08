#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constantes
#define TAM_HASH 23
#define MAX_COMODOS 30
#define MAX_SUSPEITOS 20

// ---------------------------------
// Estruturas
// ---------------------------------

// --- Estrutura de um cômodo ---
typedef struct Comodo
{
    char nome[50];
    char pista[100];
    struct Comodo *esquerda;
    struct Comodo *direita;
} Comodo;

// --- Árvore Binária de Pistas (BST) ---
typedef struct NoBST
{
    char pista[100];
    struct NoBST *esquerda;
    struct NoBST *direita;
} NoBST;

// --- Ligação pista -> suspeito ---
typedef struct
{
    char pista[100];
    char suspeito[50];
} LigacaoPistaSuspeito;

// --- Nó para tabela hash (chaining) ---
typedef struct HashNo
{
    char pista[100];
    char suspeito[50];
    struct HashNo *proximo;
} HashNo;

// --- Tabela hash (array de listas encadeadas) ---
typedef struct
{
    HashNo *tabela[TAM_HASH];
} HashPistas;

// ---------------------------------
// Protótipos
// ---------------------------------

// Mansão / construção
Comodo *criarComodo(const char *nome, const char *pista);
void ligar(Comodo *origem, Comodo *esq, Comodo *dir);
Comodo *montarMansao(Comodo *todos[], int *qtdComodos);
void liberarArvore(Comodo *no);

// distribuição de pistas
void distribuirPistas(Comodo *comodos[], int qtdComodos, LigacaoPistaSuspeito base[], int totalBase);

// BST
NoBST *criarNoBST(const char *pista);
int buscarBST(NoBST *raiz, const char *pista);
NoBST *inserirBST(NoBST *raiz, const char *pista);
void mostrarPistasBST(NoBST *raiz);
void liberarBST(NoBST *raiz);

// Hash
int funcao_hash(const char *chave);
void inicializarHashPistas(HashPistas *hash);
void inserirHashPista(HashPistas *hash, const char *pista, const char *suspeito);
int contarPistasPorSuspeito(HashPistas *hash, const char *suspeito);
void listarPistasPorSuspeito(HashPistas *hash, const char *suspeito);
void mostrarHashPistas(HashPistas *hash);
void liberarHashPistas(HashPistas *hash);

// Interface / menus
void menu(Comodo *raiz, NoBST **pistasBST, HashPistas *hash, LigacaoPistaSuspeito base[], int totalBase, char *suspeitos[], int totalSuspeitos);
void menuFinal(HashPistas *hash, char *suspeitos[], int totalSuspeitos);

// Utilitários
void trim_newline(char *s); // remove \n e \r

// ---------------------------------
// Implementação
// ---------------------------------

// ---------------------------------
// Função principal
// ---------------------------------
int main(void)
{
    srand((unsigned)time(NULL));

    // Lista fixa de suspeitos solicitada
    char *suspeitos[] = {
        "Mordomo",
        "Jardineiro",
        "Cozinheira",
        "Bibliotecario",
        "Visitante Misterioso"};
    int totalSuspeitos = 5;

    // pista -> suspeito
    LigacaoPistaSuspeito base[] = {
        {"A luz está apagada.", "Mordomo"},
        {"Há pegadas de lama.", "Jardineiro"},
        {"Um objeto foi derrubado.", "Cozinheira"},
        {"Uma janela está entreaberta.", "Visitante Misterioso"},
        {"Um cheiro estranho vem daqui.", "Jardineiro"},
        {"Marcas de faca na mesa.", "Cozinheira"},
        {"Perfume caro no ar.", "Visitante Misterioso"},
        {"Livro antigo fora do lugar.", "Mordomo"},
        {"Ferramentas sujas largadas.", "Jardineiro"},
        {"Copo quebrado na cozinha.", "Cozinheira"},
        {"Bilhete com letras cortadas.", "Visitante Misterioso"},
        {"Papel rasgado no chão.", "Mordomo"},
        {"Sinais de lama na estufa.", "Jardineiro"},
        {"Sujeira próxima aos arquivos.", "Mordomo"},
        {"Garrafa vazia na adega.", "Visitante Misterioso"}};
    int totalBase = sizeof(base) / sizeof(base[0]);

    // Estruturas de armazenamento
    HashPistas tabela;
    inicializarHashPistas(&tabela);

    NoBST *pistasEncontradas = NULL;

    // Montar mansão e obter todos os cômodos
    Comodo *todos[MAX_COMODOS];
    int qtdComodos = 0;
    Comodo *raiz = montarMansao(todos, &qtdComodos);

    // Distribuir pistas automaticamente (aleatoriamente escolhendo da base)
    distribuirPistas(todos, qtdComodos, base, totalBase);

    // Menu principal (navegação)
    menu(raiz, &pistasEncontradas, &tabela, base, totalBase, suspeitos, totalSuspeitos);

    // Menu final de investigação
    menuFinal(&tabela, suspeitos, totalSuspeitos);

    // Exibir BST final (opcional)
    printf("\n===== Pistas Encontradas (ordenadas) =====\n");
    if (pistasEncontradas == NULL)
        printf("Nenhuma pista coletada.\n");
    else
        mostrarPistasBST(pistasEncontradas);

    // Mostrar tabela hash completa
    printf("\n===== Tabela Hash (pista -> suspeito) =====\n");
    mostrarHashPistas(&tabela);

    // Liberar memória
    liberarBST(pistasEncontradas);
    liberarHashPistas(&tabela);
    liberarArvore(raiz);

    return 0;
}

// ---------------------------------
// Funções da Mansão e criação de cômodos
// ---------------------------------

// Cria um novo cômodo
Comodo *criarComodo(const char *nome, const char *pista)
{
    Comodo *c = malloc(sizeof(Comodo));
    if (c == NULL)
    {
        printf("Erro ao alocar memória para cômodo.\n");
        exit(1);
    }
    strncpy(c->nome, nome, sizeof(c->nome) - 1);
    c->nome[sizeof(c->nome) - 1] = '\0';
    if (pista)
        strncpy(c->pista, pista, sizeof(c->pista) - 1);
    else
        c->pista[0] = '\0';
    c->pista[sizeof(c->pista) - 1] = '\0';
    c->esquerda = c->direita = NULL;
    return c;
}
// Liga dois cômodos à esquerda e direita de um cômodo origem
void ligar(Comodo *origem, Comodo *esq, Comodo *dir)
{
    if (origem == NULL)
        return;
    origem->esquerda = esq;
    origem->direita = dir;
}

// Monta a mansão e preenche o vetor todos[] com ponteiros para cada cômodo.
// Retorna a raiz (hall).
// qtdComodos incrementado com o número de cômodos.
Comodo *montarMansao(Comodo *todos[], int *qtdComodos)
{
    /* Estrutura da Mansão (exemplo):
                   Hall
                 /     \
            Cozinha   Biblioteca
            /   \       /    \
         Quarto Escritorio SalaJ SalaE
          |       |         |     \
        Closet Arquivos   Jardim Banheiro
                          |
                         Estufa
    */

    Comodo *hall = criarComodo("Hall de Entrada", NULL);
    Comodo *cozinha = criarComodo("Cozinha", NULL);
    Comodo *biblioteca = criarComodo("Biblioteca", NULL);
    Comodo *quarto = criarComodo("Quarto Master", NULL);
    Comodo *escritorio = criarComodo("Escritorio", NULL);
    Comodo *salaJ = criarComodo("Sala de Jantar", NULL);
    Comodo *salaE = criarComodo("Sala de Estar", NULL);
    Comodo *banheiro = criarComodo("Banheiro", NULL);
    Comodo *closet = criarComodo("Closet", NULL);
    Comodo *arquivos = criarComodo("Sala de Arquivos", NULL);
    Comodo *jardim = criarComodo("Jardim", NULL);
    Comodo *estufa = criarComodo("Estufa", NULL);

    ligar(hall, cozinha, biblioteca);
    ligar(cozinha, quarto, escritorio);
    ligar(quarto, closet, NULL);
    ligar(escritorio, arquivos, NULL);
    ligar(biblioteca, salaJ, salaE);
    ligar(salaJ, jardim, NULL);
    ligar(jardim, estufa, NULL);
    ligar(salaE, NULL, banheiro);

    // preencher vetor de todos os cômodos (ordem arbitrária)
    Comodo *lista[] = {
        hall, cozinha, biblioteca, quarto, escritorio,
        salaJ, salaE, banheiro, closet, arquivos, jardim, estufa};
    int total = sizeof(lista) / sizeof(lista[0]);
    for (int i = 0; i < total; ++i)
        todos[(*qtdComodos)++] = lista[i];

    return hall;
}
// Libera memória da árvore de cômodos
void liberarArvore(Comodo *no)
{
    if (no == NULL)
        return;
    liberarArvore(no->esquerda);
    liberarArvore(no->direita);
    free(no);
}

// ---------------------------------
// Distribuição de pistas
// ---------------------------------
void distribuirPistas(Comodo *comodos[], int qtdComodos, LigacaoPistaSuspeito base[], int totalBase)
{
    // Distribui pistas aleatoriamente entre os cômodos
    // (nem todos recebem pista)
    for (int i = 0; i < qtdComodos; i++)
    {
        // 90% de chance de ter pista
        if ((rand() % 100) < 90)
        {
            int id = rand() % totalBase;
            strncpy(comodos[i]->pista, base[id].pista, sizeof(comodos[i]->pista) - 1);
            comodos[i]->pista[sizeof(comodos[i]->pista) - 1] = '\0';
        }
        else
        {
            comodos[i]->pista[0] = '\0';
        }
    }
}

// ---------------------------------
// BST (pistas encontradas)
// ---------------------------------
// Cria um novo nó da BST
NoBST *criarNoBST(const char *pista)
{
    NoBST *n = malloc(sizeof(NoBST));
    if (n == NULL)
    {
        printf("Erro ao alocar memória para nó BST.\n");
        exit(1);
    }
    strncpy(n->pista, pista, sizeof(n->pista) - 1);
    n->pista[sizeof(n->pista) - 1] = '\0';
    n->esquerda = n->direita = NULL;
    return n;
}
// Busca uma pista na BST; retorna 1 se encontrada, 0 caso contrário
int buscarBST(NoBST *raiz, const char *pista)
{
    if (raiz == NULL)
        return 0;
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0)
        return 1;
    if (cmp < 0)
        return buscarBST(raiz->esquerda, pista);
    return buscarBST(raiz->direita, pista);
}
// Insere uma pista na BST
NoBST *inserirBST(NoBST *raiz, const char *pista)
{
    if (raiz == NULL)
        return criarNoBST(pista);
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0)
        raiz->esquerda = inserirBST(raiz->esquerda, pista);
    else if (cmp > 0)
        raiz->direita = inserirBST(raiz->direita, pista);
    return raiz;
}
// Mostra todas as pistas na BST (ordem)
void mostrarPistasBST(NoBST *raiz)
{
    if (raiz == NULL)
        return;
    mostrarPistasBST(raiz->esquerda);
    printf(" - %s\n", raiz->pista);
    mostrarPistasBST(raiz->direita);
}
// Libera memória da BST
void liberarBST(NoBST *raiz)
{
    if (raiz == NULL)
        return;
    liberarBST(raiz->esquerda);
    liberarBST(raiz->direita);
    free(raiz);
}

// ---------------------------------
// Hash (pista -> suspeito)
// ---------------------------------
// Função de hash
int funcao_hash(const char *chave)
{
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++)
    {
        soma += (unsigned char)chave[i];
    }
    return soma % TAM_HASH;
}
// Inicializa a tabela hash
void inicializarHashPistas(HashPistas *hash)
{
    for (int i = 0; i < TAM_HASH; ++i)
        hash->tabela[i] = NULL;
}
// Insere uma pista e seu suspeito associado na tabela hash
void inserirHashPista(HashPistas *hash, const char *pista, const char *suspeito)
{
    int idx = funcao_hash(pista);
    HashNo *novo = malloc(sizeof(HashNo));
    if (novo == NULL)
    {
        printf("Erro ao alocar memória para nó da tabela hash.\n");
        exit(1);
    }
    strncpy(novo->pista, pista, sizeof(novo->pista) - 1);
    novo->pista[sizeof(novo->pista) - 1] = '\0';
    strncpy(novo->suspeito, suspeito, sizeof(novo->suspeito) - 1);
    novo->suspeito[sizeof(novo->suspeito) - 1] = '\0';
    novo->proximo = hash->tabela[idx];
    hash->tabela[idx] = novo;
}

// Conta quantas pistas estão associadas a um suspeito (varre a hash)
int contarPistasPorSuspeito(HashPistas *hash, const char *suspeito)
{
    int cont = 0;
    for (int i = 0; i < TAM_HASH; ++i)
    {
        for (HashNo *h = hash->tabela[i]; h; h = h->proximo)
            if (strcmp(h->suspeito, suspeito) == 0)
                ++cont;
    }
    return cont;
}
// Lista todas as pistas associadas a um suspeito
void listarPistasPorSuspeito(HashPistas *hash, const char *suspeito)
{
    int encontrou = 0;
    for (int i = 0; i < TAM_HASH; ++i)
    {
        for (HashNo *h = hash->tabela[i]; h; h = h->proximo)
        {
            if (strcmp(h->suspeito, suspeito) == 0)
            {
                if (encontrou == 0)
                {
                    printf("\nPistas associadas a %s:\n", suspeito);
                    encontrou = 1;
                }
                printf(" - %s\n", h->pista);
            }
        }
    }
    if (encontrou == 0)
        printf("Nenhuma pista associada a %s.\n", suspeito);
}
// Mostra toda a tabela hash
void mostrarHashPistas(HashPistas *hash)
{
    int any = 0; // flag para saber se há algo para mostrar
    for (int i = 0; i < TAM_HASH; ++i)
    {
        for (HashNo *h = hash->tabela[i]; h; h = h->proximo)
        {
            if (any == 0)
            {
                printf("\n");
                any = 1;
            }
            printf("Pista: %-40s -> Suspeito: %s\n", h->pista, h->suspeito);
        }
    }
    if (any == 0)
        printf("Tabela hash vazia.\n");
}
// Libera memória da tabela hash
void liberarHashPistas(HashPistas *hash)
{
    for (int i = 0; i < TAM_HASH; ++i)
    {
        HashNo *h = hash->tabela[i];
        while (h)
        {
            HashNo *tmp = h->proximo;
            free(h);
            h = tmp;
        }
        hash->tabela[i] = NULL;
    }
}

// ---------------------------------
// Trim utility
// ---------------------------------
void trim_newline(char *s) // remove \n e \r
{
    if (!s)
        return;
    s[strcspn(s, "\r\n")] = '\0';
}

// ---------------------------------
// Menu de navegação (coleção de pistas)
// ---------------------------------
void menu(Comodo *raiz, NoBST **pistasBST, HashPistas *hash, LigacaoPistaSuspeito base[], int totalBase, char *suspeitos[], int totalSuspeitos)
{
    Comodo *atual = raiz;
    char entrada[64];

    while (1)
    {
        printf("\n===== Mapa da Mansão =====\n");
        printf("Você está em: %s\n\n", atual->nome);
        printf("e - Ir para esquerda  [%s]\n", atual->esquerda ? atual->esquerda->nome : "Nenhum");
        printf("d - Ir para direita   [%s]\n", atual->direita ? atual->direita->nome : "Nenhum");
        printf("s - Sair do mapa (Ir para o menu de suspeitos)\n");
        printf("===========================\n");
        printf("Escolha: ");
        if (fgets(entrada, sizeof(entrada), stdin) == NULL)
            break;
        trim_newline(entrada);
        if (strlen(entrada) == 0)
            continue;
        char c = entrada[0];

        if (c == 's')
        {
            printf("Saindo do mapa.\n");
            break;
        }

        Comodo *dest = NULL; // ir para o cômodo escolhido
        if (c == 'e')
            dest = atual->esquerda;
        else if (c == 'd')
            dest = atual->direita;
        else
        {
            printf("Opção inválida.\n");
            continue;
        }

        if (dest == NULL)
        {
            printf("Não existe cômodo nessa direção.\n");
            continue;
        }

        atual = dest;
        printf("Você entrou em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0)
        {
            printf("Pista visível: %s\n", atual->pista);

            // verificar se já coletamos esta pista (BST)
            if (!buscarBST(*pistasBST, atual->pista))
            {
                *pistasBST = inserirBST(*pistasBST, atual->pista);

                // Determinar suspeito de forma determinística, usando a base
                const char *suspeito = "Desconhecido";
                for (int i = 0; i < totalBase; ++i)
                    if (strcmp(base[i].pista, atual->pista) == 0)
                    {
                        suspeito = base[i].suspeito;
                        break;
                    }

                // Inserir na hash a associação pista -> suspeito
                inserirHashPista(hash, atual->pista, suspeito);

                printf("Pista registrada. Suspeito associado: %s\n", suspeito);
            }
            else
            {
                printf("Você já registrou essa pista antes.\n");
            }
        }
        else
        {
            printf("Sem pista visível aqui.\n");
        }
    }
}

// ---------------------------------
// Menu final: análises e acusação
// ---------------------------------
void menuFinal(HashPistas *hash, char *suspeitos[], int totalSuspeitos)
{
    char entrada[80];

    while (1)
    {
        printf("\n===== MENU FINAL =====\n");
        printf("1 - Ver suspeito(s) mais associado(s)\n");
        printf("2 - Mostrar pistas por suspeito\n");
        printf("3 - Acusar um suspeito\n");
        printf("4 - Sair\n");
        printf("======================\n");
        printf("Escolha: ");
        if (fgets(entrada, sizeof(entrada), stdin) == NULL)
            break;
        trim_newline(entrada);
        if (strlen(entrada) == 0)
            continue;

        if (entrada[0] == '1')
        {
            // calcular contagens para cada suspeito da lista fornecida
            int contagens[MAX_SUSPEITOS] = {0};
            int totalRelevantes = 0;
            for (int i = 0; i < totalSuspeitos; ++i)
            {
                contagens[i] = contarPistasPorSuspeito(hash, suspeitos[i]);
                if (contagens[i] > 0)
                    totalRelevantes++;
            }

            if (totalRelevantes == 0)
            {
                printf("Nenhuma pista coletada. Ninguém associado ainda.\n");
                continue;
            }

            // encontra maior contagem
            int maior = 0;
            for (int i = 1; i < totalSuspeitos; ++i)
                if (contagens[i] > contagens[maior])
                    maior = i;

            // se há empate, mostramos todos com o mesmo valor
            int maxVal = contagens[maior];
            printf("\nSuspeito(s) mais associado(s) com %d pista(s):\n", maxVal);
            for (int i = 0; i < totalSuspeitos; ++i)
            {
                if (contagens[i] == maxVal && maxVal > 0)
                    printf(" - %s\n", suspeitos[i]);
            }
        }
        else if (entrada[0] == '2')
        {
            // listar nomes para escolha
            printf("\nEscolha um suspeito para ver suas pistas:\n");
            for (int i = 0; i < totalSuspeitos; ++i)
                printf(" %d) %s\n", i + 1, suspeitos[i]);
            printf("Escolha (numero): ");
            if (fgets(entrada, sizeof(entrada), stdin) == NULL)
                break;
            int escolha = atoi(entrada);
            if (escolha >= 1 && escolha <= totalSuspeitos)
            {
                listarPistasPorSuspeito(hash, suspeitos[escolha - 1]);
            }
            else
            {
                printf("Escolha inválida.\n");
            }
        }
        else if (entrada[0] == '3')
        {
            // mostrar lista de suspeitos
            printf("\nLista de suspeitos:\n");
            for (int i = 0; i < totalSuspeitos; ++i)
                printf(" %d) %s\n", i + 1, suspeitos[i]);

            printf("Escolha um suspeito para acusar (numero): ");
            if (fgets(entrada, sizeof(entrada), stdin) == NULL)
                break;
            int escolha = atoi(entrada);
            if (escolha >= 1 && escolha <= totalSuspeitos)
            {
                const char *selecionado = suspeitos[escolha - 1];
                int cont = contarPistasPorSuspeito(hash, selecionado);

                printf("\nVocê acusou: %s\n", selecionado);
                printf("Pistas associadas a esse suspeito: %d\n", cont);

                if (cont >= 2)
                {
                    printf("Resultado: ACERTOU! Esse suspeito tem %d pistas associadas.\n", cont);
                }
                else
                {
                    printf("Resultado: ERROU. Esse suspeito não possui 2+ pistas associadas.\n");
                }
            }
            else
            {
                printf("Escolha inválida.\n");
            }
        }
        else if (entrada[0] == '4')
        {
            printf("Saindo do menu final.\n");
            return;
        }
        else
        {
            printf("Opção inválida.\n");
        }
    }
}
