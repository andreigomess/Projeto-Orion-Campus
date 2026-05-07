#include "orion_campus.h"
#include <assert.h>

/* ==========================================================================
 * INTERFACES DE CONTRATO (Assumidas dos Agentes 2 e 3)
 * ========================================================================== */
extern Missao* buscar_missao_por_id(int id_missao);
extern void remover_no_da_fila(Missao* alvo);
extern StatusRecurso verificar_status_drone(int id_drone);
extern StatusRecurso verificar_status_operador(int id_operador);
extern void alterar_status_drone(int id_drone, StatusRecurso novo_status);
extern void alterar_status_operador(int id_operador, StatusRecurso novo_status);

/* ==========================================================================
 * ESTADO INTERNO DO HISTÓRICO (RF07, RN10)
 * ========================================================================== */
#define MAX_HISTORICO 1000

// Vetor estático que representa o repositório imutável de missões finalizadas.
static Missao array_historico[MAX_HISTORICO];
static int total_historico = 0;

/* ==========================================================================
 * FUNÇÕES DE TRANSIÇÃO E CICLO DE VIDA
 * ========================================================================== */

/*
 * Helper de arquivamento. Copia o dado estrutural (passagem por valor) para o
 * vetor histórico e limpa referências dinâmicas.
 */
static void arquivar_missao_historico(Missao* missao_finalizada) {
    if (total_historico >= MAX_HISTORICO) {
        printf("[ERRO CRITICO] Overflow no banco de historico operacional.\n");
        return;
    }

    // Deep Copy: Copia todos os bytes da struct, transferindo os dados da HEAP para a BSS
    array_historico[total_historico] = *missao_finalizada;

    // Anulação de engates: Garante que o histórico jamais aponte para lixo de memória
    array_historico[total_historico].anterior = NULL;
    array_historico[total_historico].proximo = NULL;

    total_historico++;
}

/*
 * Helper para liberar os recursos físicos alocados em uma missão.
 */
static void liberar_recursos(int id_drone, int id_operador) {
    if (id_drone != -1) alterar_status_drone(id_drone, LIVRE);
    if (id_operador != -1) alterar_status_operador(id_operador, LIVRE);
}

/*
 * CSU13 / RF15: Associar Recursos à Missão (Aplica RN01, RN02, RN07, RN08)
 */
void associar_recursos_missao(int id_missao, int id_drone, int id_operador) {
    Missao* missao = buscar_missao_por_id(id_missao);

    if (missao == NULL) {
        printf("[ERRO] Missao ID:%d nao encontrada na fila.\n", id_missao);
        return;
    }

    // Validação da Máquina de Estado
    if (missao->status != PENDENTE) {
        printf("[ERRO] Apenas missoes PENDENTES podem receber recursos.\n");
        return;
    }

    // Validação de Disponibilidade e Mutuamente Exclusiva (Locks de Recurso)
    if (verificar_status_drone(id_drone) != LIVRE) {
        printf("[ERRO] Drone ID:%d nao esta disponivel.\n", id_drone);
        return;
    }
    if (verificar_status_operador(id_operador) != LIVRE) {
        printf("[ERRO] Operador ID:%d nao esta disponivel.\n", id_operador);
        return;
    }

    // Acoplamento: Bloqueia recursos no vetor estático (Agente 2)
    alterar_status_drone(id_drone, OCUPADO);
    alterar_status_operador(id_operador, OCUPADO);

    // Registra os Foreign Keys no nó dinâmico (Agente 3)
    missao->id_drone_alocado = id_drone;
    missao->id_operador_alocado = id_operador;

    // Transição de Estado
    missao->status = EM_EXECUCAO;

    printf("[SUCESSO] Recursos associados. Missao ID:%d entrou EM_EXECUCAO.\n", id_missao);
}

/*
 * CSU05 / RF13 / RN05: Atualizar Status da Missão
 * Gerencia a transição para CONCLUIDA ou CANCELADA, efetuando o descarte do nó.
 */
void atualizar_status_missao(int id_missao, StatusMissao novo_status) {
    Missao* missao = buscar_missao_por_id(id_missao);

    if (missao == NULL) {
        printf("[ERRO] Missao ID:%d nao encontrada.\n", id_missao);
        return;
    }

    // Validação de transição: Conclusão (RN09)
    if (novo_status == CONCLUIDA && missao->status != EM_EXECUCAO) {
        printf("[ERRO] Uma missao so pode ser CONCLUIDA se estiver EM_EXECUCAO.\n");
        return;
    }

    // Validação de transição: Cancelamento (RN12 / CSU11)
    if (novo_status == CANCELADA && (missao->status == CONCLUIDA || missao->status == CANCELADA)) {
        printf("[ERRO] Nao e possivel cancelar uma missao ja encerrada.\n");
        return;
    }

    // Efetiva a transição terminal
    missao->status = novo_status;

    // Libera os recursos físicos de volta para a matriz do Agente 2
    liberar_recursos(missao->id_drone_alocado, missao->id_operador_alocado);

    // Move os dados estruturais para o repositório imutável
    arquivar_missao_historico(missao);

    // O Operador de guindaste entra em ação: removemos o vagão da fila férrea
    remover_no_da_fila(missao);

    // Desaloca a memória dinâmica original para prevenir memory leak
    free(missao);

    printf("[SUCESSO] Missao ID:%d foi finalizada (Status: %d), movida ao historico e recursos liberados.\n",
            id_missao, novo_status);
}

/* ==========================================================================
 * INTERFACES EXPORTADAS (Para módulo de Relatórios)
 * ========================================================================== */

const Missao* obter_array_historico(int* total) {
    *total = total_historico;
    return array_historico;
}
