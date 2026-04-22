#!/bin/bash

# ---------------------------------------- Funciones ------------------------------------------------------------- #
ErrorCantidadParametros()
{
	if [ $1 -gt 7 ]; then
		echo "Error: Cantidad de parámetros incorrecta."
		echo "Uso: $0 [-h] [-z] [-d dir] [-u user1 user2 ...]"
		exit 1
	fi
}

ErrorAWK()
{
	if [ ! -x "$(command -v awk)" ]; then
		echo "Error: El comando awk no está disponible."
		exit 1
	fi
}

FuncionUsuario()
{
	# Limpiar USER_LIST si se proporciona la opción -u
	USER_LIST=()
	while [[ -n "$1" && "$1" != "-"* ]]; do
		USER_LIST+=("$1")
		shift
	done
	if [ ${#USER_LIST[@]} -eq 0 ]; then
		echo "Error: Falta el nombre de usuario."
		exit 1
	fi
}

FuncionDirectorio()
{
	if ([ -n "$1" ] && [ "$1" != "-d" ] && [ "$1" != "-h" ] && [ "$1" != "-z" ] && [ "$1" != "-d" ]); then
		DIR=$1
		PIDS=$(lsof +d $DIR | tail -n+2 | tr -s ' ' | cut -d ' ' -f 2 | sort -u)
	else
		echo "Error: Falta el directorio."
		exit 1
	fi
}

MostrarDatos()
{ 
	# Convierte USER_LIST en una expresión regular para awk
	USUARIOS=$(echo "${USER_LIST[@]}" | sed 's/ /|/g')
	ps -eo sess,pgid,pid,user,tty,%mem,cmd | awk -v SID0=$SID0 -v USUARIOS="$USUARIOS" -v TERMINAL=$TERMINAL '
	  BEGIN {
	    printf "%-20s %-20s %-20s %-20s %-20s %-20s %-20s", "SID", "PGID", "PID", "USER", "TTY", "%MEM", "CMD"
	  }
	  NR > 1 {
	    if ((SID0 == "false" && $1 == "0") || ($4 !~ USUARIOS) || (TERMINAL == "true" && ($5 == "??" || $5 == "?"))) next
	    printf "\n%-20s %-20s %-20s %-20s %-20s %-20s %-20s", $1, $2, $3, $4, $5, $6, $7
	  }
	'
}

# ---------------------------------------- Variables ------------------------------------------------------------- #
HELP=false
SID0=false
USER_LIST=($(whoami))  # Usuario actual por defecto
DIR=""
PIDS=""
TERMINAL=false
PROCESOS=false
ORDENAMIENTO_PROCESOS=false
ORDENAMIENTO_SESIONES=false
REVERTIR=false
TABLA=""

# ---------------------------------------- Procesar opciones y argumentos ---------------------------------------- #
ErrorAWK
ErrorCantidadParametros $#
while [ -n "$1" ]; do
	case "$1" in
		-h)
			HELP=true
			;;
		-z)
			SID0=true
			;;
		-u)
			shift
			FuncionUsuario "$@"
			while [[ -n "$1" && "$1" != "-"* ]]; do shift; done
			;;
		-d)
			FuncionDirectorio $2
			shift
			;;
		-t)
			TERMINAL=true
			;;
		-e)
			PROCESOS=true
			;;
		-sm)
			ORDENAMIENTO_PROCESOS=true
			;;
		-sg)
			ORDENAMIENTO_SESIONES=true
			;;
		-r)
			REVERTIR=true
			;;
		*)
			echo "Error: El parámetro $1 no reconocido en este programa."
			echo "Ejecute con -h para más información."
			exit 1
			;;
	esac
	shift
done

# ---------------------------------------- Evaluar y actuar ------------------------------------------------------ #
if $HELP; then
	FuncionAyuda
	exit 0
fi

TABLA="$(MostrarDatos)"

echo "$TABLA"
