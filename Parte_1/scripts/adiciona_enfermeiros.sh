#!/bin/bash

touch enfermeiros.txt

if [ $# -eq 4 ];then
	if [[ $(grep -c $3 enfermeiros.txt) != 0 ]]; then
		echo "Erro: O Centro de Saúde introduzido já tem um enfermeiro registado"
	else
		if [[ $(grep -c $2 enfermeiros.txt) == 0 ]]; then
			echo "$2:$1:$3:0:$4" >> enfermeiros.txt
		else
			echo "Erro: O número de cédula profissional já esta introduzido num Centro de Saúde"
		fi
	fi
else
	echo "Erro: Síntaxe: $0<número cédula profissional>:<nome>:<centro saúde associado>:<nº de vacinações efetuadas>:<disponibilidade>"
fi

cat enfermeiros.txt
