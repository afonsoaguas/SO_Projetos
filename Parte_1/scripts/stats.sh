#!/bin/bash

if [[ "$1" == "cidadaos" ]]; then
	if [[ $# -eq 1 || $# -gt 2 ]];then
		echo "Erro: Síntaxe:$0<cidadaos><localidade>"
	else
		echo "O número de cidadãos registados em $2 é $(grep -c "$2" cidadaos.txt)."
	fi
elif [[ "$1" == "registados" ]]; then
	echo "$(sort -t":" -k 3 -nr cidadaos.txt | awk -F: '{if ($3 > 60) print $2":"$3 }')"
elif [[ "$1" == "enfermeiros" ]]; then
	echo "$(awk -F":" '{if (( $5 == 1 )) print $2}' enfermeiros.txt)"
else
	echo "Erro: Síntaxe:$0<arg>"
fi
