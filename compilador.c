
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


char *strAtomo[] = {
    "algoritmo",       // 0
    "caractere",       // 1
    "div",             // 2
    "e",               // 3
    "enquanto",        // 4
    "entao",           // 5
    "escreva",         // 6
    "faca",             // 7
    "falso",            // 8
    "fim",              // 9
    "funcao",           // 10
    "inicio",           // 11
    "inteiro",          // 12
    "leia",             // 13
    "logico",           // 14
    "mod",              // 15
    "ou",               // 16
    "procedimento",     // 17
    "se",               // 18
    "senao",            // 19
    "var",              // 20
    "verdadeiro",       // 21
    "ponto_virgula",    // 22
    "ponto",            // 23
    "virgula",          // 24
    "dois_pontos",      // 25
    "abre_par",         // 26
    "fecha_par",        // 27
    "atribuicao",       // 28
    "menor",            // 29
    "menor_igual",      // 30
    "maior",            // 31
    "maior_igual",      // 32
    "igual",            // 33
    "diferente",        // 34
    "mais",              // 35
    "menos",             // 36
    "mult",              // 37
    "identificador",    // 38
    "constint",         // 39
    "constchar",        // 40
    "comentario", 
    "nao",
    "fim_arquivo"       // 42
};

TInfoAtomo info_atomo;
TAtomo lookahead;



TInfoAtomo obter_atomo(); // implementado no analisador léxico
void consome(TAtomo atomo); // implementado no analisador sintático


int main()
{
    
    info_atomo=obter_atomo();
    lookahead=info_atomo.atomo;
    programa();
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
    declaracao_de_rotinas();
    comando_composto();
}

//<declaração_variáveis> ::= [ var <lista_variaveis> ‘;’ { <lista_variaveis> ‘;’ } ]
void declaracao_variaveis()
{
    if (lookahead==VAR)
    {
        consome(VAR);
        lista_variaveis();
        consome(PONTO_VIRGULA);
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
    while(lookahead==VIRGULA)
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
   while(lookahead==FUNCAO||lookahead==PROCEDIMENTO)
       {
           if (lookahead==FUNCAO)
           {
               declaracao_de_funcao();
           }
           else
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
            parametro_formal();
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

void comando_composto()
{
    consome(INICIO);
    comando();
    while(lookahead==PONTO_VIRGULA)
    {
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(FIM);
}


void comando()
{
    if (lookahead==IDENTIFICADOR)
    {
        consome(IDENTIFICADOR);
        if(lookahead==ATRIBUICAO)
        {
           comando_atribuicao(); 
        }
        else
        {
            chamada_procedimento();
        }
    }
    else if (lookahead==LEIA)
    {
        comando_entrada();
    }
    else if (lookahead==ESCREVA)
    {
        comando_saida();
    }
    else if(lookahead==SE)
    {
        comando_condicional();
    }
    else if (lookahead==ENQUANTO)
    {
        comando_repeticao();
    }
    else
    {
        comando_composto();
    }
}


//<comando_atribuição> ::= identificador ‘:=’ <expressão> 
void comando_atribuicao()
{
    consome(ATRIBUICAO);
    expressao();
}

//<chamada_procedimento> ::= identificador [ ‘(’ <lista_expressão> ‘)’ ]
void chamada_procedimento()
{
    if(lookahead==ABRE_PAR)
    {
        consome(ABRE_PAR);
        lista_expressao();
        consome(FECHA_PAR);
    }
}



//<comando_entrada> ::= leia ‘(’ identificador { ‘,’ identificador } ‘)
void comando_entrada()
{
    consome(LEIA);
    consome(ABRE_PAR);
    consome(IDENTIFICADOR);
    while(lookahead==VIRGULA)
    {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    consome(FECHA_PAR);
}

//<comando_saida> ::= escreva ‘(’ <lista_expressão> ‘)’
void comando_saida()
{
    consome(ESCREVA);
    consome(ABRE_PAR);
    lista_expressao();
    consome(FECHA_PAR);
}
//<comando_condicional> ::= se <expressão> entao <comando> [ senao <comando> ] 
void comando_condicional()
{
    consome(SE);
    expressao();
    consome(ENTAO);
    comando();
    if(lookahead==SENAO)
    {
        consome(SENAO);
        comando();
    }
}
//<comando_repeticao> ::= enquanto <expressão> faca <comando> 
void comando_repeticao()
{
    consome(ENQUANTO);
    expressao();
    consome(FACA);
    comando();
}

//<lista_expressão> ::= <expressão> { ‘,’ <expressão> } 
void lista_expressao()
{
    expressao();
    while(lookahead==VIRGULA)
    {
        consome(VIRGULA);
        expressao();
    }
}
//<expressão> ::= <expressão_simples> [ <operador_relacional> <expressão_simples> ] 
void expressao()
{
    expressao_simples();
    if (lookahead == DIFERENTE || lookahead == MENOR || lookahead == MENOR_IGUAL ||lookahead == MAIOR_IGUAL ||lookahead == MAIOR || lookahead == IGUAL)
    {
        operador_relacional();
        expressao_simples();
    }

}
//<operador_relacional> ::= ‘<>’ | ‘<’ | ‘<=’ | ‘>=’ | ‘>’ | ‘=’  
void operador_relacional()
{
    if (lookahead == DIFERENTE || lookahead == MENOR || lookahead == MENOR_IGUAL ||lookahead == MAIOR_IGUAL ||lookahead == MAIOR || lookahead == IGUAL)
    {
        consome(lookahead);
    }
}
//<expressão_simples> ::= <termo> { <operador_adição> <termo> } 
void expressao_simples()
{
    termo();
    while(lookahead==MAIS||lookahead==MENOS||lookahead==MOD||lookahead==OU)
    {
        operador_adicao();
        termo();
    }
}
//<operador_adição> ::= ‘+’ | ‘-’ | mod | ou 
void operador_adicao()
{
    if (lookahead==MAIS)
    {
        consome(MAIS);
    }
    else if (lookahead==MENOS)
    {
        consome(MENOS);
    }
    else if (lookahead==MOD)
    {
        consome(MOD);
    }
    else
    {
        consome(OU);
    }
}


//<termo> ::= <fator> { <operador_multiplicação> <fator> } 
void termo()
{
    fator();
    while (lookahead==MULT||lookahead==DIV||lookahead==E)
    {
        operador_multiplicacao();
        fator();
    }
}

//<operador_multiplicação> ::= ‘*’ | div | e  
    //<operador_multiplicação> ::= ‘*’ | div | e  
void operador_multiplicacao()
{
    if (lookahead==MULT)
    {
        consome(MULT);
    }
    else if (lookahead==DIV)
    {
        consome(DIV);
    }
    else
    {
        consome(E);
    }
}

//<fator> ::= identificador [ ‘(’ <lista_expressão> ‘)’ ] | constint | constchar | ‘(’ <expressão> ‘)’ | ( ‘+’ | ‘-’ | nao ) <fator> | verdadeiro | falso

void fator()
{
    if (lookahead==IDENTIFICADOR)
    {
        consome(IDENTIFICADOR);
        if (lookahead==ABRE_PAR)
        {
            consome(ABRE_PAR);
            lista_expressao();
            consome(FECHA_PAR);
        }
    }
    else if (lookahead==CONSTINT)
    {
        consome(CONSTINT);
    }
    else if (lookahead==CONSTCHAR)
    {
        consome(CONSTCHAR);
    }
    else if (lookahead==ABRE_PAR)
    {
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    }
    else if (lookahead==MAIS||lookahead==MENOS||lookahead==NAO)
    {
        consome(lookahead);
        fator();
    }
    else if(lookahead==VERDADEIRO)
    {
        consome(VERDADEIRO);
    }
    else
    {
        consome(FALSO);
    }
    
}
