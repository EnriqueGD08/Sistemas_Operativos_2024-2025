#!/bin/bash

# Asignatura: Sistemas Operativos
# Práctica: Práctica 1
# Autor: Enrique Gómez Díaz
# Fecha: 07/11/2024
# Descripción: Apartado 2

# ---------------------------------------- Funciones ------------------------------------------------------------- #

# Función para controlar si la cantidad de parámetros es correcta.
ErrorCantidadParametros()
{
	if [ $1 -gt 7 ]; then
		echo "Error: Cantidad de parámetros incorrecta."
		echo "Uso: $0 [-h] [-z] [-d dir] [-u user1,user2,...]"
		exit 1
	fi
}

# Función para comprobar si el comando awk está instalado.
ErrorAWK()
{
	if [ ! -x "$(command -v awk)" ]; then
		echo "Error: El comando awk no está disponible."
		exit 1
	fi
}

# Función que guarda en un vector los nombres de usuario.
FuncionUsuario()
{
	USER=()
	while [[ -n "$1" && "$1" != "-"* ]]; do
		USER+=("$1")
		shift
	done
	if [ -z "$USER" ]; then
		echo "Error: Falta el nombre de usuario."
		exit 1
	fi
}

# Función que guarda los PIDs de los procesos que tienen archivos abiertos en un directorio especificado.
FuncionDirectorio()
{
	if ([ -n "$1" ] && [ "$1" != "-"* ]); then
		DIR=$1
    PIDS=$(lsof +d $DIR | tail -n+2 | tr -s ' ' | cut -d ' ' -f 2 | sort -u)
	else
		echo "Error: Falta el directorio."
		exit 1
	fi
}

# Función que muestra los datos de los procesos.
MostrarDatos()
{
	USUARIOS=$(echo "${USER[@]}" | sed 's/ /|/g')
	ps -eo sess,pgid,pid,user,tty,%mem,cmd | awk -v SID0=$SID0 -v USUARIOS="$USUARIOS" -v TERMINAL=$TERMINAL '
	  NR > 1 {
	    if ((SID0 == "false" && $1 == "0") || ($4 !~ USUARIOS) || (TERMINAL == "true" && ($5 == "??" || $5 == "?"))) next
	    printf "\n%-20s %-20s %-20s %-20s %-20s %-20s %-20s", $1, $2, $3, $4, $5, $6, $7
	  }
	'
}

# Función que muestra la información de ayuda.
FuncionAyuda()
{
  echo  "Uso: $0 [-h] [-z] [-u user1,user2,...] [-f archivo]"
  echo  "Opciones:"
  echo  "-h        Muestra información de como utilizar el script."
  echo  "-z        Incluye en la tabla los procesos con SID 0."
  echo  "-u user   Solo muestra los procesos cuyo usuario efectivo sea el especificado (puede ser una lista de usuarios separados por comas)."
  echo  "-d dir    Procesos que tengan abiertos archivos en el directorio especificado."
  echo  "-f archivo Redirige la salida a un archivo si esta excede las 5 líneas."
}

# Función que filtra los procesos por los PIDs que se han encontrado en la función FuncionDirectorio.
FiltrarPIDS()
{
  echo "$TABLA" | head -n 1
  for PID in $PIDS ; do
    if [ -n "$(echo "$TABLA" | tr -s ' ' | cut -d ' ' -f 3 | grep -w $PID)" ]; then
      echo "$TABLA" | grep -w $PID 
    fi
  done
}

# Función que agrupa los procesos por SID.
MostrarSesiones() 
{
	echo "$TABLA" | sort -k 1 -k 2 | LC_ALL=C awk '
	{
		if (SID[$1] == "") {
			SID[$1] = $1
			GRUPO[$1] = $2
			PROCESOS[$1] = 1
			MEMORIA[$1] = $6
			if ($3 == $1) {
				LEADER[$1] = $3
				USER[$1] = $4
			} else {
				LEADER[$1] = "?"
				USER[$1] = "?"
			}
			TTY[$1] = $5
			CMD[$1] = $7
		} else {
			if (GRUPO[$1] != $2) {
				if ($3 == $1) {
					LEADER[$1] = $3
				}
				PROCESOS[$1]++
				MEMORIA[$1] += $6
			}
		}
	}
	END {
		for (i in SID) {
			printf "\n%-20s %-20s %-20s %-20s %-20s %-20s %-20s", SID[i], PROCESOS[i], MEMORIA[i], LEADER[i], USER[i], TTY[i], CMD[i]
		}
	}
	' | tail -n+2
}

# ---------------------------------------- Variables ------------------------------------------------------------- #

HELP=false
SID0=false
USER=($(whoami))
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
			shift
			;;
		-z)
			SID0=true
			shift
			;;
		-u)
			shift
			# Se pasa el resto de argumentos a la función FuncionUsuario.
			FuncionUsuario "$@"
			# Se eliminan los argumentos que ya se han procesado.
			while [[ -n "$1" && "$1" != "-"* ]]; do
				shift
			done
			;;
		-d)
			FuncionDirectorio $2
			shift
			shift
			;;
		-t)
			TERMINAL=true
			shift
			;;
		-e)
			PROCESOS=true
			shift
			;;
		-sm)
			ORDENAMIENTO_PROCESOS=true
			shift
			;;
		-sg)
			ORDENAMIENTO_SESIONES=true
			shift
			;;
		-r)
			REVERTIR=true
			shift
			;;
		*)
			echo "Error: El parámetro $1 no reconocido en este programa."
			echo "Ejecute con -h para más información."
			exit 1
			;;
	esac
done

# ---------------------------------------- Evaluar y actuar ------------------------------------------------------ #

# Si se ha solicitado la ayuda, se muestra la información de ayuda y no se hace nada más.
if $HELP; then
	FuncionAyuda
	exit 0
fi

# Controla que no se han puesto opciones de ordenamiento que no son compatibles.
if ($ORDENAMIENTO_PROCESOS && $ORDENAMIENTO_SESIONES) || ($ORDENAMIENTO_SESIONES && $PROCESOS); then
	echo "Error: Se han usado los parámetros de ordenamiento de manera incorrecta."
	exit 1
fi

# Se obtiene la tabla con los datos de los procesos con algunos parámetros ya aplicados.
TABLA="$(MostrarDatos)"

# Se aplican los filtros que faltan por aplicar.
if $PROCESOS; then
	if [ -n "$DIR" ]; then
  	TABLA="$(FiltrarPIDS)"
	fi
else
	if [ -n "$DIR" ]; then
		TABLA="$(FiltrarPIDS)"
  	TABLA="$(MostrarSesiones)"
	else
  	TABLA="$(MostrarSesiones)"
	fi
fi

# Se ordena la tabla según el nombre de usuario.
if $REVERTIR; then
	TABLA="$(echo "$TABLA" | sort -k 4 -r -f)"
else 
	TABLA="$(echo "$TABLA" | sort -k 4 -f)"
fi

#if $ORDENAMIENTO_PROCESOS; then
#	TABLA="$(echo "$TABLA" | sort -k 3)"
#elif $ORDENAMIENTO_SESIONES; then	
#	TABLA="$(echo "$TABLA" | sort -k 1)"
#else
#	TABLA="$(echo "$TABLA" | sort -k 4)"
#fi

#if $REVERTIR; then
#	TABLA="$(echo "$TABLA" | tac)"
#fi

# Se ordena la tabla según el parámetro de ordenamiento seleccionado.
if $ORDENAMIENTO_PROCESOS; then
	if $REVERTIR; then
		TABLA="$(echo "$TABLA" | sort -k 6 -r)"
	else
		TABLA="$(echo "$TABLA" | sort -k 6)"
	fi
elif $ORDENAMIENTO_SESIONES; then	
	if $REVERTIR; then
		TABLA="$(echo "$TABLA" | sort -k 2 -r)"
	else
		TABLA="$(echo "$TABLA" | sort -k 2)"
	fi
fi

# Se imprime la tabla con los datos de los procesos.
if $PROCESOS; then
	printf "\n%-20s %-20s %-20s %-20s %-20s %-20s %-20s" "SESION" "PGID" "PID" "USUARIO" "TTY" "%MEMORIA" "CMD"
	echo "$TABLA"
else
	printf "\n%-20s %-20s %-20s %-20s %-20s %-20s %-20s" "SESION" "PROCESOS" "MEMORIA" "LIDER" "USUARIO" "TTY" "CMD"
	echo "$TABLA"
fi
