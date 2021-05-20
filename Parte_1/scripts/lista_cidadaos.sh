#!/bin/bash

#awk -F"|" '{split($1,name,":");split($2,bir,":");split($3,loc,":");split($4,tel,":");print(((10000+NR))":"name[2]":"bir[2]":"loc[2]":"tel[2])}' listagem.txt | awk -F":" '{split($3,year,"-");print($1":"$2":"year[3]":"$4":"$5":0")}' | awk '{gsub("\ :",":");print}' #> cidadaos.txt

awk '{gsub("\ \|\ ",":");print}' listagem.txt | awk -F":" '{split($4,year,"-");print(((10000+NR))":"$2":"((2021-year[3]))":"$6":"$8":0")}' > cidadaos.txt
cat cidadaos.txt

#A primeira função awk substitui todas as ocorrências (gsub) de "[space]|[space]" e substitui por ":" fazendo print do resto da linha.
#A segunda função awk deteta como separador das colunas os ":" (-F":"), separa (split) a 4º coluna (data de nascimento) em três subcolunas (dia,mês,ano), faz print das colunas desejadas separando por dois pontos, como primeria coluna é acrescentado o número da linha somado com 10000 (10000+NR), na 3º coluna(ano de nascimento) faz a subtração (2021-year[3]) para saber a idade de cada cidadao em 2021 e é acrescentada como ultima coluna o estado de vacinação(neste caso 0 porque ainda nenhum cidadao foi vacinado), por fim direciona o resultado para o ficherio cidadaos.txt, eleminando um já existente.
#A função cat mostra a lista de cidadaos criada no final da execução.

#| awk -F":" '{print($1":"$2":"((2021-$3))":"$4":"$5":"$6)}' 
# | awk -F":" '{split($3,year,"-");print($1":"$2":"year[3]":"$4":"$5":"$6)}'
