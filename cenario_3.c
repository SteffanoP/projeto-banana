#include "raylib.h"
#include "libraries/defines.c"

bool colisaoJogador;

/* Sobre o jogador:
posicao: Posição X e Y
velocidade: velocidade de movimento do jogador
podePular: condição em que pode pular
vida: quantidade de vidas do jogador */
typedef struct Jogador
{
    Vector2 posicao;
    float velocidade;
    bool podePular;
    int vida;
} Jogador;

/* Sobre os inimigos:
tipo: Tipo de inimigos
tipo = 1 = minion
tipo = 2 = gado

posicao: Posição do Minion no cenário
velocidade: velocidade de movimentação
direcao_movimento: direção em que se movimenta
vida: quantidade de vidas do inimigo
cor: Cor do inimigo
*/
typedef struct Inimigo
{
    int tipo;
    Vector2 posicao;
    float velocidade;
    bool direcao_movimento;
    int vida;
    Color cor;
} Inimigo;

typedef struct EnvItem
{
    Rectangle retangulo;
    int colisao;
    Color cor;
} EnvItem;

//Cenário Principal
EnvItem envItems[22] = {0};
int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

//Cenário de Testes_3
EnvItem cenarioTeste[] = {
       {{0, 0, TAMANHO_X_CENARIO_234, TAMANHO_Y_CENARIO_234}, 0, SKYBLUE},//background
        //PARTE 1
        {{0, 550, 1050, 450}, 1, GRAY},
        {{2100, 550, 1000, 450}, 1, BROWN},         
        {{0, 470, 80, 80}, 1, BLUE},
        {{1150, 430, 180, 20}, 1, GREEN},
        {{800, 310, 180, 20}, 1, RED},
        {{890, 130, 1000, 20}, 1, PINK},
        {{635, 220, 70, 70}, 1, BLACK},
        {{565, 220, 70, 70}, 1, PURPLE},
        {{705, 220, 70, 70}, 1, PURPLE},
      

        //PARTE 2
        {{2400, 480, 70, 70}, 1, YELLOW},
        {{2470, 410, 70, 70}, 1, YELLOW},
        {{2540, 410, 70, 140}, 1, YELLOW},
        {{2610, 480, 70, 70}, 1, YELLOW},

        {{3250, 430, 170, 20}, 1, GRAY},
        {{3250, 210, 170, 20}, 1, BLUE},
        
        {{3220, 2, 210, 70}, 1, PURPLE},
        {{3290, 2, 70, 70}, 1, BLACK},

        {{3550, 310, 70, 70}, 1, WHITE},
        {{3900, 550, 2300, 450}, 1, PINK},
        {{4000, 480, 210, 70}, 1, BLUE},
        {{4070, 410, 70, 140}, 1, BLUE},
        {{6150, 370, 140, 200}, 1, ORANGE},
};
const int tamanhoCenarioTeste = sizeof(cenarioTeste) / sizeof(cenarioTeste[0]);

bool momento_transicao = 0;

//Protótipo das funções
void UpdatePlayer(Jogador *jogador, EnvItem *envItems,Inimigo *inimigo, int envItemsLength, int tamanhoInimigo, float delta);
void UpdateInimigos(Inimigo *inimigo, EnvItem *envItems, int tamanhoInimigos, int envItemsLength, float delta);
void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
int VerificaColisaoBordasED(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto);
bool VerificaColisaoBordaS(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto);
void CarregaCenario(EnvItem *cenario, int tamanhoCenario, EnvItem *loadCenario, int tamanhoLoadCenario);

int main()
{
    // Inicialização do Jogo
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, NOME_JOGO);

    //Configurações Iniciais do jogador
    Jogador jogador = {0};
    jogador.posicao = (Vector2){0, 280}; //Posição Inicial
    jogador.velocidade = 0; //Velocidade Inicial
    jogador.podePular = false; //Habilitação de pulo
    jogador.vida = 1;

    //Configurações Iniciais dos inimigos
    Inimigo inimigo[] = {
        {1, {1850, 280}, 0, 0, 2, 0},
        {1, {1950, 280}, 0, 0, 2, 0}
    };
    const int tamanhoInimigo = sizeof(inimigo) / sizeof(inimigo[0]);

    //Configura cenário inicial
    CarregaCenario(envItems, envItemsLength, cenarioTeste, tamanhoCenarioTeste);

    //Configurações Iniciais de Câmera
    Camera2D camera = {0};
    camera.target = jogador.posicao; //Câmera centralizada inicialmente no jogador
    camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
    camera.rotation = 0.0f;
    camera.zoom = 0.5f;
    //--------------------------------------------------------------------------------------

    //O Jogo
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();
        
        //Atualiza os dados do jogador
        if (jogador.vida > 0)
        {
            UpdatePlayer(&jogador, envItems, inimigo, envItemsLength, tamanhoInimigo, deltaTime);
        }
        
        //Atualiza os dados dos inimigos
        UpdateInimigos(inimigo, envItems, tamanhoInimigo, envItemsLength, deltaTime);
        
        //Atualiza a Câmera focada no jogador
        UpdateCameraCenter(&camera, &jogador, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        //Desenho do Background do restante da Janela que não é objeto
        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);
        //impressão do background
        Texture2D cenario_fundo = LoadTexture("cenario/laranja (1).png");
        DrawTextureRec(cenario_fundo, cenarioTeste[0].retangulo, (Vector2){0, 0}, RAYWHITE);

       //impressão do piso
       //parte 1
        Texture2D cinza1 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(cinza1, cenarioTeste[1].retangulo, (Vector2){0, 550}, RAYWHITE);
        Texture2D marrom1 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(marrom1, cenarioTeste[2].retangulo, (Vector2){2100, 550}, RAYWHITE);
        Texture2D azul1 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(azul1, cenarioTeste[3].retangulo, (Vector2){0, 470}, RAYWHITE);
        Texture2D verde1 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(verde1, cenarioTeste[4].retangulo, (Vector2){1150, 430}, RAYWHITE);
        Texture2D vermelho1 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(vermelho1, cenarioTeste[5].retangulo, (Vector2){800, 310}, RAYWHITE);
        Texture2D rosa1 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(rosa1, cenarioTeste[6].retangulo, (Vector2){890, 130}, RAYWHITE);
        Texture2D preto1 = LoadTexture("cenario/bloco_mario.png");
        DrawTextureRec(preto1, cenarioTeste[7].retangulo, (Vector2){635, 220}, RAYWHITE);
        Texture2D roxo1 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(roxo1, cenarioTeste[8].retangulo, (Vector2){565, 220}, RAYWHITE);
        Texture2D roxo11 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(roxo11, cenarioTeste[9].retangulo, (Vector2){705, 220}, RAYWHITE);

        //parte 2
        //escada
        Texture2D amarelo2 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(amarelo2, cenarioTeste[10].retangulo, (Vector2){2400, 480}, RAYWHITE);
        Texture2D amarelo22 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(amarelo22, cenarioTeste[11].retangulo, (Vector2){2470, 410}, RAYWHITE);
        Texture2D amarelo222 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(amarelo222, cenarioTeste[12].retangulo, (Vector2){2540, 410}, RAYWHITE);
        Texture2D amarelo2222 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(amarelo2222, cenarioTeste[13].retangulo, (Vector2){2610, 480}, RAYWHITE);

        Texture2D cinza2 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(cinza2, cenarioTeste[14].retangulo, (Vector2){3250, 430}, RAYWHITE);
        Texture2D azul2 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(azul2, cenarioTeste[15].retangulo, (Vector2){3250, 210}, RAYWHITE);
        Texture2D roxo2 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(roxo2, cenarioTeste[16].retangulo, (Vector2){3220, 2}, RAYWHITE);
        Texture2D preto2 = LoadTexture("cenario/bloco_mario.png");
        DrawTextureRec(preto2, cenarioTeste[17].retangulo, (Vector2){3290, 2}, RAYWHITE);
        Texture2D branco2 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(branco2, cenarioTeste[18].retangulo, (Vector2){3550, 310}, RAYWHITE);
        Texture2D rosa2 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(rosa2, cenarioTeste[19].retangulo, (Vector2){3900, 550}, RAYWHITE);
        Texture2D azul_2 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(azul_2, cenarioTeste[20].retangulo, (Vector2){4000, 480}, RAYWHITE);
        Texture2D azul22 = LoadTexture("cenario/chao_mario.png");
        DrawTextureRec(azul22, cenarioTeste[21].retangulo, (Vector2){4070, 410}, RAYWHITE);
        Texture2D laranja2 = LoadTexture("cenario/Bandeira.png");
        DrawTextureRec(laranja2, cenarioTeste[22].retangulo, (Vector2){5950, 350}, RAYWHITE);


        //Desenho dos Retângulos referentes aos obstáculos de EnvItems
        for (int i = 0; i < envItemsLength; i++)
            DrawRectangleRec(envItems[350].retangulo, envItems[i].cor);

        for (int i = 0; i < tamanhoInimigo; i++)
        {
            if (inimigo[i].tipo > 0)
            {
                Rectangle inimigoRect = {inimigo[i].posicao.x - TAMANHO_MINION_X / 2, inimigo[i].posicao.y - TAMANHO_MINION_Y, TAMANHO_MINION_X, TAMANHO_MINION_Y}; //Desenho do inimigo
                DrawRectangleRec(inimigoRect, YELLOW);                                                                                                                  //Desenha o desenho do inimigo
            }
        }

        //Criação e Desenho do jogador
        Rectangle playerRect = {jogador.posicao.x - TAMANHO_X_JOGADOR / 2, jogador.posicao.y - TAMANHO_Y_JOGADOR, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR}; //Desenho do jogador
        DrawRectangleRec(playerRect, RED); //Desenha o desenho do jogador

        DrawText(FormatText("Colisão : %01i", colisaoJogador), 1000, 450, 20, BLACK);

        DrawText(FormatText("Exemplo de Inimigo"), 1650, 450, 20, BLACK);
        DrawText(FormatText("Vida Jogador: %01i",jogador.vida), 1650, 475, 20, BLACK);

        EndMode2D();

        EndDrawing();
        UnloadTexture(cinza1);
        UnloadTexture(marrom1);
        UnloadTexture(azul1);
        UnloadTexture(verde1);
        UnloadTexture(vermelho1);
        UnloadTexture(rosa1);
        UnloadTexture(preto1);
        UnloadTexture(roxo1);
        UnloadTexture(roxo11);
        UnloadTexture(amarelo2);
        UnloadTexture(amarelo22);
        UnloadTexture(amarelo222);
        UnloadTexture(amarelo2222);
        UnloadTexture(cinza2);
        UnloadTexture(azul2);
        UnloadTexture(roxo2);
        UnloadTexture(preto2);
        UnloadTexture(branco2);
        UnloadTexture(rosa2);
        UnloadTexture(azul_2);
        UnloadTexture(azul22);
        UnloadTexture(laranja2);
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
} 

void UpdatePlayer(Jogador *jogador, EnvItem *envItems, Inimigo *inimigo, int envItemsLength, int tamanhoInimigo, float delta)
{  
    if (IsKeyDown(KEY_LEFT)) //Movimentação para a Esquerda
        jogador->posicao.x -= JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Decrementa o valor da posição do player
    if (IsKeyDown(KEY_RIGHT)) //Movimentação para a Direita
        jogador->posicao.x += JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Incrementa o valor da posição do player

    if (IsKeyDown(KEY_UP) && jogador->podePular)
    {
        jogador->velocidade = -JOGADOR_PULO_VELOCIDADE;
        jogador->podePular = false;
    }
    
    //Limites da area de movimentação do jogador
    if ((jogador->posicao.x + TAMANHO_X_JOGADOR / 2) > TAMANHO_X_CENARIO_234)
    {
        jogador->posicao.x = TAMANHO_X_CENARIO_234 - TAMANHO_X_JOGADOR / 2; //Limites para direita
    } else if (jogador->posicao.x < TAMANHO_X_JOGADOR / 2)
    {
        jogador->posicao.x = TAMANHO_X_JOGADOR / 2; //Limites para a esquerda
    }
    
    if ((jogador->posicao.y) > TAMANHO_Y_CENARIO_234)  //Limites na vertical
    {
        jogador->posicao.y = TAMANHO_Y_CENARIO_234; 
    } else if (jogador->posicao.y < TAMANHO_Y_JOGADOR)
    {
        jogador->posicao.y = TAMANHO_Y_JOGADOR;
    }

    colisaoJogador = 0;
    int colisaoObjeto = 0;
    for (int i = 0; i < envItemsLength; i++) //Preechimento da áre dos pixels dos objetos colidiveis
    {
        EnvItem *objeto = envItems + i;
        Vector2 *j = &(jogador->posicao);
        
        //Condição de colisão para pulo e andar encima de plataformas
        if (objeto->colisao &&                                             
            objeto->retangulo.x - TAMANHO_X_JOGADOR/2 <= j->x &&                                      //
            objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR/2 >= j->x &&            // Definindo a invasão da área do player com a área do objeto(área de colisão)   
            objeto->retangulo.y >= j->y &&                    
            objeto->retangulo.y < j->y + jogador->velocidade * delta)
        {
            colisaoObjeto = 1; 
            jogador->velocidade = 0.0f; //Reduzindo a velocidade do player para 0, para freiar ele             
            j->y = objeto->retangulo.y; //Atualiza a variável do movimento
        }

        //Condição de colisão em objetos Universais
        if (objeto->colisao)
        {
            if (VerificaColisaoBordaS(jogador->posicao, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, objeto->retangulo))
            {
                jogador->posicao.y = objeto->retangulo.y + objeto->retangulo.height + TAMANHO_Y_JOGADOR + 1;
                jogador->velocidade = GRAVIDADE * delta;
            } 
            else if (VerificaColisaoBordasED(jogador->posicao, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, objeto->retangulo) == 1)
            {
                jogador->posicao.x = objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR / 2;
            }
            else if (VerificaColisaoBordasED(jogador->posicao, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, objeto->retangulo) == 2)
            {
                jogador->posicao.x = objeto->retangulo.x - TAMANHO_X_JOGADOR / 2;
            }
        }
    }

    if (!colisaoObjeto) //Se não há colisão com objeto
    {
        jogador->posicao.y += jogador->velocidade * delta; //Aumentar a posição do Y do jogador
        jogador->velocidade += GRAVIDADE * delta; //Vai sofrer com a Gravidade
        jogador->podePular = false; //Não pode pular no ar
    } else
        jogador->podePular = true;
    
    //Verifica colisão entre jogador e inimigo
    Rectangle ret_jogador = {jogador->posicao.x - (TAMANHO_X_JOGADOR / 2),jogador->posicao.y - TAMANHO_Y_JOGADOR, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR};
    for (int i = 0; i < tamanhoInimigo; i++) //Passa por todos os inimigos
    {
        inimigo += i;
        
        Rectangle ret_inimigo = {inimigo->posicao.x - (TAMANHO_MINION_X / 2), inimigo->posicao.y - TAMANHO_MINION_Y, TAMANHO_MINION_X, TAMANHO_MINION_Y};
        //Verifica colisão entre minion e jogador
        if (inimigo->tipo == 1)
        {
            //Verifica se jogador encosta nas bordas do objeto minion
            if (VerificaColisaoBordasED(jogador->posicao,TAMANHO_X_JOGADOR,TAMANHO_Y_JOGADOR,ret_inimigo) != 0)
            {
                jogador->vida -= 1; //Jogador encosta em minion e perde vida
            } 
            //Verifica se borda superior de minion encosta em objeto jogador
            else if (VerificaColisaoBordaS(inimigo->posicao,TAMANHO_MINION_X,TAMANHO_MINION_Y,ret_jogador))
            {
                inimigo->tipo = 0; //Jogador mata o minion
            }
        }
    }
}

void UpdateInimigos(Inimigo *inimigo, EnvItem *envItems, int tamanhoInimigos, int envItemsLength, float delta)
{
    for (int i = 0; i < tamanhoInimigos; i++)
    {
        inimigo += i;

        if (inimigo->tipo == 1)
        {
            if (inimigo->direcao_movimento == 0)
                inimigo->posicao.x -= VELOCIDADE_INIMIGO_MINION * delta;
            else if (inimigo->direcao_movimento == 1)
                inimigo->posicao.x += VELOCIDADE_INIMIGO_MINION * delta;
        }

        //Limites da area de movimentação do inimigo no cenário
        if ((inimigo->posicao.x + TAMANHO_MINION_X / 2) > TAMANHO_X_CENARIO_234)
        {
            inimigo->posicao.x = TAMANHO_X_CENARIO_234 - TAMANHO_MINION_X / 2; //Limites para direita
            inimigo->direcao_movimento = !inimigo->direcao_movimento;
        }
        else if (inimigo->posicao.x < TAMANHO_MINION_X / 2)
        {
            inimigo->posicao.x = TAMANHO_MINION_X / 2; //Limites para a esquerda
            inimigo->direcao_movimento = !inimigo->direcao_movimento;
        }

        int colisaoObjeto = 0;
        for (int i = 0; i < envItemsLength; i++) //Preechimento da área dos pixels dos objetos colidiveis
        {
            EnvItem *objeto = envItems + i;
            Vector2 *j = &(inimigo->posicao);

            //Condição de colisão para andar encima de plataformas
            if (objeto->colisao &&
                objeto->retangulo.x - TAMANHO_MINION_X / 2 <= j->x &&                           
                objeto->retangulo.x + objeto->retangulo.width + TAMANHO_MINION_X / 2 >= j->x && // Definindo a invasão da área do inimigo com a área do objeto(área de colisão)
                objeto->retangulo.y >= j->y &&
                objeto->retangulo.y < j->y + inimigo->velocidade * delta)
            {
                colisaoObjeto = 1;
                inimigo->velocidade = 0.0f; //Reduzindo a velocidade do player para 0, para freiar ele
                j->y = objeto->retangulo.y; //Atualiza a variável do movimento
            }

            //Condição de colisão em objetos Universais
            if (objeto->colisao)
            {
                if (VerificaColisaoBordasED(inimigo->posicao, TAMANHO_MINION_X, TAMANHO_MINION_Y, objeto->retangulo) == 1)
                {
                    inimigo->direcao_movimento = 1;
                }
                else if (VerificaColisaoBordasED(inimigo->posicao, TAMANHO_MINION_X, TAMANHO_MINION_Y, objeto->retangulo) == 2)
                {
                    inimigo->direcao_movimento = 0;
                }
            }
        }

        if (!colisaoObjeto) //Se não há colisão com objeto
        {
            inimigo->posicao.y += inimigo->velocidade * delta; //Aumentar a posição do Y do inimigo
            inimigo->velocidade += GRAVIDADE * delta;          //Vai sofrer com a Gravidade
        }
    }
}

void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){width / 2, height / 2};
    camera->target = jogador->posicao;
}

/*
Verifica se há colisão com as bordas Esquerda e Direita de uma Entidade com um objeto
Retorna 0 se não há colisão
Retorna 1 se há colisão com borda esquerda
Retorna 2 se há colisão com borda direita
*/
int VerificaColisaoBordasED(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto) {
    const float ponto_superior_esquerda = entidade.x - (tamanho_entidade_x / 2);
    const float ponto_superior_direita = entidade.x + (tamanho_entidade_x / 2);
    const float ponto_superior = entidade.y - tamanho_entidade_y + 1;
    const float ponto_inferior = entidade.y - 1;

    //Verifica a reta (conjunto de ~infinitos~ pontos) do ponto superior ao inferior
    for (float ponto = ponto_superior; ponto <= ponto_inferior; ponto++)
    {
        //Verifica se há colisão com borda/reta esquerda
        if (CheckCollisionPointRec((Vector2){ponto_superior_esquerda,ponto},objeto))
        {
            return 1;
        }
        //Verifica se há colisão com borda/reta direita
        if (CheckCollisionPointRec((Vector2){ponto_superior_direita,ponto},objeto))
        {
            return 2;
        }
    }

    return 0;
}

/*
Verifica se há colisão com a borda superior de uma Entidade com um objeto
Retorna 0 se não há colisão
Retorna 1 se há colisão com borda superior
*/
bool VerificaColisaoBordaS(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto) {
    Vector2 ponto_superior_esquerda = (Vector2){entidade.x - (tamanho_entidade_x / 2) + 5, entidade.y - tamanho_entidade_y - 1};
    Vector2 ponto_superior_direita = (Vector2){entidade.x + (tamanho_entidade_x / 2) - 5, entidade.y - tamanho_entidade_y - 1};

    //Verifica a colisão entre 2 pontos superiores da entidade
    if (CheckCollisionPointRec(ponto_superior_esquerda,objeto) ||
        CheckCollisionPointRec(ponto_superior_direita,objeto))
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

void CarregaCenario(EnvItem *cenario, int tamanhoCenario, EnvItem *loadCenario, int tamanhoLoadCenario)
{
    for (int i = 0; i < tamanhoLoadCenario; i++)
    {
        cenario[i] = loadCenario[i];
    }
    tamanhoCenario = tamanhoLoadCenario;
}