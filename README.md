# Compilador Portugol — Fase 1: Análise Léxica e Sintática

Projeto desenvolvido para a disciplina de Compiladores. O objetivo deste trabalho é implementar as fases de análise léxica e de análise sintática (via Analisador Sintático Descendente Recursivo — ASDR) para uma linguagem Portugol baseada em Pascal com palavras reservadas em português.

---

## Integrantes do Grupo

* **Gabriel Teixeira Bolonha** — RA: 10426937
* **Geovana Bomfim Rodrigues** — RA: 10410514

---

## Como Compilar e Executar

### Compilação
Utilize o comando exato especificado no enunciado:

```bash
gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador
./compilador arquivo.txt
