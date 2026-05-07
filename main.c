#include "orion_campus.h"

/* ==========================================================================
 * INTERFACES DE CONTRATO (Importação dos Módulos)
 * ========================================================================== */
extern void cadastrar_drone(int id, const char* nome);
extern void cadastrar_operador(int id, const char* nome);
extern void visualizar_recursos_disponiveis();
extern void registrar_operacao(int id, int prioridade_base, TipoOperacao tipo);
extern void envelhecer_missoes();
extern void associar_recursos_missao(int id_missao, int id_drone, int id_operador);
extern void atualizar_status_missao(int id_missao, StatusMissao novo_status);
extern void consultar_dados_por_id(int id_alvo);
extern void listar_pendencias_operacionais();
extern void gerar_relatorio_geral();

/* ==========================================================================
 * UTILITÁRIOS DA INTERFACE (RHIC01)
 * ========================================================================== */
/*
 * Drena o buffer de entrada do teclado (stdin) até encontrar uma quebra de linha.
 * Evita loops infinitos e leitura de lixo de memória caso o scanf falhe na tipagem.
 */
void limpar_buffer_teclado() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Apenas consome os caracteres residuais
    }
}

/* ==========================================================================
 * THREAD PRINCIPAL (Loop Operacional)
 * ========================================================================== */
int main() {
    int opcao_principal = -1;
    char buffer_nome[MAX_NOME];
    int id, id_aux1, id_aux2, prioridade;

    printf("\n=======================================================\n");
    printf("          SISTEMA OPERACIONAL ORION CAMPUS             \n");
    printf("     Controle de Segurança, Inspeção e Resposta        \n");
    printf("=======================================================\n");

    while (opcao_principal != 0) {
        // RF18: Executa o algoritmo de envelhecimento a cada ciclo para evitar starvation
        envelhecer_missoes();

        printf("\n--- MENU PRINCIPAL ---\n");
        printf("1. Gerenciar Recursos - Cadastrar Drones e Operadores\n");
        printf("2. Central de Missoes e Ocorrencias - Inclusao e exclusao\n");
        printf("3. Controle de Execucao de missao/ocorrencia (Em execucao/Concluir)\n");
        printf("4. Auditoria e Relatorios\n");
        printf("0. Encerrar Sistema\n");
        printf("Escolha: ");

        if (scanf("%d", &opcao_principal) != 1) {
            limpar_buffer_teclado();
            printf("\n[ERRO] Entrada invalida. Insira um numero.\n");
            continue;
        }
        limpar_buffer_teclado();

        switch (opcao_principal) {
            case 1: // Módulo Agente 2
                printf("\n[RECURSOS] 1.Cadastrar Drone | 2.Cadastrar Operador | 3.Ver Livres: ");
                if (scanf("%d", &id_aux1) != 1) { limpar_buffer_teclado(); break; }
                limpar_buffer_teclado();

                if (id_aux1 == 1 || id_aux1 == 2) {
                    printf("Informe o ID Unico: ");
                    if (scanf("%d", &id) != 1) { limpar_buffer_teclado(); break; }
                    limpar_buffer_teclado();

                    printf("Informe o Nome: ");
                    fgets(buffer_nome, MAX_NOME, stdin);
                    buffer_nome[strcspn(buffer_nome, "\n")] = 0; // Remove o '\n' lido pelo fgets

                    if (id_aux1 == 1) cadastrar_drone(id, buffer_nome);
                    else cadastrar_operador(id, buffer_nome);
                } else if (id_aux1 == 3) {
                    visualizar_recursos_disponiveis();
                }
                break;

            case 2: // Módulo Agente 3
                printf("\n[MISSOES] 1.Registrar Missao | 2.Registrar Ocorrencia: ");
                if (scanf("%d", &id_aux1) != 1) { limpar_buffer_teclado(); break; }
                limpar_buffer_teclado();

                printf("Informe o ID da Operacao: ");
                if (scanf("%d", &id) != 1) { limpar_buffer_teclado(); break; }
                limpar_buffer_teclado();

                if (id_aux1 == 1) {
                    printf("Informe a Prioridade Base (Numerica): ");
                    if (scanf("%d", &prioridade) != 1) { limpar_buffer_teclado(); break; }
                    limpar_buffer_teclado();
                    registrar_operacao(id, prioridade, MISSAO_PLANEJADA);
                } else if (id_aux1 == 2) {
                    registrar_operacao(id, 9999, OCORRENCIA_EMERGENCIAL); // Ocorrência tem prioridade by-pass (RN03)
                }
                break;

            case 3: // Módulo Agente 4
                printf("\n[EXECUCAO] 1.Colocar em execucao - missao e operador | 2.Concluir uma missao em execucao | 3.Cancelar: ");
                if (scanf("%d", &id_aux1) != 1) { limpar_buffer_teclado(); break; }
                limpar_buffer_teclado();

                printf("ID da Missao/Ocorrencia alvo: ");
                if (scanf("%d", &id) != 1) { limpar_buffer_teclado(); break; }
                limpar_buffer_teclado();

                if (id_aux1 == 1) {
                    printf("ID do Drone disponivel: ");
                    if (scanf("%d", &id_aux1) != 1) { limpar_buffer_teclado(); break; }
                    limpar_buffer_teclado();

                    printf("ID do Operador disponivel: ");
                    if (scanf("%d", &id_aux2) != 1) { limpar_buffer_teclado(); break; }
                    limpar_buffer_teclado();

                    associar_recursos_missao(id, id_aux1, id_aux2);
                } else if (id_aux1 == 2) {
                    atualizar_status_missao(id, CONCLUIDA);
                } else if (id_aux1 == 3) {
                    atualizar_status_missao(id, CANCELADA);
                }
                break;

            case 4: // Módulo Agente 5
                printf("\n[RELATORIOS] 1.Consulta por ID | 2.Fila Ativa | 3.Relatorio Geral (historico): ");
                if (scanf("%d", &id_aux1) != 1) { limpar_buffer_teclado(); break; }
                limpar_buffer_teclado();

                if (id_aux1 == 1) {
                    printf("ID para busca completa: ");
                    if (scanf("%d", &id) != 1) { limpar_buffer_teclado(); break; }
                    limpar_buffer_teclado();
                    consultar_dados_por_id(id);
                } else if (id_aux1 == 2) {
                    listar_pendencias_operacionais();
                } else if (id_aux1 == 3) {
                    gerar_relatorio_geral();
                }
                break;

            case 0:
                printf("\nEncerrando operacoes do ORION CAMPUS e liberando processos...\n");
                // Em um cenário real longo, aqui fariamos um loop liberando (free) a fila restante.
                break;

            default:
                printf("\n[ERRO] Comando nao reconhecido pelo sistema.\n");
        }
    }
    return 0;
}
