#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#define MAX_CAMPOS 40
#define MAX_REGISTROS 1000
#define MAX_TAMANHO_CAMPO 30

typedef struct {
    char nome_campo[MAX_TAMANHO_CAMPO];
    char tipo_campo;
} Campo;

typedef struct {
    Campo campos[MAX_CAMPOS];
    char registros[MAX_REGISTROS][MAX_CAMPOS][MAX_TAMANHO_CAMPO];
    int quantidade_campos;
    int quantidade_registros;
} Tabela;

Tabela tabela;

typedef enum {
    TOKEN,
    TOKEN_CREATE,
    TOKEN_TABLE,
    TOKEN_VIRGULA,
    TOKEN_PARENTESE_DIREITO,
    TOKEN_PARENTESE_ESQUERDO,
    TOKEN_INSERT,
    TOKEN_INTO,
    TOKEN_SELECT,
    TOKEN_FROM,
    TOKEN_STRING,
    TOKEN_NUMERO,
    TOKEN_DOIS_PONTOS,
    TOKEN_ASTERISTICO,
    TOKEN_DELETE  // Novo tipo de token para DELETE
} TipoToken;

typedef struct {
    TipoToken tipo;
    char valor[MAX_TAMANHO_CAMPO];
} Token;

Token obter_token(const char** entrada);

void inicializar_tabela() {
    tabela.quantidade_campos = 0;
    tabela.quantidade_registros = 0;
}

// obter token da string de entrada
Token obter_token(const char** entrada) {
    Token token;
    token.tipo = TOKEN;
    token.valor[0] = '\0';

    while (**entrada == ' ' || **entrada == '\n') {
        (*entrada)++;
    }

    if (strncmp(*entrada, "CREATE", 6) == 0) {
        (*entrada) += 6;
        token.tipo = TOKEN_CREATE;
    } else if (strncmp(*entrada, "TABLE", 5) == 0) {
        (*entrada) += 5;
        token.tipo = TOKEN_TABLE;
    } else if (**entrada == ',') {
        (*entrada)++;
        token.tipo = TOKEN_VIRGULA;
    } else if (**entrada == '(') {
        (*entrada)++;
        token.tipo = TOKEN_PARENTESE_DIREITO;
    } else if (**entrada == ')') {
        (*entrada)++;
        token.tipo = TOKEN_PARENTESE_ESQUERDO;
    } else if (**entrada == ';') {
        (*entrada)++;
        token.tipo = TOKEN_DOIS_PONTOS;
    } else if (strncmp(*entrada, "INSERT", 6) == 0) {
        (*entrada) += 6;
        token.tipo = TOKEN_INSERT;
    } else if (strncmp(*entrada, "INTO", 4) == 0) {
        (*entrada) += 4;
        token.tipo = TOKEN_INTO;
    } else if (strncmp(*entrada, "SELECT", 6) == 0) {
        (*entrada) += 6;
        token.tipo = TOKEN_SELECT;
    } else if (strncmp(*entrada, "FROM", 4) == 0) {
        (*entrada) += 4;
        token.tipo = TOKEN_FROM;
    } else if (**entrada == '*') {
        (*entrada)++;
        token.tipo = TOKEN_ASTERISTICO;
    } else if (strncmp(*entrada, "DELETE", 6) == 0) {  // Reconhecendo o comando DELETE
        (*entrada) += 6;
        token.tipo = TOKEN_DELETE;
    } else if (isdigit(**entrada)) {
        int i = 0;
        while (isdigit(**entrada)) {
            token.valor[i++] = *(*entrada)++;
        }
        token.valor[i] = '\0';
        token.tipo = TOKEN_NUMERO;
    } else if (isalpha(**entrada) || **entrada == '_') {
        int i = 0;
        while (isalnum(**entrada) || **entrada == '_') {
            token.valor[i++] = *(*entrada)++;
        }
        token.valor[i] = '\0';
        token.tipo = TOKEN_STRING;
    }

    return token;
}

// criar uma tabela
void criar_tabela(const char** entrada) {
    Token token = obter_token(entrada);
    if (token.tipo != TOKEN_STRING) {
        printf("Erro: Nome de tabela esperado.\n");
        return;
    }

    token = obter_token(entrada);
    if (token.tipo != TOKEN_PARENTESE_DIREITO) {
        printf("Erro: '(' esperado após o nome da tabela.\n");
        return;
    }

    int indice_campo = 0;
    while (indice_campo < MAX_CAMPOS) {
        token = obter_token(entrada);
        if (token.tipo != TOKEN_STRING) break;

        strcpy(tabela.campos[indice_campo].nome_campo, token.valor);
        
        token = obter_token(entrada);
        if (strcmp(token.valor, "C") == 0) {
            tabela.campos[indice_campo].tipo_campo = 'C';
        } else if (strcmp(token.valor, "N") == 0) {
            tabela.campos[indice_campo].tipo_campo = 'N';
        } else {
            printf("Erro: Tipo de campo inválido (C para texto, N para número).\n");
            return;
        }

        indice_campo++;
        tabela.quantidade_campos++;
        token = obter_token(entrada);

        if (token.tipo != TOKEN_VIRGULA && token.tipo != TOKEN_PARENTESE_ESQUERDO) break;
        if (token.tipo == TOKEN_PARENTESE_ESQUERDO) break;
    }
}

// inserir registros na tabela
void inserir_na_tabela(const char** entrada) {
    if (tabela.quantidade_campos == 0) {
        printf("Erro: Nenhuma tabela criada para inserção de dados.\n");
        return;
    }

    Token token = obter_token(entrada);
    if (token.tipo != TOKEN_STRING) {
        printf("Erro: Nome da tabela esperado.\n");
        return;
    }

    token = obter_token(entrada);
    if (token.tipo != TOKEN_PARENTESE_DIREITO) {
        printf("Erro: '(' esperado após o nome da tabela.\n");
        return;
    }

    int indice_registro = tabela.quantidade_registros;
    int indice_campo = 0;
    while (indice_campo < tabela.quantidade_campos && indice_campo < MAX_CAMPOS) {
        token = obter_token(entrada);
        if (token.tipo == TOKEN_STRING || token.tipo == TOKEN_NUMERO) {
            strcpy(tabela.registros[indice_registro][indice_campo], token.valor);
            indice_campo++;
        }

        token = obter_token(entrada);
        if (token.tipo != TOKEN_VIRGULA && token.tipo != TOKEN_PARENTESE_ESQUERDO) break;
        if (token.tipo == TOKEN_PARENTESE_ESQUERDO) break;
    }

    tabela.quantidade_registros++;
    printf("Registro inserido com sucesso! (Registro %d)\n", indice_registro + 1);
}

// exibir a tabela
void exibir_tabela() {
    if (tabela.quantidade_campos == 0) {
        printf("Nenhuma tabela criada para exibir.\n");
        return;
    }

    for (int i = 0; i < tabela.quantidade_campos; i++) {
        printf("%s\t", tabela.campos[i].nome_campo);
    }
    printf("\n");

    for (int i = 0; i < tabela.quantidade_registros; i++) {
        for (int j = 0; j < tabela.quantidade_campos; j++) {
            printf("%s\t", tabela.registros[i][j]);
        }
        printf("\n");
    }
}

// excluir registros da tabela
void excluir_registros() {
    if (tabela.quantidade_campos == 0) {
        printf("Erro: Nenhuma tabela criada para exclusão de dados.\n");
        return;
    }

    tabela.quantidade_registros = 0;
    printf("Todos os registros foram excluídos.\n");
}

void executar_comando(const char* entrada) {
    const char* ptr = entrada;
    Token token = obter_token(&ptr);

    if (token.tipo == TOKEN_CREATE) {
        token = obter_token(&ptr);
        if (token.tipo == TOKEN_TABLE) {
            criar_tabela(&ptr);
            printf("Tabela criada com sucesso!\n");
        }
    } else if (token.tipo == TOKEN_INSERT) {
        token = obter_token(&ptr);
        if (token.tipo == TOKEN_INTO) {
            inserir_na_tabela(&ptr);
        }
    } else if (token.tipo == TOKEN_SELECT) {
        token = obter_token(&ptr);
        if (token.tipo == TOKEN_ASTERISTICO) {
            token = obter_token(&ptr);
            if (token.tipo == TOKEN_FROM) {
                exibir_tabela();
            }
        }
    } else if (token.tipo == TOKEN_DELETE) {  // Comando DELETE
        token = obter_token(&ptr);
        if (token.tipo == TOKEN_FROM) {
            excluir_registros();
        }
    } else {
        printf("Comando não reconhecido.\n");
    }
}

int main() {
    inicializar_tabela();
    char comando[50];

    printf("Digite um comando SQL ou 'sair' para encerrar:\n");
    while (1) {
        printf("> ");
        if (fgets(comando, sizeof(comando), stdin) == NULL) {
            break;
        }
        
        comando[strcspn(comando, "\n")] = '\0';

        if (strcmp(comando, "sair") == 0) {
            break;
        }

        executar_comando(comando);
    }

    printf("Programa encerrado.\n");
    return 0;
}
