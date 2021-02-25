/* Sobre os inimigos:
tipo: Tipo de inimigos
tipo = 1 = minion
tipo = 2 = gado
tamanho: Tamanho do inimigo no cenário
posicao: Posição do Inimigo no cenário
velocidade: velocidade de movimentação
direcao_movimento: direção em que se movimenta
vida: quantidade de vidas do inimigo
cor: Cor do inimigo*/
typedef struct Inimigo
{
    int tipo;
    Vector2 tamanho;
    Vector2 posicao;
    float velocidade;
    bool direcao_movimento;
    int vida;
    Color cor;
    bool podePular;
} Inimigo;

typedef struct InimigoData
{
    int tipo;
    Vector2 posicao;
    bool direcao_movimento;
} InimigoData;

//Variável de inimigo Global: que está sendo carregado
Inimigo inimigo[30] = {0};
int tamanhoInimigo = sizeof(inimigo) / sizeof(inimigo[0]);

//Variáveis de Armazenamento de inimigo:
InimigoData inimigoTeste[] = {
    {1, {1650, 280}},
    {1, {1750, 280}},
    {1, {1850, 280}},
    {2, {2050, 280}},
    {2, {2150, 280}},
    {3, {3310, 280}},
    {3, {3210, 280}},
    {3, {3110, 280}}
};
const int tamanho_inimigoTeste = sizeof(inimigoTeste) / sizeof(inimigoTeste[0]);

//Dados dos inimigos do cenário 1
InimigoData inimigoCenario1[] = {
    {1, {1650, 280}},
    {2, {1650, 280}}
};
const int tamanho_inimigoCenario1 = sizeof(inimigoCenario1) / sizeof(inimigoCenario1[0]);

//Dados dos inimigos do cenário 2
InimigoData inimigoCenario2[] = {
    {1, {1650, 280}},
    {1, {1750, 280}},
    {2, {2050, 280}},
    {2, {2150, 280}}
};
const int tamanho_inimigoCenario2 = sizeof(inimigoCenario2) / sizeof(inimigoCenario2[0]);

//Dados dos inimigos do cenário 3
InimigoData inimigoCenario3[] = {
    {1, {1650, 280}},
    {1, {1750, 280}},
    {2, {2050, 280}},
    {2, {2150, 280}}
};
const int tamanho_inimigoCenario3 = sizeof(inimigoCenario3) / sizeof(inimigoCenario3[0]);

//Dados dos inimigos do cenário 4
InimigoData inimigoCenario4[] = {
    {1, {1650, 280}},
    {1, {1750, 280}},
    {2, {2050, 280}},
    {2, {2150, 280}}
};
const int tamanho_inimigoCenario4 = sizeof(inimigoCenario4) / sizeof(inimigoCenario4[0]);