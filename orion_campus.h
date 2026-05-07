#ifndef ORION_CAMPUS_H_INCLUDED
#define ORION_CAMPUS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ==========================================================================
 * DIRETIVAS DE PRÉ-COMPILADOR E CONSTANTES (Gerenciamento de Memória)
 * ========================================================================== */
#define MAX_DRONES 50
#define MAX_OPERADORES 50
#define MAX_NOME 64          // Múltiplo de 8 para alinhamento eficiente em arquiteturas 64-bits
#define BUFFER_SIZE 128      // Tamanho do buffer para entrada segura de dados via teclado

/* ==========================================================================
 * ENUMERAÇÕES (Máquinas de Estado)
 * ========================================================================== */

// Define o estado das entidades físicas (Drones e Operadores)
typedef enum {
    LIVRE,
    EM_MANUTENCAO,
    OCUPADO
} StatusRecurso;

// Define o ciclo de vida das operações
typedef enum {
    PENDENTE,
    EM_EXECUCAO,
    CONCLUIDA,
    CANCELADA
} StatusMissao;

// Diferencia as operações na fila (Atende ao RF16)
typedef enum {
    MISSAO_PLANEJADA,
    OCORRENCIA_EMERGENCIAL
} TipoOperacao;

/* ==========================================================================
 * ESTRUTURAS DE DADOS (Entidades)
 * ========================================================================== */

// Entidade: Drone (Armazenada em lista vetorial estática)
typedef struct {
    int id;
    char nome[MAX_NOME];
    StatusRecurso status;
} Drone;

// Entidade: Operador (Armazenada em lista vetorial estática)
typedef struct {
    int id;
    char nome[MAX_NOME];
    StatusRecurso status;
} Operador;

// Entidade Dinâmica: Missão / Ocorrência
// Funciona como os vagões de um trem na nossa lista duplamente encadeada.
typedef struct Missao {
    int id;
    TipoOperacao tipo;          // Classifica se é Missão (baixa prio) ou Ocorrência (alta prio)
    int prioridade;             // Define a ordem na fila (Ocorrências terão valor máximo)
    int tempo_espera;           // Contador para o algoritmo de envelhecimento (RF18)
    StatusMissao status;

    // Relações (Foreign Keys para os vetores)
    int id_drone_alocado;       // -1 se não alocado
    int id_operador_alocado;    // -1 se não alocado

    // Ponteiros de Engate (Lista Duplamente Encadeada)
    // Permitem percorrer a composição tanto da locomotiva pro fim, quanto de ré.
    struct Missao* anterior;
    struct Missao* proximo;
} Missao;

/* ==========================================================================
 * PROTÓTIPOS DAS FUNÇÕES DA FUNDAÇÃO (Utilitários)
 * ========================================================================== */

// Função vital para interface homem-computador limpa (RHIC01)
// Previne o "loop infinito" do scanf quando o usuário digita letras ao invés de números.
void limpar_buffer_teclado();

// Protótipo do Menu Principal
void exibir_menu_principal();


#endif // ORION_CAMPUS_H_INCLUDED
