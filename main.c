#include "raylib.h"
#include "libraries/defines.c"

bool colisaoJogador;

typedef struct Jogador
{
    Vector2 posicao;
    float velocidade;
    bool podePular;
    int vida;
    //Poder Poder; //Especificar qual o poder que um jogador tem
} Jogador;

/* Sobre os inimigos:
tipo: Tipo de inimigos
tipo = 1 = minion
tipo = 2 = gado

Hitbox: Retângulo do Minion no cenário
velocidade: velocidade de movimentação
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

/* Sobre os poderes:

posicao: posicao em relação ao jogador
velocidade: velocidade da movimentação
raio: raio da forma (o poder representa um círculo)

poder_ativo: verificação se está ativo ou não
cor: cor do poder */

//Struct relacionada ao poder à direita do jogador
typedef struct Poder
{
    Vector2 posicao;
    float velocidade;
    float raio;
    bool poder_ativo;
    Color cor;
} Poder;

//Struct relacionada ao poder à esquerda do jogador
typedef struct PoderEsq
{
    Vector2 posicao_Esq;
    float velocidade_Esq;
    float raio_Esq;
    bool poder_ativo_Esq;
    Color cor_Esq;
} PoderEsq;

typedef struct EnvItem
{
    Rectangle retangulo;
    int colisao;
    Color cor;
} EnvItem;

static Poder poderes[PODER_MAX_PERSONAGEM] = {0};
static PoderEsq poderesEsq[PODER_MAX_PERSONAGEM] = {0};

//Protótipo das funções
void UpdatePlayer(Jogador *jogador, EnvItem *envItems,Inimigo *inimigo, int envItemsLength, int tamanhoInimigo, float delta);
void UpdateInimigos(Inimigo *inimigo, EnvItem *envItems, int tamanhoInimigos, int envItemsLength, float delta);
void UpdatePoder(Poder *poderes, PoderEsq *poderesEsq, Inimigo *inimigo, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

int main()
{
    // Inicialização do Jogo
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, NOME_JOGO);
    SetTargetFPS(60);

    //Configurações Iniciais do jogador
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, 280}; //Posição Inicial
    jogador.velocidade = 0; //Velocidade Inicial
    jogador.podePular = false; //Habilitação de pulo
    jogador.vida = 1;

    //Configurações Iniciais dos inimigos
    Inimigo inimigo[] = {
        {1, {1850, 280}, 0, 0, 2, 0},
        {1, {1950, 280}, 0, 0, 2, 0}
    };
    const int tamanhoInimigo = sizeof(inimigo) / sizeof(inimigo[0]);

    //Configurações iniciais do poder à direita
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        poderes[p].posicao = (Vector2){0,0}; 
        poderes[p].velocidade = 0;
        poderes[p].raio = 10;
        poderes[p].poder_ativo = false;
        poderes[p].cor = BLACK;
    }

    //Configurações iniciais do poder à esquerda
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        poderesEsq[p].posicao_Esq = (Vector2){0,0}; 
        poderesEsq[p].velocidade_Esq = 0;
        poderesEsq[p].raio_Esq = 10;
        poderesEsq[p].poder_ativo_Esq = false;
        poderesEsq[p].cor_Esq = BLACK;
    }

    //Configurações Iniciais dos Elementos do Cenário
    EnvItem envItems[] = {
        {{0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, 0, SKYBLUE}, //Background
        {{0, 400, 2000, 200}, 1, GRAY},
        {{300, 200, 400, 10}, 1, GRAY},
        {{250, 300, 100, 10}, 1, GRAY},
        {{650, 300, 100, 10}, 1, GRAY},
        {{900, 350,  50, 50}, 1, PURPLE},
        {{1050, 311,  50, 50}, 1, PURPLE},
        {{1200, 309,  50, 50}, 1, PURPLE},
        {{1350, 330,  50, 50}, 1, PURPLE},
        {{1350, 330,  50, 50}, 1, PURPLE},
        {{1450, 340,  30, 60}, 1, GREEN},
        {{1970, 340,  30, 60}, 1, GREEN}
    };
    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    //Configurações Iniciais de Câmera
    Camera2D camera = {0};
    camera.target = jogador.posicao; //Câmera centralizada inicialmente no jogador
    camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
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

        //Atualiza o poder do jogador
        UpdatePoder(poderes, poderesEsq, inimigo, &jogador, envItems, envItemsLength, deltaTime);

        //Atualiza a Câmera focada no jogador
        UpdateCameraCenter(&camera, &jogador, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        //Desenho do Background do restante da Janela que não é objeto
        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);

        //Desenho dos Retângulos referentes aos obstáculos de EnvItems
        for (int i = 0; i < envItemsLength; i++)
            DrawRectangleRec(envItems[i].retangulo, envItems[i].cor);

        //Desenho do poder à direita
        for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
        {
            if (poderes[p].poder_ativo)
            {
                DrawCircleV(poderes[p].posicao, poderes[p].raio, BLACK);
            }
        }

        //Desenho do poder à esquerda
        for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
        {
            if (poderesEsq[p].poder_ativo_Esq)
            {
                DrawCircleV(poderesEsq[p].posicao_Esq, poderesEsq[p].raio_Esq, BLACK);
            }
        }

        //Desenho dos Retângulos referentes aos inimigos
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

        EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
} 

void UpdatePlayer(Jogador *jogador, EnvItem *envItems, Inimigo *inimigo, int envItemsLength, int tamanhoInimigo, float delta){
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
    if ((jogador->posicao.x + TAMANHO_X_JOGADOR / 2) > TAMANHO_X_CENARIO)
    {
        jogador->posicao.x = TAMANHO_X_CENARIO - TAMANHO_X_JOGADOR / 2; //Limites para direita
    } else if (jogador->posicao.x < TAMANHO_X_JOGADOR / 2)
    {
        jogador->posicao.x = TAMANHO_X_JOGADOR / 2; //Limites para a esquerda
    }
    
    if ((jogador->posicao.y) > TAMANHO_Y_CENARIO)  //Limites na vertical
    {
        jogador->posicao.y = TAMANHO_Y_CENARIO; 
    } else if (jogador->posicao.y < TAMANHO_Y_JOGADOR)
    {
        jogador->posicao.y = TAMANHO_Y_JOGADOR;
    }

    colisaoJogador = 0;
    int colisaoObjeto = 0;
    for (int i = 0; i < envItemsLength; i++) //Preechimento da área dos pixels dos objetos colidiveis
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
        if (objeto->colisao && //Detecta se o objeto é colidível
            objeto->retangulo.x - TAMANHO_X_JOGADOR/2 <= j->x && //Detecta a borda esquerda do objeto
            objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR/2 >= j->x && //Detecta a borda direita do objeto
            objeto->retangulo.y < j->y && //Detecta colisão acima do objeto
            objeto->retangulo.y + objeto->retangulo.height + TAMANHO_Y_JOGADOR > j->y) //Detecta colisão abaixo do objeto
        {
            if (objeto->retangulo.x - TAMANHO_X_JOGADOR / 2 <= j->x &&
                objeto->retangulo.x > j->x) //Detecta a colisão com a esquerda do objeto
            {
                jogador->posicao.x = objeto->retangulo.x - TAMANHO_X_JOGADOR / 2;
            }
            else if (objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR / 2 >= j->x &&
                     objeto->retangulo.x + objeto->retangulo.width < j->x) //Detecta a colisão a direita do objeto
            {
                jogador->posicao.x = objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR / 2;
            }
            else if (objeto->retangulo.y + objeto->retangulo.height + TAMANHO_Y_JOGADOR > j->y) //Detecta a colisão abaixo do objeto
            {
                jogador->posicao.y = objeto->retangulo.y + objeto->retangulo.height + TAMANHO_Y_JOGADOR;
                jogador->velocidade = GRAVIDADE * delta;
            }
            colisaoJogador = 1;
        }
    }

    if (!colisaoObjeto) //Se não há colisão com objeto
    {
        jogador->posicao.y += jogador->velocidade * delta; //Aumentar a posição do Y do jogador
        jogador->velocidade += GRAVIDADE * delta; //Vai sofrer com a Gravidade
        jogador->podePular = false; //Não pode pular no ar
    } else
        jogador->podePular = true;

    for (int i = 0; i < tamanhoInimigo; i++)
    {
        inimigo += i;
        if (inimigo->tipo > 0)
        {
            if (inimigo->posicao.x - (TAMANHO_MINION_X / 2) - (TAMANHO_X_JOGADOR / 2) < jogador->posicao.x &&
                inimigo->posicao.x + (TAMANHO_MINION_X / 2) + (TAMANHO_X_JOGADOR / 2) >= jogador->posicao.x &&
                inimigo->posicao.y - TAMANHO_MINION_Y < jogador->posicao.y &&
                inimigo->posicao.y + TAMANHO_Y_JOGADOR > jogador->posicao.y)
            {
                if (inimigo->posicao.x - (TAMANHO_MINION_X / 2) - (TAMANHO_X_JOGADOR / 2) < jogador->posicao.x)
                {
                    jogador->vida -= 1;
                }
                else if (inimigo->posicao.x + (TAMANHO_MINION_X / 2) + (TAMANHO_X_JOGADOR / 2) >= jogador->posicao.x)
                {
                    jogador->vida -= 1;
                }
            }
            else if (inimigo->posicao.x - (TAMANHO_MINION_X / 2) - (TAMANHO_X_JOGADOR / 2) < jogador->posicao.x &&
                     inimigo->posicao.x + (TAMANHO_MINION_X / 2) + (TAMANHO_X_JOGADOR / 2) >= jogador->posicao.x &&
                     inimigo->posicao.y - TAMANHO_MINION_Y - 1 <= jogador->posicao.y &&
                     inimigo->posicao.y + TAMANHO_Y_JOGADOR > jogador->posicao.y)
            {
                inimigo->tipo = 0; 
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
        if ((inimigo->posicao.x + TAMANHO_MINION_X / 2) > TAMANHO_X_CENARIO)
        {
            inimigo->posicao.x = TAMANHO_X_CENARIO - TAMANHO_MINION_X / 2; //Limites para direita
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
            if (objeto->colisao &&                                                               //Detecta se o objeto é colidível
                objeto->retangulo.x - TAMANHO_MINION_X / 2 <= j->x &&                           //Detecta a borda esquerda do objeto
                objeto->retangulo.x + objeto->retangulo.width + TAMANHO_MINION_X / 2 >= j->x && //Detecta a borda direita do objeto
                objeto->retangulo.y < j->y &&                                                    //Detecta colisão acima do objeto
                objeto->retangulo.y + objeto->retangulo.height + TAMANHO_MINION_Y > j->y)       //Detecta colisão abaixo do objeto
            {
                if (objeto->retangulo.x - TAMANHO_MINION_X / 2 <= j->x &&
                    objeto->retangulo.x > j->x) //Detecta a colisão com a esquerda do objeto
                {
                    inimigo->posicao.x = objeto->retangulo.x - TAMANHO_MINION_X / 2;
                    inimigo->direcao_movimento = 0;
                }
                else if (objeto->retangulo.x + objeto->retangulo.width + TAMANHO_MINION_X / 2 >= j->x &&
                        objeto->retangulo.x + objeto->retangulo.width < j->x) //Detecta a colisão a direita do objeto
                {
                    inimigo->posicao.x = objeto->retangulo.x + objeto->retangulo.width + TAMANHO_MINION_X / 2;
                    inimigo->direcao_movimento = 1;
                }
                else if (objeto->retangulo.y + objeto->retangulo.height + TAMANHO_MINION_Y > j->y) //Detecta a colisão abaixo do objeto
                {
                    inimigo->posicao.y = objeto->retangulo.y + objeto->retangulo.height + TAMANHO_MINION_Y;
                    inimigo->velocidade = GRAVIDADE * delta;
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

void UpdatePoder(Poder *poderes, PoderEsq *poderesEsq, Inimigo *inimigo, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta) {

    //Usando o poder na DIREITA
    if (IsKeyPressed(KEY_SPACE) && jogador->posicao.x < inimigo->posicao.x)
    {
        for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
        {
            if (!poderes[p].poder_ativo) //Caso não haja poder ativo, seta as configurações abaixo
            { 
                poderes[p].posicao = (Vector2){jogador->posicao.x - (TAMANHO_X_JOGADOR/2) + 10, jogador->posicao.y - (TAMANHO_Y_JOGADOR/2)};
                poderes[p].poder_ativo = true;
                poderes[p].velocidade = JOGADOR_MOVIMENTO_VELOCIDADE;
                break;
            }
        }
    }

    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (poderes[p].poder_ativo) //Caso haja poder ativo
        {
            poderes[p].posicao.x += JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Posição é incrementada

            for (int i = 0; i < envItemsLength; i++)
            {
                EnvItem *objeto = envItems + 1;
                if (objeto[i].colisao && CheckCollisionCircleRec(poderes[p].posicao, poderes[p].raio, objeto[i].retangulo))
                {
                    poderes[p].poder_ativo = false;
                }
            }
            
            if (poderes[p].posicao.x > TAMANHO_X_CENARIO + poderes[p].raio) //Movimentação da bola atirada é limitada até o fim do cenário
            {                                                               //no lado direito, então desaparece
                poderes[p].poder_ativo = false;
            }
        }
    }

    //Usando o poder na ESQUERDA
    if (IsKeyPressed(KEY_SPACE) && jogador->posicao.x > inimigo->posicao.x) 
    {
        for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
        {
            if (!poderesEsq[p].poder_ativo_Esq) //Caso não haja poder ativo, seta as configurações abaixo
            { 
                poderesEsq[p].posicao_Esq = (Vector2){jogador->posicao.x - (TAMANHO_X_JOGADOR/2) + 20, jogador->posicao.y - (TAMANHO_Y_JOGADOR/2)};
                poderesEsq[p].poder_ativo_Esq = true;
                poderesEsq[p].velocidade_Esq = JOGADOR_MOVIMENTO_VELOCIDADE;
                break;
            }
        }
    }

    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (poderesEsq[p].poder_ativo_Esq) //Caso haja poder ativo
        {
            poderesEsq[p].posicao_Esq.x -= JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Posição é decrementada  

            for (int i = 0; i < envItemsLength; i++)
            {
                EnvItem *objeto = envItems + 1;
                if (objeto[i].colisao && CheckCollisionCircleRec(poderesEsq[p].posicao_Esq, poderesEsq[p].raio_Esq, objeto[i].retangulo))
                {
                    poderesEsq[p].poder_ativo_Esq = false;
                }
            }

            if (poderesEsq[p].posicao_Esq.x < 0 - poderesEsq[p].raio_Esq) //Lado esquerdo do cenário é o limite do poder, então este desaparece
            {
                poderesEsq[p].poder_ativo_Esq = false;
            }
        }
    }
}

void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){width / 2, height / 2};
    camera->target = jogador->posicao;
}