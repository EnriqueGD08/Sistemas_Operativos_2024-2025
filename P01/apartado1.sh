#!/bin/bash

# Asignatura: Sistemas Operativos
# Práctica: Práctica 1
# Autor: Enrique Gómez Díaz
# Fecha: 31/10/2024
# Descripción: Apartado 1

# ---------------------------------------- Funciones ------------------------------------------------------------- #
ErrorCantidadParametros()
{
	if [ $1 -gt 5 ]; then
	echo "Error: Cantidad de parámetros incorrecta."
	echo "Uso: $0 [-h] [-z] [-u user]"
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
	if ([ -n "$1" ] && [ "$1" != "-u" ] && [ "$1" != "-h" ] && [ "$1" != "-z" ]); then
		USER=$1
	else
		echo "Error: Falta el nombre de usuario."
		exit 1
	fi
}

MostrarDatos()
{
  ps -eo sid,pgid,pid,user,tty,%mem,cmd | awk -v SID0=$SID0 -v USER=$USER '
		BEGIN {
		  printf "%-20s %-20s %-20s %-20s %-20s %-20s %-20s", "SID", "PGID", "PID", "USER", "TTY", "%MEM", "CMD"
		}
		NR > 1 {
			if ((SID0 == "false" && $1 == 0) || ($3 != USER)) next			
		  printf "\n%-20s %-20s %-20s %-20s %-20s %-20s %-20s", $1, $2, $3, $4, $5, $6, $7
		}
	'
}

FuncionAyuda()
{
  echo  "Uso: $0 [-h] [-z] [-u user]"
  echo  "Opciones:"
  echo  "-h        Muestra información de como utilizar el script."
  echo  "-z        Incluye en la tabla los procesos con SID 0."
  echo  "-u user   Solo muestra los procesos cuyo usuario efectivo sea el especificado."
}

# ---------------------------------------- Variables ------------------------------------------------------------- #
HELP=false
SID0=false
USER=&(whoami)

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
			FuncionUsuario $2
			shift
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
MostrarDatos