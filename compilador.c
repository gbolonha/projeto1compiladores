TInfoAtomo info_atomo;
TAtomo lookahead;

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

    // controle
    FIM_ARQUIVO
} TAtomo;


typedef struct{
  TAtomo atomo;
  int linha;
  union{
    int numero; // atributo do átomo constint (constante inteira)
    char id[16]; // atributo identificador
    char ch; // atributo do átomo constchar (constante caractere)
  }atributo;
}TInfoAtomo;


char *strAtomo[] = {"Erro lexico", "Constante Numerica", "Identificador","*", "+","Fim de buffer"};




TInfoAtomo obter_atomo(); // implementado no analisador léxico
void consome(TAtomo atomo); // implementado no analisador sintático


int main()
{
    return 0;
}



//analise sintatica

void consome( TAtomo atomo ){
    if( lookahead == atomo ){
        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
    }
    else{
        // tratador de erros
        printf("Erro sintatico: esperado [%s] encontrado [%s]\n",strAtomo[atomo],strAtomo[lookahead]);
        exit(1);
    }
}

//<programa> ::= algoritmo identificador ‘;’ <bloco> ‘.’

void programa()
{
  consome(ALGORITMO);
  consome(IDENTIFICADOR);
  consome(PONTO_VIRGULA);
  bloco();
  consome(PONTO);
}

//<bloco> ::= <declaração_variáveis> <declaração_de_rotinas> <comando_composto>
void bloco()
{
    declaracao_variaveis();
    declaracao_rotinas();
    comando_composto();
}

//<declaração_variáveis> ::= [ var <lista_variaveis> ‘;’ { <lista_variaveis> ‘;’ } ]
void declaracao_variaveis()
{
    if (lookahead==VAR)
    {
        consome(VAR)
        lista_variaveis();
        consome(PONTO_VIRGULA)
        while (lookahead==IDENTIFICADOR)
        {
            lista_variaveis();
            consome(PONTO_VIRGULA);
        }
    }
}

//<lista_variaveis> ::= identificador { ‘,’ identificador } ‘:’ <tipo>
void lista_variaveis()
{
    consome(IDENTIFICADOR);
    while(lookahead==PONTO_VIRGULA)
    {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    consome(DOIS_PONTOS);
    tipo();
}


//<declaracao_de_rotinas> ::= { <declaração_de_função>|<declaração_de_procedimento> }
void declaracao_de_rotinas()
{
    if(lookahead==FUNCAO)
    {
        while (lookahead==FUNCAO)
        {
            declaração_de_função();
        }
    }
    else if (lookahead==PROCEDIMENTO)
    {
        while (lookahead==PROCEDIMENTO)
        {
            declaracao_de_procedimento();
        }
    }
}


//<declaração_de_função> ::= funcao <tipo> identificador <parametros_formais> <declaracao_de_variaveis> <comando_composto>
void declaracao_de_funcao()
{
    consome(FUNCAO);
    tipo();
    consome(IDENTIFICADOR);
    parametros_formais();
    declaracao_de_variaveis();
    comando_composto();
}

//<declaracao_de_procedimento> ::= procedimento identificador <parametros_formais> <declaracao_de_variaveis> <comando_composto>
void declaracao_de_procedimento()
{
    consome(PROCEDIMENTO);
    consome(IDENTIFICADOR);
    parametros_formais();
    declaracao_de_variaveis();
    comando_composto();
}

//<tipo> ::= caractere | inteiro | logico
void tipo()
{
    if (lookahead==CARACTERE)
    {
        consome(CARACTERE);
    }
    else if (lookahead==INTEIRO)
    {
        consome(INTEIRO);
    }
    else
    {
        consome(LOGICO);
    }
}

//<parâmetros_formais> ::= ‘(’ <parâmetro_formal> { ‘;’ parâmetro_formal } ‘)’ | ‘(’ ‘)
void parametros_formais()
{
    consome(ABRE_PAR);
    if (lookahead==VAR||lookahead==IDENTIFICADOR)
    {
        parametro_formal();
        while(lookahead==PONTO_VIRGULA)
        {
            consome(PONTO_VIRGULA);
            parametro_formal
        }
        consome(FECHA_PAR);
    }
    else
    {
        consome(FECHA_PAR);
    }
    
}

//<parâmetro_formal> ::= [var] <lista_variaveis>
void parametro_formal()
{
    if (lookahead==VAR)
    {
        consome(VAR);
    }
    lista_variaveis();
}


