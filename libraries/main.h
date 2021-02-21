#include "raylib.h"

#ifndef MAIN
#define MAIN

#define PODER_MAX_PERSONAGEM 10

/* Sobre o jogador:
posicao: Posição X e Y
velocidade: velocidade de movimento do jogador
podePular: condição em que pode pular
vida: quantidade de vidas do jogador 
direcao_movimento = player segue para a direita ou esquerda */

typedef struct Jogador
{
    Vector2 tamanho;
    Vector2 posicao;
    Vector2 posicaoAnterior;
    float velocidade;
    bool podePular;
    int vida;
    int direcao_movimento;
} Jogador;

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
} Inimigo;

/* Sobre os poderes:
posicao: posicao em relação ao jogador
velocidade: velocidade da movimentação
raio: raio da forma (o poder representa um círculo)
poder_ativo: verificação se está ativo ou não
direcao_movimento: direção do poder independente da do jogador 
cor: cor do poder */
typedef struct Poder
{
    Vector2 posicao;
    float raio;
    bool poder_ativo;
    bool direcao_movimento;
    Color cor;
} Poder;

typedef struct EnvItem
{
    Rectangle retangulo;
    int colisao;
    Color cor;
} EnvItem;

typedef struct Personagem
{
    Vector2 posicao;
    Texture2D texture;
    float frameWidth;
    float frameHeight;
    Rectangle frameRect;
} Personagem;

typedef struct Minions
{
    Vector2 posicao;
    Texture2D texture;
    float frameWidth;
    float frameHeight;
    Rectangle frameRect;
} Minions;

typedef struct Gados
{
    Vector2 posicao;
    Texture2D texture;
    float frameWidth;
    float frameHeight;
    Rectangle frameRect;
} Gados;

typedef struct FPS_Animacao
{
    int counter;
    float speed;
    int currentFrame;
} FPS_Animacao;

static Poder imune_19[PODER_MAX_PERSONAGEM] = {0}; //A variável inicializa zerada em suas posições
//Protótipo das funções
void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void UpdateInimigo(Inimigo *inimigo, EnvItem *envItems, Jogador *jogador, int tamanhoInimigos, int envItemsLength, float delta);
void UpdatePoder(Poder *imune_19, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void AnimacaoJogadorMovimento(FPS_Animacao *frames, Jogador *jogador,Personagem *personagem, Inimigo *inimigo, Minions *minions, int tamanhoInimigos, float deltaTime);
void AnimacaoInimigo(FPS_Animacao *frames, Inimigo *inimigo, Minions *minions, Gados *gados, int tamanhoInimigos, float deltaTime);
void AnimacaoJogadorParado(Jogador *jogador, Personagem *personagem, float delta);
void Draw(Camera2D camera, EnvItem *envItems, int envItemsLength, int tamanhoInimigo, Inimigo *inimigo, Minions *minions, Gados *gados, Jogador *jogador, Personagem *personagem);
void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
int VerificaColisaoBordasED(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto);
bool VerificaColisaoBordaS(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto, int range);
int VerificaRangeGado(Vector2 posicao_inicial, float tamanho_gado_x, float tamanho_gado_y, Rectangle jogador, float range);

#endif