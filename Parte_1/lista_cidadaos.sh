#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 99114  Nome: Afonso Valente Mareco Ferreira Águas
## Nome do Módulo: lista_cidadaos.sh
## Descrição/Explicação do Módulo: 
## A explicação está em comentarios ao longo do código
##
###############################################################################

awk '{gsub("\ \|\ ",":");print}' listagem.txt | awk -F":" '{split($4,year,"-");print(((10000+NR))":"$2":"((2021-year[3]))":"$6":"$8":0")}' > cidadaos.txt
cat cidadaos.txt

#O primeiro comando awk substitui todas as ocorrências (gsub) de "[space]|[space]" e substitui por ":" fazendo print do resto da linha.
#O segundo awk deteta como separador das colunas os ":" (-F":"), separa (split) a 4º coluna (data de nascimento) em três subcolunas (dia,mês,ano), faz print das colunas desejadas separando por dois pontos, como primeria coluna é acrescentado o número da linha somado com 10000 (10000+NR), na 3º coluna(ano de nascimento) faz a subtração (2021-year[3]) para saber a idade de cada cidadao em 2021 e é acrescentada como ultima coluna o estado de vacinação(neste caso 0 porque ainda nenhum cidadao foi vacinado), por fim direciona o resultado para o ficherio cidadaos.txt, eleminando um já existente.
#O comando cat mostra a lista de cidadaos criada no final da execução.
