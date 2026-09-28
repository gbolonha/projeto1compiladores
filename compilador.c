// Gabriel Teixeira Bolonha (10426937)
// Geovana Bomfim Rodrigues (10410514)

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Para compilar:
// gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador
// Para executar:
// ./compilador arquivo.txt

typedef enum {
    // palavras reservadas
    ALGORITMO, CARACTERE, DIV, E, ENQUANTO, ENTAO, ESCREVA, FACA, FALSO,
    FIM, FUNCAO, INICIO, INTEIRO, LEIA, LOGICO, MOD, OU,
    PROCEDIMENTO, SE, SENAO, VAR, VERDADEIRO,

    // pontuação / delimitadores
    PONTO_VIRGULA,   // ;
    PONTO,           // .
    VIRGULA,         // ,
    DOIS_PONTOS,     // :
    ABRE_PAR,        // (
    FECHA_PAR,       // )

    // operadores
    ATRIBUICAO,      // :=
    MENOR,           // <
    MENOR_IGUAL,     // <=
    MAIOR,           // >
    MAIOR_IGUAL,     // >=
    IGUAL,           // =
    DIFERENTE,       // <>
    MAIS,            // +
    MENOS,           // -
    MULT,            // *

    // categorias especiais
    IDENTIFICADOR,
    CONSTINT,
    CONSTCHAR,
    COMENTARIO,
    NAO,  

    // controle
    FIM_ARQUIVO,
    ERRO
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    union {
        int numero;  // atributo do átomo constint (constante inteira)
        char id[16]; // atributo identificador
        char ch;     // atributo do átomo constchar (constante caractere)
    } atributo;
} TInfoAtomo;

char *strAtomo[] = {
    "algoritmo",       // 0
    "caractere",       // 1
    "div",             // 2
    "e",               // 3
    "enquanto",        // 4
    "entao",           // 5
    "escreva",         // 6
    "faca",            // 7
    "falso",           // 8
    "fim",             // 9
    "funcao",          // 10
    "inicio",          // 11
    "inteiro",         // 12
    "leia",            // 13
    "logico",          // 14
    "mod",             // 15
    "ou",              // 16
    "procedimento",    // 17
    "se",              // 18
    "senao",           // 19
    "var",             // 20
    "verdadeiro",      // 21
    "ponto_virgula",   // 22
    "ponto",           // 23
    "virgula",         // 24
    "dois_pontos",     // 25
    "abre_par",        // 26
    "fecha_par",       // 27
    "atribuicao",      // 28
    "menor",           // 29
    "menor_igual",     // 30
    "maior",           // 31
    "maior_igual",     // 32
    "igual",           // 33
    "diferente",       // 34
    "mais",            // 35
    "menos",           // 36
    "mult",            // 37
    "identificador",   // 38
    "constint",        // 39
    "constchar",       // 40
    "comentario",      // 41
    "nao",             // 42
    "fim_arquivo",     // 43
    "erro"             // 44
};

/* tabela de simbolos literais para a mensagem de erro sintatico */
char *strSimbolo[] = {
    "algoritmo", "caractere", "div", "e", "enquanto", "entao", "escreva",
    "faca", "falso", "fim", "funcao", "inicio", "inteiro", "leia", "logico",
    "mod", "ou", "procedimento", "se", "senao", "var", "verdadeiro",
    ";", ".", ",", ":", "(", ")",
    ":=", "<", "<=", ">", ">=", "=", "<>", "+", "-", "*",
    "identificador", "constint", "constchar", "comentario", "nao",
    "fim_arquivo", "erro"
};

void reconhece_id(TInfoAtomo *info_atomo);
void reconhece_int(TInfoAtomo *info_atomo);
void reconhece_char(TInfoAtomo *info_atomo);
void reconhece_comentario(TInfoAtomo *info_atomo);
TAtomo verifica_palavra_reservada(char *lexema);
void imprime_atomo(TInfoAtomo t);
void erro_lexico(int linha, const char *msg);

TInfoAtomo obter_atomo(void);
void consome(TAtomo atomo);

void programa(void);
void bloco(void);
void declaracao_variaveis(void);
void lista_variaveis(void);
void tipo(void);
void declaracao_de_rotinas(void);
void declaracao_de_funcao(void);
void declaracao_de_procedimento(void);
void parametros_formais(void);
void parametro_formal(void);
void comando_composto(void);
void comando(void);
void comando_atribuicao(void);
void chamada_procedimento(void);
void comando_entrada(void);
void comando_saida(void);
void comando_condicional(void);
void comando_repeticao(void);
void lista_expressao(void);
void expressao(void);
void operador_relacional(void);
void expressao_simples(void);
void operador_adicao(void);
void termo(void);
void operador_multiplicacao(void);
void fator(void);

char *buffer;
char *inicio_buffer; // guarda o endereco original do malloc para o free()
FILE *arquivo_global;
char lexema[100];
int contaLinha = 1;
TInfoAtomo info_atomo;
TAtomo lookahead;

void libera_buffer(void)
{
    free(inicio_buffer);

    if (arquivo_global != NULL)
        fclose(arquivo_global);
}

int main(int argc, char *argv[])
{
    FILE *arquivo;
    long tamanho;

    if (argc < 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    arquivo = fopen(argv[1], "r");
    

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }
    arquivo_global = arquivo;

    fseek(arquivo, 0, SEEK_END);
    tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);

    buffer = malloc(tamanho + 1);
    inicio_buffer = buffer; 
    atexit(libera_buffer); 
    fread(buffer, 1, tamanho, arquivo);
    buffer[tamanho] = '\0';

    info_atomo = obter_atomo();
    lookahead = info_atomo.atomo;

    while (lookahead == COMENTARIO)
    {
        imprime_atomo(info_atomo);
        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
    }

    programa();

    printf("%d linhas analisadas, programa sintaticamente correto\n", contaLinha);

   
    arquivo = NULL;

    free(inicio_buffer);
    inicio_buffer = NULL;

    return 0; 
}

void imprime_atomo(TInfoAtomo t)
{
    if (t.atomo == FIM_ARQUIVO)
    {
        return; // fim_arquivo nunca aparece na listagem
    }

    switch (t.atomo)
    {
        case IDENTIFICADOR:
            printf("# %d:identificador: %s\n", t.linha, t.atributo.id);
            break;
        case CONSTINT:
            printf("# %d:constint: %d\n", t.linha, t.atributo.numero);
            break;
        case CONSTCHAR:
            printf("# %d:constchar: %c\n", t.linha, t.atributo.ch);
            break;
        default:
            printf("# %d:%s\n", t.linha, strAtomo[t.atomo]);
    }
}

void erro_lexico(int linha, const char *msg)
{
    printf("# %d:erro lexico, %s\n", linha, msg);
    exit(1);
}

// Analise lexica
TInfoAtomo obter_atomo(void)
{
    TInfoAtomo info_atomo;

    info_atomo.atomo = ERRO;

    while (*buffer == ' ' ||
           *buffer == '\t' ||
           *buffer == '\r' ||
           *buffer == '\n')
    {
        if (*buffer == '\n' && *(buffer + 1) != '\0')
        {
            contaLinha++;
        }

        buffer++;
    }

    info_atomo.linha = contaLinha;

    if (*buffer == 0)
    {
        info_atomo.atomo = FIM_ARQUIVO;
    }
    else if (isalpha(*buffer))
    {
        reconhece_id(&info_atomo);
    }
    else if (isdigit(*buffer))
    {
        reconhece_int(&info_atomo);
    }
    else if (*buffer == '\'')
    {
        reconhece_char(&info_atomo);
    }
    else if (*buffer == '{')
    {
        reconhece_comentario(&info_atomo);
    }
    else if (*buffer == '*')
    {
        info_atomo.atomo = MULT;
        buffer++;
    }
    else if (*buffer == '+')
    {
        info_atomo.atomo = MAIS;
        buffer++;
    }
    else if (*buffer == '-')
    {
        info_atomo.atomo = MENOS;
        buffer++;
    }
    else if (*buffer == ';')
    {
        info_atomo.atomo = PONTO_VIRGULA;
        buffer++;
    }
    else if (*buffer == '.')
    {
        info_atomo.atomo = PONTO;
        buffer++;
    }
    else if (*buffer == ',')
    {
        info_atomo.atomo = VIRGULA;
        buffer++;
    }
    else if (*buffer == ':')
    {
        buffer++;

        if (*buffer == '=')
        {
            info_atomo.atomo = ATRIBUICAO;
            buffer++;
        }
        else
        {
            info_atomo.atomo = DOIS_PONTOS;
        }
    }
    else if (*buffer == '(')
    {
        info_atomo.atomo = ABRE_PAR;
        buffer++;
    }
    else if (*buffer == ')')
    {
        info_atomo.atomo = FECHA_PAR;
        buffer++;
    }
    else if (*buffer == '<')
    {
        buffer++;

        if (*buffer == '=')
        {
            info_atomo.atomo = MENOR_IGUAL;
            buffer++;
        }
        else if (*buffer == '>')
        {
            info_atomo.atomo = DIFERENTE;
            buffer++;
        }
        else
        {
            info_atomo.atomo = MENOR;
        }
    }
    else if (*buffer == '>')
    {
        buffer++;

        if (*buffer == '=')
        {
            info_atomo.atomo = MAIOR_IGUAL;
            buffer++;
        }
        else
        {
            info_atomo.atomo = MAIOR;
        }
    }
    else if (*buffer == '=')
    {
        info_atomo.atomo = IGUAL;
        buffer++;
    }

    return info_atomo;
}

// Usada para verificar se o lexema faz parte das palavras reservadas
TAtomo verifica_palavra_reservada(char *lexema)
{
    char copia[100];
    int i;

    strcpy(copia, lexema);
    for (i = 0; copia[i] != '\0'; i++)
    {
        copia[i] = tolower(copia[i]);
    }

    if (strcmp(copia, "algoritmo") == 0) return ALGORITMO;
    if (strcmp(copia, "caractere") == 0) return CARACTERE;
    if (strcmp(copia, "div") == 0) return DIV;
    if (strcmp(copia, "e") == 0) return E;
    if (strcmp(copia, "enquanto") == 0) return ENQUANTO;
    if (strcmp(copia, "entao") == 0) return ENTAO;
    if (strcmp(copia, "escreva") == 0) return ESCREVA;
    if (strcmp(copia, "faca") == 0) return FACA;
    if (strcmp(copia, "falso") == 0) return FALSO;
    if (strcmp(copia, "fim") == 0) return FIM;
    if (strcmp(copia, "funcao") == 0) return FUNCAO;
    if (strcmp(copia, "inicio") == 0) return INICIO;
    if (strcmp(copia, "inteiro") == 0) return INTEIRO;
    if (strcmp(copia, "leia") == 0) return LEIA;
    if (strcmp(copia, "logico") == 0) return LOGICO;
    if (strcmp(copia, "mod") == 0) return MOD;
    if (strcmp(copia, "ou") == 0) return OU;
    if (strcmp(copia, "procedimento") == 0) return PROCEDIMENTO;
    if (strcmp(copia, "se") == 0) return SE;
    if (strcmp(copia, "senao") == 0) return SENAO;
    if (strcmp(copia, "var") == 0) return VAR;
    if (strcmp(copia, "verdadeiro") == 0) return VERDADEIRO;
    if (strcmp(copia, "nao") == 0) return NAO;

    return IDENTIFICADOR;
}

void reconhece_id(TInfoAtomo *info_atomo)
{
    char *ini_lexema = buffer;
    info_atomo->atomo = ERRO;

    if (isalpha(*buffer))
    {
        buffer++;
        goto q1;
    }

    if (isdigit(*buffer) || *buffer == '_')
    {
        buffer++;
        goto q2;
    }

q1:
    if (isdigit(*buffer) || isalpha(*buffer) || *buffer == '_')
    {
        buffer++;
        goto q1;
    }
    
    int tamanho = (int)(buffer - ini_lexema);

    if (tamanho <= 15)
    {
        strncpy(info_atomo->atributo.id, ini_lexema, buffer - ini_lexema);
        info_atomo->atributo.id[buffer - ini_lexema] = '\0';
        info_atomo->atomo = verifica_palavra_reservada(info_atomo->atributo.id);
        return;
    }
    else 
    {
        erro_lexico(info_atomo->linha, "identificador com mais de 15 caracteres");
    }

q2:
    erro_lexico(contaLinha, "identificador nao pode comecar com digito ou '_'");
}

// constint → digito+((E(+|ε)digito+)|ε)
void reconhece_int(TInfoAtomo *info_atomo)
{
    char *ini_lexema = buffer;
    char *pos_antes_E = NULL;
    info_atomo->atomo = ERRO;

    if (isdigit(*buffer))
    {
        buffer++;
        goto q1;
    }

    goto q5;

q1:
    if (isdigit(*buffer))
    {
        buffer++;
        goto q1;
    }

    if (*buffer == 'E')
    {
        pos_antes_E = buffer;
        buffer++;
        goto q2;
    }

    goto final;

q2:
    if (*buffer == '+')
    {
        buffer++;
        goto q4;
    }

    if (isdigit(*buffer))
    {
        buffer++;
        goto q3;
    }

    buffer = pos_antes_E;
    goto final;

q3:
    if (isdigit(*buffer))
    {
        buffer++;
        goto q3;
    }

    goto final;

q4:
    if (isdigit(*buffer))
    {
        buffer++;
        goto q3;
    }

    buffer = pos_antes_E;
    goto final;

final:
    strncpy(lexema, ini_lexema, buffer - ini_lexema);
    lexema[buffer - ini_lexema] = '\0';

    info_atomo->atomo = CONSTINT;

    char *e = strchr(lexema, 'E');

    if (e == NULL)
    {
        info_atomo->atributo.numero = atoi(lexema);
    }
    else
    {
        int base = atoi(lexema);
        int expoente = atoi(e + 1);
        int resultado = base;

        for (int i = 0; i < expoente; i++)
        {
            resultado *= 10;
        }

        info_atomo->atributo.numero = resultado;
    }

    return;

q5:
    erro_lexico(contaLinha, "constante inteira mal formada");
}

void reconhece_char(TInfoAtomo *info_atomo)
{
    info_atomo->atomo = ERRO;

    if (*buffer == '\'')
    {
        buffer++;
        goto q1;
    }

    return;

q1:
    if (*buffer != '\0')
    {
        info_atomo->atributo.ch = *buffer;
        buffer++;
        goto q2;
    }

    return;

q2:
    if (*buffer == '\'')
    {
        buffer++;
        info_atomo->atomo = CONSTCHAR;
        return;
    }

    return;
}

void reconhece_comentario(TInfoAtomo *info_atomo)
{
    info_atomo->atomo = ERRO;

    if (*buffer == '{')
    {
        buffer++;
        goto q1;
    }

    return;

q1:
    if (*buffer == '-')
    {
        buffer++;
        goto q2;
    }

    return;

q2:
    if (*buffer == '\n')
    {
        contaLinha++;
        buffer++;
        goto q2;
    }

    if (*buffer == '-')
    {
        buffer++;
        goto q3;
    }

    if (*buffer == '\0')
    {
        return; 
    }

    buffer++;
    goto q2;

q3:
    if (*buffer == '}')
    {
        buffer++;
        info_atomo->atomo = COMENTARIO;
        return;
    }

    goto q2;
}

// Analise sintatica
void consome(TAtomo atomo)
{
    while (lookahead == COMENTARIO)
    {
        imprime_atomo(info_atomo);
        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
    }

    if (lookahead == atomo)
    {
        imprime_atomo(info_atomo);

        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;

        while (lookahead == COMENTARIO)
        {
            imprime_atomo(info_atomo);
            info_atomo = obter_atomo();
            lookahead = info_atomo.atomo;
        }
    }
    else
    {
        printf("# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
               info_atomo.linha, strSimbolo[atomo], strSimbolo[lookahead]);
        exit(1);
    }
}

// <programa> ::= algoritmo identificador ‘;’ <bloco> ‘.’
void programa(void)
{
    consome(ALGORITMO); 
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA); 
    bloco();
    consome(PONTO);
}

// <bloco> ::= <declaração_variáveis> <declaração_de_rotinas> <comando_composto>
void bloco(void)
{
    declaracao_variaveis();
    declaracao_de_rotinas();
    comando_composto();
}

// <declaração_variáveis> ::= [ var <lista_variaveis> ‘;’ { <lista_variaveis> ‘;’ } ]
void declaracao_variaveis(void)
{
    if (lookahead == VAR)
    {
        consome(VAR);
        lista_variaveis();
        consome(PONTO_VIRGULA);

        while (lookahead == IDENTIFICADOR)
        {
            lista_variaveis();
            consome(PONTO_VIRGULA);
        }
    }
}

// <lista_variaveis> ::= identificador { ‘,’ identificador } ‘:’ <tipo>
void lista_variaveis(void)
{
    consome(IDENTIFICADOR);

    while (lookahead == VIRGULA)
    {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }

    consome(DOIS_PONTOS);
    tipo();
}

// <declaracao_de_rotinas> ::= { <declaração_de_função>|<declaração_de_procedimento> }
void declaracao_de_rotinas(void)
{
    while (lookahead == FUNCAO || lookahead == PROCEDIMENTO)
    {
        if (lookahead == FUNCAO)
        {
            declaracao_de_funcao();
        }
        else
        {
            declaracao_de_procedimento();
        }
    }
}

// <declaração_de_função> ::= funcao <tipo> identificador <parametros_formais> <declaracao_de_variaveis> <comando_composto>
void declaracao_de_funcao(void)
{
    consome(FUNCAO);
    tipo();
    consome(IDENTIFICADOR);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// <declaracao_de_procedimento> ::= procedimento identificador <parametros_formais> <declaracao_de_variaveis> <comando_composto>
void declaracao_de_procedimento(void)
{
    consome(PROCEDIMENTO);
    consome(IDENTIFICADOR);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// <tipo> ::= caractere | inteiro | logico
void tipo(void)
{
    if (lookahead == CARACTERE)
    {
        consome(CARACTERE);
    }
    else if (lookahead == INTEIRO)
    {
        consome(INTEIRO);
    }
    else
    {
        consome(LOGICO);
    }
}

// <parâmetros_formais> ::= ‘(’ <parâmetro_formal> { ‘;’ parâmetro_formal } ‘)’ | ‘(’ ‘)’
void parametros_formais(void)
{
    consome(ABRE_PAR);

    if (lookahead == VAR || lookahead == IDENTIFICADOR)
    {
        parametro_formal();

        while (lookahead == PONTO_VIRGULA)
        {
            consome(PONTO_VIRGULA);
            parametro_formal();
        }

        consome(FECHA_PAR);
    }
    else
    {
        consome(FECHA_PAR);
    }
}

// <parâmetro_formal> ::= [var] <lista_variaveis>
void parametro_formal(void)
{
    if (lookahead == VAR)
    {
        consome(VAR);
    }

    lista_variaveis();
}

void comando_composto(void)
{
    consome(INICIO);
    comando();

    while (lookahead == PONTO_VIRGULA)
    {
        consome(PONTO_VIRGULA);
        comando();
    }

    consome(FIM);
}

void comando(void)
{
    if (lookahead == IDENTIFICADOR)
    {
        consome(IDENTIFICADOR);

        if (lookahead == ATRIBUICAO)
        {
            comando_atribuicao(); 
        }
        else
        {
            chamada_procedimento();
        }
    }
    else if (lookahead == LEIA)
    {
        comando_entrada();
    }
    else if (lookahead == ESCREVA)
    {
        comando_saida();
    }
    else if (lookahead == SE)
    {
        comando_condicional();
    }
    else if (lookahead == ENQUANTO)
    {
        comando_repeticao();
    }
    else
    {
        comando_composto();
    }
}

// <comando_atribuição> ::= identificador ‘:=’ <expressão> 
void comando_atribuicao(void)
{
    consome(ATRIBUICAO);
    expressao();
}

// <chamada_procedimento> ::= identificador [ ‘(’ <lista_expressão> ‘)’ ]
void chamada_procedimento(void)
{
    if (lookahead == ABRE_PAR)
    {
        consome(ABRE_PAR);
        lista_expressao();
        consome(FECHA_PAR);
    }
}

// <comando_entrada> ::= leia ‘(’ identificador { ‘,’ identificador } ‘)’
void comando_entrada(void)
{
    consome(LEIA);
    consome(ABRE_PAR);
    consome(IDENTIFICADOR);

    while (lookahead == VIRGULA)
    {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }

    consome(FECHA_PAR);
}

// <comando_saida> ::= escreva ‘(’ <lista_expressão> ‘)’
void comando_saida(void)
{
    consome(ESCREVA);
    consome(ABRE_PAR);
    lista_expressao();
    consome(FECHA_PAR);
}

// <comando_condicional> ::= se <expressão> entao <comando> [ senao <comando> ] 
void comando_condicional(void)
{
    consome(SE);
    expressao();
    consome(ENTAO);
    comando();

    if (lookahead == SENAO)
    {
        consome(SENAO);
        comando();
    }
}

// <comando_repeticao> ::= enquanto <expressão> faca <comando> 
void comando_repeticao(void)
{
    consome(ENQUANTO);
    expressao();
    consome(FACA);
    comando();
}

// <lista_expressão> ::= <expressão> { ‘,’ <expressão> } 
void lista_expressao(void)
{
    expressao();

    while (lookahead == VIRGULA)
    {
        consome(VIRGULA);
        expressao();
    }
}

// <expressão> ::= <expressão_simples> [ <operador_relacional> <expressão_simples> ] 
void expressao(void)
{
    expressao_simples();

    if (lookahead == DIFERENTE ||
        lookahead == MENOR ||
        lookahead == MENOR_IGUAL ||
        lookahead == MAIOR_IGUAL ||
        lookahead == MAIOR ||
        lookahead == IGUAL)
    {
        operador_relacional();
        expressao_simples();
    }
}

// <operador_relacional> ::= ‘<>’ | ‘<’ | ‘<=’ | ‘>=’ | ‘>’ | ‘=’  
void operador_relacional(void)
{
    if (lookahead == DIFERENTE ||
        lookahead == MENOR ||
        lookahead == MENOR_IGUAL ||
        lookahead == MAIOR_IGUAL ||
        lookahead == MAIOR ||
        lookahead == IGUAL)
    {
        consome(lookahead);
    }
}

// <expressão_simples> ::= <termo> { <operador_adição> <termo> } 
void expressao_simples(void)
{
    termo();

    while (lookahead == MAIS ||
           lookahead == MENOS ||
           lookahead == MOD ||
           lookahead == OU)
    {
        operador_adicao();
        termo();
    }
}

// <operador_adição> ::= ‘+’ | ‘-’ | mod | ou 
void operador_adicao(void)
{
    if (lookahead == MAIS)
    {
        consome(MAIS);
    }
    else if (lookahead == MENOS)
    {
        consome(MENOS);
    }
    else if (lookahead == MOD)
    {
        consome(MOD);
    }
    else
    {
        consome(OU);
    }
}

// <termo> ::= <fator> { <operador_multiplicação> <fator> } 
void termo(void)
{
    fator();

    while (lookahead == MULT ||
           lookahead == DIV ||
           lookahead == E)
    {
        operador_multiplicacao();
        fator();
    }
}

// <operador_multiplicação> ::= ‘*’ | div | e  
void operador_multiplicacao(void)
{
    if (lookahead == MULT)
    {
        consome(MULT);
    }
    else if (lookahead == DIV)
    {
        consome(DIV);
    }
    else
    {
        consome(E);
    }
}

// <fator> ::= identificador [ ‘(’ <lista_expressão> ‘)’ ] | constint | constchar | ‘(’ <expressão> ‘)’ | ( ‘+’ | ‘-’ | nao ) <fator> | verdadeiro | falso
void fator(void)
{
    if (lookahead == IDENTIFICADOR)
    {
        consome(IDENTIFICADOR);

        if (lookahead == ABRE_PAR)
        {
            consome(ABRE_PAR);
            lista_expressao();
            consome(FECHA_PAR);
        }
    }
    else if (lookahead == CONSTINT)
    {
        consome(CONSTINT);
    }
    else if (lookahead == CONSTCHAR)
    {
        consome(CONSTCHAR);
    }
    else if (lookahead == ABRE_PAR)
    {
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    }
    else if (lookahead == MAIS ||
             lookahead == MENOS ||
             lookahead == NAO)
    {
        consome(lookahead);
        fator();
    }
    else if (lookahead == VERDADEIRO)
    {
        consome(VERDADEIRO);
    }
    else
    {
        consome(FALSO);
    }
}
