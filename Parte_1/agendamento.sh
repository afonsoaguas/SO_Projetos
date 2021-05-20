#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 99114  Nome: Afonso Valente Mareco Ferreira Águas
## Nome do Módulo: agendamento.sh
## Descrição/Explicação do Módulo: 
## A explicação está em comentarios ao longo do código
##
###############################################################################

if [[ -e agenda.txt ]]; then #No caso de existir o ficheiro agenda.txt elemina-o
	rm agenda.txt
fi

if [[ $(./stats.sh enfermeiros | wc -l) != 0 ]] ; then #Executa o script stats.sh enfermeiros, para verificar se existem enfermeiros disponiveis (wc -l vai contar o numero de linhas resultante do comando anterior), no caso de não haver aparece uma mensagem de erro
	for i in $(awk -F":" '/1$/ { print $1 }' enfermeiros.txt); do #A variável "i" vai percorrer todos os numeros de cédula dos enfermiros disponiveis
		first=$(grep "^$i" enfermeiros.txt | awk -F":" '{ print ($2":"$1) }') #Na variável "first" guardo o nome e o número de cédula de cada enfermeiro (nome:numero de cedula). O comando grep vai procurar pela linha que começa com cada valor da variavel "i", o comando awk vai fazer print do nome e do número de cédula de cada enfermeiro
		loc=$(grep "^$i" enfermeiros.txt | awk -F":" '{ sub("CS","");print $3 }') #Na variavel "loc" guardo o localização do Centro de Saude de cada enfermeiro (sem o "CS"). O comando grep vai procurar pela linha que começa com cada valor da variavel "i", o comando awk vai fazer print da localidado do Centro de Saúde
		CSloc=$(grep "^$i" enfermeiros.txt | awk -F":" '{ print $3 }') #Na variavel "CSloc" guardo o Centro de Saude de cada enfermeiro. O comando grep vai procurar pela linha que começa com cada valor da variavel "i", o comando awk vai fazer print do Centro de Saúde
		data=$(date -I'date') #Na variavel "data" guardo a data da execução do script com o formato ano-mês-dia
		awk -F":" -v loc="$loc" -v first="$first" -v data="$data" -v CSloc="$CSloc" '{ if (( $4 == loc )) print ( first":"$2":"$1":"CSloc":"data ) }' cidadaos.txt >> agenda.txt #O comando awk no caso da localidade no coluna $4 corresponder á localidade do enfermeiro irá fazer print do nome e do número de cédula de cada cidadao, juntamente com as outras variaveis na ordem pretendida (enfermeiro:cidadao:centro de saude:data)
	done
	echo ""
	cat agenda.txt #Mostra no terminal o conteudo do ficheiro agenda.txt
	echo ""
else
	echo "Erro: Não há enfermeiros disponiveis!" #Mensagem de erro
fi
