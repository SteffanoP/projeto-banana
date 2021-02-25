/*
colisao: tipo de bloco de colisão
colisao = 1 = bloco normal com colisão
colisao = 2 = bloco com poder imune-19
*/
typedef struct EnvItem
{
    Rectangle retangulo;
    int colisao;
    Color cor;
} EnvItem;

//Variável do cenário Global: que está sendo carregado
EnvItem envItems[40] = {0};
int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

//Variáveis de armazenamento de cenário:
EnvItem cenarioTeste[] = {
    {{0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, 0, SKYBLUE}, //Background
    {{0, 400, 5070, 200}, 1, GRAY},
    {{300, 200, 400, 10}, 1, GRAY},
    {{250, 300, 100, 10}, 1, GRAY},
    {{650, 300, 100, 10}, 1, GRAY},
    {{900, 350, 50, 50}, 1, PURPLE},
    {{1050, 311, 50, 50}, 1, PURPLE},
    {{1200, 308, 50, 50}, 1, PURPLE},
    {{1350, 330, 50, 50}, 1, PURPLE},
    {{1450, 340, 30, 60}, 1, GREEN},
    {{1970, 340, 30, 60}, 1, GREEN},
    {{2490, 340, 30, 60}, 1, GREEN},
    {{2700, 200, 50, 50}, 2, BLACK},
    {{3010, 340, 30, 60}, 1, GREEN},
    {{3530, 270, 30, 130}, 1, GREEN},
    {{4000, 270, 30, 130}, 1, GREEN},
    {{4520, 270, 30, 130}, 1, GREEN},
    {{5040, 270, 30, 130}, 1, GREEN}
};

const int tamanhoCenarioTeste = sizeof(cenarioTeste) / sizeof(cenarioTeste[0]);

EnvItem cenario1[] = {
    {{0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, 0, SKYBLUE}, //Background
    {{0, 400, 5070, 200}, 1, RED}
};
const int tamanhoCenario1 = sizeof(cenario1) / sizeof(cenario1[0]);

EnvItem cenario2[] = {
    {{0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, 0, SKYBLUE}, //Background
    {{0, 400, 5070, 200}, 1, RED}
};
const int tamanhoCenario2 = sizeof(cenario2) / sizeof(cenario2[0]);

EnvItem cenario3[] = {
    {{0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, 0, SKYBLUE}, //Background
    {{0, 400, 5070, 200}, 1, RED}
};
const int tamanhoCenario3 = sizeof(cenario3) / sizeof(cenario3[0]);

EnvItem cenario4[] = {
    {{0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, 0, SKYBLUE}, //Background
    {{0, 400, 5070, 200}, 1, RED}
};
const int tamanhoCenario4 = sizeof(cenario4) / sizeof(cenario4[0]);