#include "orion_campus.h"
#include <assert.h> // Para validações rigorosas (Contrato de execução)

/* ==========================================================================
 * CONTROLE DA FILA DE MISSÕES (A Locomotiva e o Fim do Trem)
 * ========================================================================== */
// Ponteiros estáticos garantem que a fila não seja acessada fora deste arquivo
static Missao* head_fila = NULL;
static Missao* tail_fila = NULL;

/* ==========================================================================
 * FUNÇÕES AUXILIARES DE ENCADEAMENTO ("Mecânica dos Engates")
 * ========================================================================== */

/*
 * Desengata um vagão (Missão) da fila sem destruí-lo.
 * Utilizado para reordenação durante o processo de envelhecimento.
 */
void remover_no_da_fila(Missao* alvo) {
    assert(alvo != NULL && "Tentativa de remover um vagão nulo da fila.");

    // Se possui um vagão anterior, o próximo do anterior passa a ser o próximo do alvo
    if (alvo->anterior != NULL) {
        alvo->anterior->proximo = alvo->proximo;
    } else {
        // Se não tem anterior, era a locomotiva (head). Atualiza o head.
        head_fila = alvo->proximo;
    }

    // Se possui um vagão depois, o anterior dele passa a ser o anterior do alvo
    if (alvo->proximo != NULL) {
        alvo->proximo->anterior = alvo->anterior;
    } else {
        // Se não tem próximo, era o último vagão (tail). Atualiza o tail.
        tail_fila = alvo->anterior;
    }

    // Limpa os engates do vagão isolado para evitar dangling pointers
    alvo->anterior = NULL;
    alvo->proximo = NULL;
}

/*
 * Insere um vagão na fila de forma ordenada com base na PRIORIDADE.
 * Complexidade: O(N) no pior caso.
 */
static void inserir_missao_ordenada(Missao* nova_missao) {
    assert(nova_missao != NULL && "Tentativa de inserir vagão nulo na fila.");

    // Caso 1: Fila vazia (Primeiro vagão)
    if (head_fila == NULL) {
        head_fila = nova_missao;
        tail_fila = nova_missao;
        return;
    }

    // Percorre a fila procurando a posição de engate (Ordenação Decrescente)
    Missao* atual = head_fila;
    while (atual != NULL && atual->prioridade >= nova_missao->prioridade) {
        atual = atual->proximo;
    }

    // Caso 2: Inserção no Fim do trem (Tail)
    if (atual == NULL) {
        tail_fila->proximo = nova_missao;
        nova_missao->anterior = tail_fila;
        tail_fila = nova_missao;
        return;
    }

    // Caso 3: Inserção no Início do trem (Substitui o Head)
    if (atual->anterior == NULL) {
        nova_missao->proximo = head_fila;
        head_fila->anterior = nova_missao;
        head_fila = nova_missao;
        return;
    }

    // Caso 4: Inserção no Meio do trem (Desengata e reengata)
    Missao* vagao_anterior = atual->anterior;

    // Conecta o novo vagão aos vizinhos
    vagao_anterior->proximo = nova_missao;
    nova_missao->anterior = vagao_anterior;

    nova_missao->proximo = atual;
    atual->anterior = nova_missao;
}

/* ==========================================================================
 * IMPLEMENTAÇÃO DOS REQUISITOS DE NEGÓCIO
 * ========================================================================== */

/*
 * RF03, RF04, RF16: Registro de Missões ou Ocorrências.
 * Aloca o nó na heap e direciona para a inserção ordenada.
 */
void registrar_operacao(int id, int prioridade_base, TipoOperacao tipo) {
    // Alocação dinâmica do vagão
    Missao* nova_missao = (Missao*)malloc(sizeof(Missao));
    assert(nova_missao != NULL && "Falha catastrófica: Sem memória HEAP disponível.");

    nova_missao->id = id;
    nova_missao->tipo = tipo;
    nova_missao->status = PENDENTE;
    nova_missao->tempo_espera = 0;
    nova_missao->id_drone_alocado = -1;
    nova_missao->id_operador_alocado = -1;
    nova_missao->anterior = NULL;
    nova_missao->proximo = NULL;

    // Regra de Negócio RN03: Ocorrências recebem prioridade máxima absoluta
    if (tipo == OCORRENCIA_EMERGENCIAL) {
        nova_missao->prioridade = 9999; // Prioridade bypass
    } else {
        nova_missao->prioridade = prioridade_base;
    }

    // Insere os engates no lugar certo
    inserir_missao_ordenada(nova_missao);

    printf("[SUCESSO] %s [ID:%d] registrada com prioridade %d e acoplada na fila.\n",
            tipo == OCORRENCIA_EMERGENCIAL ? "Ocorrencia" : "Missao", id, nova_missao->prioridade);
}

/*
 * RF18: Algoritmo de Envelhecimento Computacional (Anti-Starvation).
 * Deve ser chamado ciclicamente pelo sistema (ex: a cada iteração do menu principal).
 */
void envelhecer_missoes() {
    Missao* atual = head_fila;
    int missoes_reordenadas = 0;

    // Precisamos de um ponteiro 'salva_proximo' porque se o vagão atual
    // mudar de posição na fila, perderemos o rastro do loop original.
    while (atual != NULL) {
        Missao* salva_proximo = atual->proximo;

        // Apenas missões PENDENTES sofrem envelhecimento
        if (atual->status == PENDENTE && atual->tipo == MISSAO_PLANEJADA) {
            atual->tempo_espera++;

            // Se o vagão está parado há 3 ciclos, aumentamos sua prioridade
            if (atual->tempo_espera >= 3) {
                atual->prioridade += 1;
                atual->tempo_espera = 0; // Reseta o relógio de espera

                // Desengata o vagão atual e reengata na nova posição correta
                remover_no_da_fila(atual);
                inserir_missao_ordenada(atual);

                missoes_reordenadas++;
            }
        }
        atual = salva_proximo;
    }

    if (missoes_reordenadas > 0) {
        printf("[SISTEMA] Algoritmo Anti-Starvation executado: %d missoes reordenadas.\n", missoes_reordenadas);
    }
}

/* ==========================================================================
 * INTERFACES EXPORTADAS (Integração com Execução e Relatórios)
 * ========================================================================== */

Missao* buscar_missao_por_id(int id_missao) {
    Missao* atual = head_fila;
    while (atual != NULL) {
        if (atual->id == id_missao) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL; // Retorna nulo se não encontrar na fila férrea
}

const Missao* obter_head_fila_missoes() {
    return head_fila; // Usado estritamente para leitura de relatórios
}
