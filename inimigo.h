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
    {1, {1650, 280}, 0},
    {1, {1750, 280}, 0},
    {1, {1850, 280}, 1},
    {2, {2050, 280}, 1},
    {2, {2150, 280}, 0},
    {3, {3310, 280}, 0},
    {3, {3210, 280}, 0},
    {3, {3110, 280}, 0}
};
const int tamanho_inimigoTeste = sizeof(inimigoTeste) / sizeof(inimigoTeste[0]);

//Dados dos inimigos do cenário 1
InimigoData inimigoCenario1[] = {
    {1, {860, 450}, 0},
    {1, {710, 450}, 0},
    {1, {2200, 450}, 1},
    {1, {1050, 450}, 1}
    
};
const int tamanho_inimigoCenario1 = sizeof(inimigoCenario1) / sizeof(inimigoCenario1[0]);

//Dados dos inimigos do cenário 2
InimigoData inimigoCenario2[] = {
    {1, {1320, 450}, 0},
    {1, {3970, 420}, 0},
    {1, {4500, 450}, 0},
    {1, {4800, 450}, 0},
    {1, {2870, 250}, 1},
    {2, {2700, 450}, 1},
    {2, {2150, 450}, 0},
    {2, {1500, 450}, 0},
    {3, {4850, 450}, 0},
    {3, {5000, 450}, 0},
    {3, {5150, 450}, 0},
    {3, {5300, 450}, 0},
    {3, {5450, 450}, 0},
    {3, {5600, 450}, 0}
};
const int tamanho_inimigoCenario2 = sizeof(inimigoCenario2) / sizeof(inimigoCenario2[0]);

//Dados dos inimigos do cenário 3
InimigoData inimigoCenario3[] = {
    {1, {550, 450}, 0},
    {1, {660, 450}, 0},
    {1, {760, 450}, 1},
    {2, {2380, 450}, 1},
    {2, {2280, 450}, 1},
    {1, {2690, 450}, 1},
    {1, {2800, 450}, 1},
    {1, {4220, 450}, 1},
    {2, {4320, 450}, 1},
    {2, {4500, 450}, 1},
    {1, {4600, 450}, 1}

};
const int tamanho_inimigoCenario3 = sizeof(inimigoCenario3) / sizeof(inimigoCenario3[0]);

//Dados dos inimigos do cenário 4
InimigoData inimigoCenario4[] = {
    {1, {450, 450}, 0},
    {1, {660, 450}, 0},
    {2, {560, 450}, 0},
    {1, {2490, 450}, 0},
    {2, {2330, 450}, 0},
    {1, {2770, 450}, 0},
    {2, {2640, 450}, 0},
    {1, {2950, 450}, 0},
    {2, {2850, 450}, 0},
    {2, {3100, 450}, 0},
    {2, {3380, 450}, 0},
    {1, {3290, 450}, 0},

    {1, {4580, 440}, 0},
    {2, {4780, 440}, 0},
    {2, {4690, 440}, 0},
    {2, {4800, 440}, 0},
    {1, {4920, 440}, 0},
    {2, {4800, 150}, 0},
    {1, {4720, 150}, 0}
    
};
const int tamanho_inimigoCenario4 = sizeof(inimigoCenario4) / sizeof(inimigoCenario4[0]);

//Dados dos inimigos do cenário 5
InimigoData inimigoCenario5[] = {
    {1, {150, 1320}, 0},
    {1, {300, 1320}, 0},
    {2, {530, 1320}, 0},

    {1, {2750, 630}, 0},
    {2, {2630, 630}, 0},

    {1, {20, 590}, 0},
    {2, {0, 590}, 0},

    {1, {1200, 330}, 0},
    {2, {1450, 330}, 0},

    {1, {1700, 330}, 0},
    {2, {195, 330}, 0}
    

};
const int tamanho_inimigoCenario5 = sizeof(inimigoCenario5) / sizeof(inimigoCenario5[0]);