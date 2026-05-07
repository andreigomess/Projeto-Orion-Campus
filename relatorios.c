#include "orion_campus.h"

/* ==========================================================================
 * INTERFACES DE CONTRATO (Getters de Leitura Segura)
 * Asseguram que este módulo acesse os dados sem permissão de escrita.
 * ========================================================================== */
extern const Drone* obter_array_drones(int* total);
extern const Operador* obter_array_operadores(int* total);
extern const Missao* obter_head_fila_missoes();
extern const Missao* obter_array_historico(int* total);

/* ==========================================================================
 * FUNÇÕES UTILITÁRIAS DE FORMATAÇÃO VISUAL (RHIC01)
 * ========================================================================== */

/* Converte o enum de Status do Recurso para String literal */
static const char* status_recurso_str(StatusRecurso status) {
    switch(status) {
        case LIVRE: return "LIVRE";
        case EM_MANUTENCAO: return "MANUTENCAO";
        case OCUPADO: return "OCUPADO";
        default: return "DESCONHECIDO";
    }
}

/* Converte o enum de Status da Missão para String literal */
static const char* status_missao_str(StatusMissao status) {
    switch(status) {
        case PENDENTE: return "PENDENTE";
        case EM_EXECUCAO: return "EM EXECUCAO";
        case CONCLUIDA: return "CONCLUIDA";
        case CANCELADA: return "CANCELADA";
        default: return "DESCONHECIDO";
    }
}

/* ==========================================================================
 * IMPLEMENTAÇÃO DOS REQUISITOS DE INSPEÇÃO
 * ========================================================================== */

/*
 * CSU06 / RF06: Consultar Dados por Identificador
 * Varredura sistemática não-mutável de todas as estruturas em busca do ID.
 */
void consultar_dados_por_id(int id_alvo) {
    printf("\n=======================================================\n");
    printf("              RESULTADO DA AUDITORIA (ID: %d)            \n", id_alvo);
    printf("=======================================================\n");

    int total_drones, total_operadores, total_historico;
    const Drone* drones = obter_array_drones(&total_drones);
    const Operador* operadores = obter_array_operadores(&total_operadores);
    const Missao* head_missoes = obter_head_fila_missoes();
    const Missao* historico = obter_array_historico(&total_historico);

    int encontrado = 0;

    // 1. Varredura no vetor de Drones O(N)
    for (int i = 0; i < total_drones; i++) {
        if (drones[i].id == id_alvo) {
            printf("[TIPO] DRONE | Nome: %-15s | Status: %s\n",
                   drones[i].nome, status_recurso_str(drones[i].status));
            encontrado = 1;
        }
    }

    // 2. Varredura no vetor de Operadores O(N)
    for (int i = 0; i < total_operadores; i++) {
        if (operadores[i].id == id_alvo) {
            printf("[TIPO] OPERADOR | Nome: %-15s | Status: %s\n",
                   operadores[i].nome, status_recurso_str(operadores[i].status));
            encontrado = 1;
        }
    }

    // 3. Varredura na fila duplamente encadeada (Operações Ativas) O(N)
    const Missao* atual = head_missoes;
    while (atual != NULL) {
        if (atual->id == id_alvo) {
            printf("[TIPO] %s | Prio: %-4d | Status: %s | Duração: %d ciclos\n",
                   atual->tipo == OCORRENCIA_EMERGENCIAL ? "OCORRENCIA" : "MISSAO",
                   atual->prioridade, status_missao_str(atual->status), atual->tempo_espera);
            encontrado = 1;
        }
        atual = atual->proximo;
    }

    // 4. Varredura no vetor Histórico O(N)
    for (int i = 0; i < total_historico; i++) {
        if (historico[i].id == id_alvo) {
            printf("[TIPO] HISTORICO | Tipo: %d | Finalizada como: %s\n",
                   historico[i].tipo, status_missao_str(historico[i].status));
            encontrado = 1;
        }
    }

    if (!encontrado) {
        printf("Nenhum registro correspondente encontrado no sistema para o identificador %d.\n", id_alvo);
    }
    printf("=======================================================\n");
}

/*
 * CSU09 / RF09 / CSU10 / RF10: Listagem Específica de Missões e Ocorrências
 * Passagem por referência com modificador const assegura segurança de memória.
 */
void listar_pendencias_operacionais() {
    const Missao* atual = obter_head_fila_missoes();

    printf("\n=========================================================================\n");
    printf("                      PAINEL DE PENDENCIAS (FILA ATIVA)                    \n");
    printf("=========================================================================\n");
    printf("%-5s | %-12s | %-5s | %-15s | %-15s\n", "ID", "TIPO", "PRIO", "STATUS", "TEMPO (CICLOS)");
    printf("-------------------------------------------------------------------------\n");

    int contagem = 0;
    while (atual != NULL) {
        // Filtra apenas PENDENTES e EM EXECUÇÃO (Missões pendentes  e Ocorrências abertas [cite: 184])
        if (atual->status == PENDENTE || atual->status == EM_EXECUCAO) {
            printf("%-5d | %-12s | %-5d | %-15s | %-15d\n",
                   atual->id,
                   atual->tipo == OCORRENCIA_EMERGENCIAL ? "OCORRENCIA" : "MISSAO",
                   atual->prioridade,
                   status_missao_str(atual->status),
                   atual->tempo_espera);
            contagem++;
        }
        atual = atual->proximo; // Avança o vagão
    }

    if (contagem == 0) {
        printf("Nenhuma missao ou ocorrencia pendente na fila operacional.\n");
    }
    printf("=========================================================================\n");
}

/*
 * CSU08 / RF08: Geração de Relatórios (Histórico Operacional)
 */
void gerar_relatorio_geral() {
    int total_historico;
    const Missao* historico = obter_array_historico(&total_historico);

    printf("\n=========================================================================\n");
    printf("                        RELATORIO GERAL DE OPERACOES                       \n");
    printf("=========================================================================\n");
    printf("%-5s | %-15s | %-15s | %-10s | %-10s\n", "ID", "TIPO", "STATUS FINAL", "ID DRONE", "ID OPER.");
    printf("-------------------------------------------------------------------------\n");

    for (int i = 0; i < total_historico; i++) {
        printf("%-5d | %-15s | %-15s | %-10d | %-10d\n",
               historico[i].id,
               historico[i].tipo == OCORRENCIA_EMERGENCIAL ? "OCORRENCIA" : "MISSAO",
               status_missao_str(historico[i].status),
               historico[i].id_drone_alocado,
               historico[i].id_operador_alocado);
    }

    if (total_historico == 0) {
        printf("Nenhuma operacao foi finalizada ou arquivada ate o momento.\n");
    }
    printf("=========================================================================\n");
}
