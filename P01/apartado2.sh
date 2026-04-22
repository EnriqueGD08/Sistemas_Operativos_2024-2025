#!/bin/bash

# Asignatura: Sistemas Operativos
# Práctica: Práctica 1
# Autor: Enrique Gómez Díaz
# Fecha: 07/11/2024
# Descripción: Apartado 2

# ---------------------------------------- Funciones ------------------------------------------------------------- #
ErrorCantidadParametros()
{
	if [ $1 -gt 7 ]; then
	echo "Error: Cantidad de parámetros incorrecta."
	echo "Uso: $0 [-h] [-z] [-d dir] [-u user1,user2,...]"
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
	if ([ -n "$1" ] && [ "$1" != "-u" ] && [ "$1" != "-h" ] && [ "$1" != "-z" ] && [ "$1" != "-d" ]); then
		USER=$1
	else
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
# Se cambian las comas que separan los usuarios por "|" para que al pasarlos por awk, se compare el valor de $4 con cada usario de la lista.
# El parámetro "s" de sed es para cambiar lo que se le pase después de / por lo que se le pase después de la segunda /.
# El parámetro "g" de sed es para que busque en toda la lína los parámetros que tiene que cambiar.
    USUARIOS=$(echo $USER | sed 's/,/|/g')

    ps -eo sess,pgid,pid,user,tty,%mem,cmd | awk -v SID0=$SID0 -v USUARIOS="$USUARIOS" -v TERMINAL=$TERMINAL '
      BEGIN {
        printf "%-20s %-20s %-20s %-20s %-20s %-20s %-20s", "SID", "PGID", "PID", "USER", "TTY", "%MEM", "CMD"
      }
      NR > 1 {
        if ((SID0 == "false" && $1 == "0") || ($4 !~ USUARIOS) || (TERMINAL == "true" && $5 == "??")) next
        printf "\n%-20s %-20s %-20s %-20s %-20s %-20s %-20s", $1, $2, $3, $4, $5, $6, $7
      }
    '
}

FuncionAyuda()
{
  echo  "Uso: $0 [-h] [-z] [-u user1,user2,...]"
  echo  "Opciones:"
  echo  "-h        Muestra información de como utilizar el script."
  echo  "-z        Incluye en la tabla los procesos con SID 0."
  echo  "-u user   Solo muestra los procesos cuyo usuario efectivo sea el especificado (puede ser una lista de usuarios separados por comas)."
  echo  "-d dir    Procesos que tengan abiertos archivos en el directorio especificado."
}

FiltrarPIDS()
{
  echo "$TABLA" | head -n 1
  for PID in $PIDS ; do
    if [ -n "$(echo "$TABLA" | tr -s ' ' | cut -d ' ' -f 3 | grep -w $PID)" ]; then
      echo "$TABLA" | grep -w $PID 
    fi
  done
}

MostrarSesiones() 
{
	awk '
	BEGIN {
		printf "%-20s %-20s %-20s %-20s %-20s %-20s", "SID", "Total PGIDs", "Proceso lider", "USER", "TTY", "CMD"
	}	
	'
# Se crean vectores para cada campo y se almacenan los valores para cada valor distinto de SID. Sino, solo se incrementa el número de procesos.
	echo "$TABLA" | tail -n+2 | awk '
	{
		if (SID[$1] == "") {
			SID[$1] = $1
			PROCESOS[$1] = 1
			LEADER[$1] = $3
			USER[$1] = $4
			TTY[$1] = $5
			CMD[$1] = $7
		} else {
			PROCESOS[$1]++
		}
	}
	END {
		for (i in SID) {
			printf "\n%-20s %-20s %-20s %-20s %-20s %-20s", SID[i], PROCESOS[i], LEADER[i], USER[i], TTY[i], CMD[i]
		}
	}
	'
}

# ---------------------------------------- Variables ------------------------------------------------------------- #
HELP=false
SID0=false
USER=$(whoami)
DIR=""
PIDS=""
TERMINAL=false
PROCESOS=false

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

if $PROCESOS; then
	if [ -n "$DIR" ]; then
  	FiltrarPIDS
	else
  	echo "$TABLA"
	fi
else
	if [ -n "$DIR" ]; then
		TABLA="$(FiltrarPIDS)"
  	MostrarSesiones
	else
  	MostrarSesiones
	fi
fi