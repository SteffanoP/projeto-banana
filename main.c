#include "raylib.h"
#include "libraries/defines.c"

//Ativação da Biblioteca Physac.h
#define PHYSAC_IMPLEMENTATION
#include "physac.h"

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
    bool colisao;
    Rectangle retangulo;
    Color cor;
} EnvItem;

//Configurações Iniciais dos inimigos
Inimigo inimigo[] = {
    {1, {1850, 280}, 0, 0, 2, 0}
};
int tamanhoInimigos = sizeof(inimigo) / sizeof(inimigo[0]);

//Configurações Iniciais dos Elementos do Cenário
EnvItem objetosCenario[] = {
    {0, {0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, SKYBLUE}, //Background
    {1, {0, 400, 2000, 200}, GRAY},
    {1, {300, 200, 400, 10}, GRAY},
    {1, {250, 300, 100, 10}, GRAY},
    {1, {650, 300, 100, 10}, GRAY},
    {1, {900, 350, 50, 50}, PURPLE},
    {1, {1050, 311, 50, 50}, PURPLE},
    {1, {1200, 309, 50, 50}, PURPLE},
    {1, {1350, 330, 50, 50}, PURPLE},
    {1, {1450, 340, 30, 60}, GREEN},
    {1, {1970, 340, 30, 60}, GREEN}
};
const int tamanho_objetosCenario = sizeof(objetosCenario) / sizeof(objetosCenario[0]);

//Protótipo das funções
void UpdateJogador(Jogador *jogador, PhysicsBody corpo);
void UpdateInimigo(Inimigo *inimigo, PhysicsBody corpo);
PhysicsBody CriaObjetoCenario(EnvItem envItem);
PhysicsBody CriaCorpoInimigo(Inimigo inimigo);
void ConfiguraJogador(Jogador *jogador);

int main()
{
    // Inicialização do Jogo
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, NOME_JOGO);

    //Iniciar Física da Physac
    InitPhysics();
    SetPhysicsGravity(0,5.0f);

    //Configurações Iniciais do jogador
    Jogador jogador = {0};
    ConfiguraJogador(&jogador);

    //Criação de Corpos/Objetos Fixos   
    PhysicsBody cenario[tamanho_objetosCenario];
    for (int i = 0; i < tamanho_objetosCenario; i++)
    {
        if (objetosCenario[i].colisao > 0) {
            cenario[i] = CriaObjetoCenario(objetosCenario[i]);
            cenario[i]->enabled = false;
        }
    }

    //Criação de Corpos dos inimigos
    PhysicsBody corpo_inimigo[tamanhoInimigos];
    for (int i = 0; i < tamanhoInimigos; i++)
    {
        corpo_inimigo[i] = CriaCorpoInimigo(inimigo[i]);
        corpo_inimigo[i]->freezeOrient = true;
        corpo_inimigo[i]->inertia = 0;
    }
    
    //Corpo do jogador
    PhysicsBody corpo_jogador = (PhysicsBody)PHYSAC_MALLOC(sizeof(PhysicsBody));
    corpo_jogador = CreatePhysicsBodyRectangle(jogador.posicao, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, 10);
    corpo_jogador->freezeOrient = true;

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
        SetTargetFPS(60);
        float deltaTime = GetFrameTime();
        
        //Atualiza os dados do jogador
        if (jogador.vida > 0)
        {
            UpdateJogador(&jogador, corpo_jogador);
        }
        
        //Atualiza os dados dos inimigos
        for (int i = 0; i < tamanhoInimigos; i++)
        {
            UpdateInimigo(&inimigo[i],corpo_inimigo[i]);
        }
        
        //Atualiza a Câmera focada no jogador
        UpdateCameraCenter(&camera, &jogador, objetosCenario, tamanho_objetosCenario, deltaTime, screenWidth, screenHeight);
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
        
        //Desenho do corpo do jogador
        int quantidadeCorpos = GetPhysicsBodiesCount();
        for (int i = 0; i < quantidadeCorpos; i++)
        {
            PhysicsBody corpos = GetPhysicsBody(i);

            int vertexCount = GetPhysicsShapeVerticesCount(i);
            for (int j = 0; j < vertexCount; j++)
            {
                // Get physics bodies shape vertices to draw lines
                // Note: GetPhysicsShapeVertex() already calculates rotation transformations
                Vector2 vertexA = GetPhysicsShapeVertex(corpos, j);

                int jj = (((j + 1) < vertexCount) ? (j + 1) : 0); // Get next vertex or first to close the shape
                Vector2 vertexB = GetPhysicsShapeVertex(corpos, jj);

                DrawLineV(vertexA, vertexB, BLUE); // Draw a line between two vertex positions
            }
        }

        DrawText(FormatText("Colisão : %01i", colisaoJogador), 1000, 450, 20, BLACK);

        DrawText(FormatText("Exemplo de Inimigo"), 1650, 450, 20, BLACK);
        DrawText(FormatText("Vida Jogador: %01i",jogador.vida), 1650, 475, 20, BLACK);

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

void UpdateJogador(Jogador *jogador,PhysicsBody corpo)
{  
    if (IsKeyDown(KEY_LEFT))
    {
        corpo->velocity.x = -JOGADOR_MOVIMENTO_VELOCIDADE;
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        corpo->velocity.x = JOGADOR_MOVIMENTO_VELOCIDADE; //Decrementa o valor da posição do player
    }

    if (IsKeyDown(KEY_UP) && corpo->isGrounded)
    {
        corpo->velocity.y = -JOGADOR_PULO_VELOCIDADE;
    }
    jogador->posicao = corpo->position;
    
    // for (int i = 0; i < tamanhoInimigo; i++)
    // {
    //     inimigo += i;
    //     if (inimigo->tipo > 0)
    //     {
    //         if (inimigo->posicao.x - (TAMANHO_MINION_X / 2) - (TAMANHO_X_JOGADOR / 2) < jogador->posicao.x &&
    //             inimigo->posicao.x + (TAMANHO_MINION_X / 2) + (TAMANHO_X_JOGADOR / 2) >= jogador->posicao.x &&
    //             inimigo->posicao.y - TAMANHO_MINION_Y < jogador->posicao.y &&
    //             inimigo->posicao.y + TAMANHO_Y_JOGADOR > jogador->posicao.y)
    //         {
    //             if (inimigo->posicao.x - (TAMANHO_MINION_X / 2) - (TAMANHO_X_JOGADOR / 2) < jogador->posicao.x)
    //             {
    //                 jogador->vida -= 1;
    //             }
    //             else if (inimigo->posicao.x + (TAMANHO_MINION_X / 2) + (TAMANHO_X_JOGADOR / 2) >= jogador->posicao.x)
    //             {
    //                 jogador->vida -= 1;
    //             }
    //         }
    //         else if (inimigo->posicao.x - (TAMANHO_MINION_X / 2) - (TAMANHO_X_JOGADOR / 2) < jogador->posicao.x &&
    //                  inimigo->posicao.x + (TAMANHO_MINION_X / 2) + (TAMANHO_X_JOGADOR / 2) >= jogador->posicao.x &&
    //                  inimigo->posicao.y - TAMANHO_MINION_Y - 1 <= jogador->posicao.y &&
    //                  inimigo->posicao.y + TAMANHO_Y_JOGADOR > jogador->posicao.y)
    //         {
    //             inimigo->tipo = 0; 
    //         } 
    //     }
    // }
    
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

    for (int i = 0; i < tamanho_objetosCenario; i++)
    {
        if (objetosCenario[i].colisao)
        {
            if (ColisaoInimigoObjeto(inimigo->posicao,TAMANHO_MINION_X,TAMANHO_MINION_Y,objetosCenario[i].retangulo))
            {
                inimigo->direcao_movimento = !inimigo->direcao_movimento;
                if (inimigo->direcao_movimento > 0)
                    corpo->velocity.x = VELOCIDADE_INIMIGO_MINION;
                else 
                    corpo->velocity.x = -VELOCIDADE_INIMIGO_MINION;
            }
        }
    }
}

void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *objetosCenario, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){width / 2, height / 2};
    camera->target = jogador->posicao;
}

void ConfiguraJogador(Jogador *jogador) {
    //Configurações do jogador
    jogador->posicao = (Vector2){400, 280}; //Posição Inicial
    jogador->velocidade = 0;                //Velocidade Inicial
    jogador->vida = 1;                      //Define a quantidade de vidas iniciais
}

PhysicsBody CriaObjetoCenario(EnvItem envItem) {
    PhysicsBody objeto = CreatePhysicsBodyRectangle((Vector2){envItem.retangulo.x + (envItem.retangulo.width / 2),envItem.retangulo.y + (envItem.retangulo.height / 2)}, envItem.retangulo.width, envItem.retangulo.height, 1);
    return objeto;
} 
            }
} 

PhysicsBody CriaCorpoInimigo(Inimigo inimigo) {
    PhysicsBody objeto = CreatePhysicsBodyRectangle(inimigo.posicao, TAMANHO_MINION_X, TAMANHO_MINION_Y, 1);
    return objeto;
} 

bool ColisaoInimigoObjeto(Vector2 inimigo,float tamanhoInimigo_x, float tamanhoInimigo_y,Rectangle objeto) {
    Vector2 ponto_superior_esquerdo = (Vector2){inimigo.x - tamanhoInimigo_x / 2,inimigo.y};
    Vector2 ponto_inferior_esquerdo = (Vector2){inimigo.x - tamanhoInimigo_x / 2,inimigo.y + (tamanhoInimigo_y / 2) - 1};
    Vector2 ponto_superior_direito = (Vector2){inimigo.x + tamanhoInimigo_x / 2, inimigo.y};
    Vector2 ponto_inferior_direito = (Vector2){inimigo.x + tamanhoInimigo_x / 2, inimigo.y + (tamanhoInimigo_y / 2) - 1};

    if (CheckCollisionPointRec(ponto_superior_esquerdo, objeto) ||
        CheckCollisionPointRec(ponto_inferior_esquerdo, objeto) ||
        CheckCollisionPointRec(ponto_superior_direito, objeto) ||
        CheckCollisionPointRec(ponto_inferior_direito, objeto))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}