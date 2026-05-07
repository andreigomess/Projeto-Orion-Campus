#include "orion_campus.h"

/* ==========================================================================
 * ESTADO INTERNO DO MÓDULO (Encapsulamento Estrito)
 * ========================================================================== */
// Arrays estáticos garantem blocos de memória contíguos na área de dados (.bss)
static Drone array_drones[MAX_DRONES];
static int total_drones = 0;

static Operador array_operadores[MAX_OPERADORES];
static int total_operadores = 0;

/* ==========================================================================
 * FUNÇÕES AUXILIARES (Lógica "Under the Hood")
 * ========================================================================== */

/*
 * Busca sequencial no array de Drones.
 * Retorna o endereço de offset (índice) no array ou -1 se não encontrado.
 */
static int buscar_indice_drone(int id) {
    for (int i = 0; i < total_drones; i++) {
        if (array_drones[i].id == id) {
            return i;
        }
    }
    return -1;
}

/*
 * Busca sequencial no array de Operadores.
 */
static int buscar_indice_operador(int id) {
    for (int i = 0; i < total_operadores; i++) {
        if (array_operadores[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* ==========================================================================
 * IMPLEMENTAÇÃO DOS REQUISITOS DE NEGÓCIO
 * ========================================================================== */

/*
 * RF01 / RN11: Cadastro de Drone com identificador único.
 * Recebe os dados já lidos pela interface, garantindo input único no terminal.
 */
void cadastrar_drone(int id, const char* nome) {
    // Bounds checking: Impede Buffer Overflow no array vetorial
    if (total_drones >= MAX_DRONES) {
        printf("[ERRO FATAL] Limite maximo de drones (%d) atingido na memoria.\n", MAX_DRONES);
        return;
    }

    // Validação estrita de unicidade (Regra de Negócio RN11)
    if (buscar_indice_drone(id) != -1) {
        printf("[ERRO] O Identificador ID:%d ja esta em uso. Operacao rejeitada.\n", id);
        return;
    }

    // Alocação estruturada no array
    array_drones[total_drones].id = id;

    // strncpy garante que não copiaremos mais bytes do que o buffer suporta
    strncpy(array_drones[total_drones].nome, nome, MAX_NOME - 1);
    array_drones[total_drones].nome[MAX_NOME - 1] = '\0'; // Terminador nulo garantido de segurança

    array_drones[total_drones].status = LIVRE;

    total_drones++;
    printf("[SUCESSO] Drone '%s' [ID:%d] registrado e pronto para operacao.\n", nome, id);
}

/*
 * RF02 / RN11: Cadastro de Operador com identificador único.
 */
void cadastrar_operador(int id, const char* nome) {
    if (total_operadores >= MAX_OPERADORES) {
        printf("[ERRO FATAL] Limite maximo de operadores (%d) atingido na memoria.\n", MAX_OPERADORES);
        return;
    }

    if (buscar_indice_operador(id) != -1) {
        printf("[ERRO] O Identificador ID:%d ja esta atribuido a outro operador.\n", id);
        return;
    }

    array_operadores[total_operadores].id = id;
    strncpy(array_operadores[total_operadores].nome, nome, MAX_NOME - 1);
    array_operadores[total_operadores].nome[MAX_NOME - 1] = '\0';
    array_operadores[total_operadores].status = LIVRE;

    total_operadores++;
    printf("[SUCESSO] Operador '%s' [ID:%d] registrado no sistema.\n", nome, id);
}

/*
 * CSU07 / RF11: Visualização de Recursos Disponíveis
 * Itera sobre as listas vetoriais filtrando pelo estado LIVRE.
 */
void visualizar_recursos_disponiveis() {
    printf("\n=======================================================\n");
    printf("              RECURSOS OPERACIONAIS LIVRES               \n");
    printf("=======================================================\n");

    int drones_livres = 0;
    printf("\n--- DRONES DISPONIVEIS ---\n");
    for (int i = 0; i < total_drones; i++) {
        if (array_drones[i].status == LIVRE) {
            printf(" -> [ID: %d] %s\n", array_drones[i].id, array_drones[i].nome);
            drones_livres++;
        }
    }
    if (drones_livres == 0) {
        printf(" Nenhum drone disponivel no momento.\n");
    }

    int operadores_livres = 0;
    printf("\n--- OPERADORES ATIVOS E LIVRES ---\n");
    for (int i = 0; i < total_operadores; i++) {
        if (array_operadores[i].status == LIVRE) {
            printf(" -> [ID: %d] %s\n", array_operadores[i].id, array_operadores[i].nome);
            operadores_livres++;
        }
    }
    if (operadores_livres == 0) {
        printf(" Nenhum operador disponivel no momento.\n");
    }
    printf("=======================================================\n");
}

/* ==========================================================================
 * INTERFACES EXPORTADAS (Getters e Setters para Integração)
 * ========================================================================== */

StatusRecurso verificar_status_drone(int id_drone) {
    int idx = buscar_indice_drone(id_drone);
    if (idx != -1) return array_drones[idx].status;
    return OCUPADO; // Fail-safe: se não achar, assume indisponível
}

StatusRecurso verificar_status_operador(int id_operador) {
    int idx = buscar_indice_operador(id_operador);
    if (idx != -1) return array_operadores[idx].status;
    return OCUPADO;
}

void alterar_status_drone(int id_drone, StatusRecurso novo_status) {
    int idx = buscar_indice_drone(id_drone);
    if (idx != -1) array_drones[idx].status = novo_status;
}

void alterar_status_operador(int id_operador, StatusRecurso novo_status) {
    int idx = buscar_indice_operador(id_operador);
    if (idx != -1) array_operadores[idx].status = novo_status;
}

// Retorna o ponteiro const garantindo leitura não-destrutiva para os relatórios
const Drone* obter_array_drones(int* total) {
    *total = total_drones;
    return array_drones;
}

const Operador* obter_array_operadores(int* total) {
    *total = total_operadores;
    return array_operadores;
}
