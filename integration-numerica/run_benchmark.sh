#!/bin/sh

# --- Configuración ---
# Cantidad fija de iteraciones para todas las pruebas
ITERATIONS=10000000

# Lista de números de hilos con los que se probarán las versiones paralelas
THREADS_TO_TEST="1 4 8 16 32"

# Lista de ejecutables a probar (sin la extensión .cc)
# El script asumirá que se compilan a archivos con el mismo nombre base
EXECUTABLES="without-threads with-threads-no-secure with-threads-secure with-threads-mutex"

# --- Fin de la Configuración ---

echo "=========================================="
echo "  Iniciando Benchmark de Estimación de π"
echo "=========================================="
echo "Iteraciones fijas: $ITERATIONS"
echo "Hilos a probar: $THREADS_TO_TEST"
echo ""

# 1. Compilar todos los archivos fuente
echo "--> Paso 1: Compilando archivos fuente..."
for source_file in ${EXECUTABLES}; do
  echo "Compilando ${source_file}.cc..."
  # Usamos g++ con el estándar C++17 y enlazamos pthread para los hilos
  g++ -std=c++17 -O2 -pthread ${source_file}.cc -o ${source_file}
  if [ $? -ne 0 ]; then
    echo "❌ Error al compilar ${source_file}.cc. Abortando."
    exit 1
  fi
done
echo "✅ Compilación finalizada."
echo ""

# 2. Ejecutar las pruebas
echo "--> Paso 2: Ejecutando pruebas..."
echo ""

# Bucle principal sobre cada ejecutable
for exe in ${EXECUTABLES}; do
  echo "================================================"
  echo "Probando ejecutable: ${exe}"
  echo "================================================"

  # Caso especial para el ejecutable sin hilos
  if [ "${exe}" = "without-threads" ]; then
    echo "Ejecutando (versión secuencial, sin hilos):"
    echo "Comando: ./${exe} ${ITERATIONS}"
    ./${exe} ${ITERATIONS}
    echo ""
  else
    # Bucle para las versiones con hilos
    for num_threads in ${THREADS_TO_TEST}; do
      echo "Ejecutando con ${num_threads} hilo(s):"
      echo "Comando: ./${exe} ${ITERATIONS} ${num_threads}"
      ./${exe} ${ITERATIONS} ${num_threads}
      echo ""
    done
  fi
done

echo "=========================================="
echo "  Benchmark finalizado."
echo "=========================================="

# 3. (Opcional) Limpiar los ejecutables generados
# Descomenta las siguientes líneas si quieres que el script se encargue de limpiar
echo "--> Paso 3: Limpiando ejecutables..."
rm -f ${EXECUTABLES}
echo "✅ Limpieza finalizada."
