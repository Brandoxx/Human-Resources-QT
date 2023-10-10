#include <cmath>
#include <iostream>

const int MAX_CHAR = 100;
const int MAX_BLOQUE = sqrt(MAX_CHAR);
const int MAX_IMPRIMIR = 10;

const int BORRADO_RAPIDO = 0;
const int BORRADO_PROFUNDO = 1;

// RESTRICCIONES
// 1. bloques son de 1 caracter
// 2. Unidad es una matriz de caracteres
// 3. Tamaño máximo 100 caracteres, por el momento
// 4. Usando FAT
// 5. Usando un struct para el directorio (nombre, bloque inicial, fecha, por el
// momento)

struct entradaDirectorio {
  entradaDirectorio(std::string nombre = "", int bloque = -2,
                    time_t fecha = time(0)) {
    this->bloque = bloque;
    this->nombre = nombre;
    this->fecha = fecha;
  }
  int bloque;
  std::string nombre;
  time_t fecha;
};

/*
 * Estructura del directorio
 * @param entradas: arreglo de entradas del directorio
 * @param cantidadEntradas: cantidad de entradas en el directorio
 */
struct Directorio {
  Directorio() {
    entradas = new entradaDirectorio[MAX_CHAR];
    cantidadEntradas = 0;
  }
  entradaDirectorio *entradas;
  int cantidadEntradas;
};

class FS {
  const int BLOQUE_VACIO = -2;
  const int FIN_ARCHIVO = -1;
  const int ASIGNADO_NO_USADO = -3;
  const int ERROR_FAT = -4;
  const int ESPACIO_LLENO = -5;
  const int ERROR_ARCHIVO = -6;
  const int ARCHIVO_YA_EXISTE = -7;

  // vars
 private:
  char **Unidad;
  Directorio raiz;  // si
  int *FAT;

  // helpers

  /*
   * Crea una entrada para el directorio
   * @param nombre: nombre del archivo
   * @return: entrada creada
   */
  entradaDirectorio crearEntrada(std::string nombre) {
    entradaDirectorio entrada(nombre);
    return entrada;
  }
  /*
    * Borra una entrada del directorio
    * @param entradaABorrar: entrada a borrar
    * @param directorio: directorio en el que se borrara la entrada
    * @return: 0 si se borro correctamente, ERROR_ARCHIVO si no se pudo borrar
  */
  int borrarEntrada(entradaDirectorio* entradaABorrar) {
    int error = EXIT_SUCCESS;
    if (entradaABorrar != nullptr) {
      entradaABorrar->bloque = BLOQUE_VACIO;
      entradaABorrar->nombre = "";
      entradaABorrar->fecha = time(0);
    } else {
      error = ERROR_ARCHIVO;
    }
    return error;
  }

  /*
   * Agrega una entrada al directorio
   * @param nuevoArchivo: entrada a agregar
   * @param directorio: directorio al que se le agregara la entrada
   * @return: 0 si se agrego correctamente, ESPACIO_LLENO si no se pudo agregar
   */
  int agregarADirectorio(entradaDirectorio nuevoArchivo,
                         Directorio *directorio) {
    // agregar entrada al directorio
    int error = EXIT_SUCCESS;
    entradaDirectorio* entradaAUsar = nullptr;
    entradaDirectorio* entradaIteracion = nullptr;
    int indice = 0;

    while (entradaAUsar == nullptr && indice < MAX_CHAR) {
      entradaIteracion = &directorio->entradas[indice];
      if (entradaIteracion->bloque == BLOQUE_VACIO) {
        entradaAUsar = entradaIteracion;
      }
      indice++;
    }
    if (entradaAUsar != nullptr) {
      *entradaAUsar = nuevoArchivo;
      directorio->cantidadEntradas++;
    } else {
      error = ESPACIO_LLENO;
    }
    return error;
  }

  /*
   * Busca un bloque vacio en la FAT
   * @return: indice del bloque vacio. -1 si no hay bloques vacios
   */
  int buscarBloqueVacio() {
    int bloqueAEscribir = ESPACIO_LLENO;  // Asume que no hay espacio
    int indice = 0;
    bool encontrado = false;
    while (indice < MAX_CHAR && !encontrado) {
      if (FAT[indice] == BLOQUE_VACIO) {
        bloqueAEscribir = indice;
        encontrado = true;
      }
      indice++;
    }
    return bloqueAEscribir;
  }

  /*
    * Busca una entrada en el directorio
    * @param nombreABuscar: nombre del archivo a buscar
    * @param directorio: directorio en el que se buscara el archivo
    * @return: entrada del archivo encontrado. nullptr si no se encontro
  */
  entradaDirectorio *buscarEntrada(std::string nombreABuscar,
                                   Directorio *directorio) {
    int cantidadEntradas = directorio->cantidadEntradas;
    entradaDirectorio *entradaEncontrada = nullptr;
    std::string nombreEntrada = "";
    while (cantidadEntradas >= 0 && entradaEncontrada == nullptr) {
      nombreEntrada = directorio->entradas[cantidadEntradas].nombre;
      if (nombreEntrada == nombreABuscar) {
        entradaEncontrada = &directorio->entradas[cantidadEntradas];
      }
      cantidadEntradas--;
    }
    return entradaEncontrada;
  }

  /*
     * Busca el bloque en el que se encuentra el archivo
     * @param indiceFAT: indice del primer bloque del archivo
     * @param NumeroDePasos: OPCIONAl (Asume encontrar el final del archivo. Pasos
     * a dar en los bloques del archivo. Si los pasos son mayores al tamaño del
     * archivo, se asume que se quiere encontrar el final del archivo)
     * @return: indice del bloque numero NumeroDePasos del archivo
   */
  int buscarIndiceBloque(int indiceFAT, int NumeroDePasos = MAX_CHAR) {
    int indiceBloque = indiceFAT;
    while (NumeroDePasos > 0 && FAT[indiceBloque] != FIN_ARCHIVO) {
      indiceBloque = FAT[indiceBloque];
      NumeroDePasos--;
    }
    return indiceBloque;
  }

  /*
   * Crea una entrada en la FAT
   * @param entrada: entrada del archivo a crear
   * @param bloqueAEscribir: bloque vacio a agregar al archivo
   * @return: 0 si se creo correctamente, ERROR_FAT si no se pudo crear
   */
  int crearEntradaFAT(entradaDirectorio *entrada, int bloqueAEscribir) {
    int error = EXIT_SUCCESS;
    if (entrada != nullptr && FAT[bloqueAEscribir] == BLOQUE_VACIO) {
      FAT[bloqueAEscribir] = ASIGNADO_NO_USADO;
      entrada->bloque = bloqueAEscribir;
    } else {
      error = ERROR_FAT;
    }
    return error;
  }

  /*
    * Actualiza la FAT para agregar un nuevo bloque al archivo. Escribe en el
    * error si no se puede actualizar
    * @param entrada: entrada del archivo a actualizar
    * @param bloqueAEscribir: bloque vacio a agregar al archivo
    * @return: 0 si se actualizo correctamente, -3 si no se pudo actualizar
  */
  int actualizarFATAEntrada(entradaDirectorio* entrada, int bloqueAEscribir) {
    // agregar caracter a la entrada
    int ultimoBloque = FIN_ARCHIVO;
    int error = EXIT_SUCCESS;
    if (entrada != nullptr) {
      FAT[bloqueAEscribir] = FIN_ARCHIVO;
      if (entrada->bloque != bloqueAEscribir) {
        ultimoBloque = buscarIndiceBloque(entrada->bloque);
        FAT[ultimoBloque] = bloqueAEscribir;
      }
    } else {
      error = ERROR_FAT;
    }
    return error;
  }

  /*
    * Borra una entrada de la FAT
    * @param archivo: entrada del archivo a borrar
    * @param modo: modo de borrado (borrado rapido o profundo)
    * @return: 0 si se borro correctamente, ERROR_FAT si no se pudo borrar
  */
  int borraEntradaFAT(entradaDirectorio *archivo, int modo) {
    int error = EXIT_SUCCESS;
    int bloque = archivo->bloque;
    int bloqueSiguiente = 0;
    while (bloque >= 0) {
      bloqueSiguiente = FAT[bloque];
      FAT[bloque] = BLOQUE_VACIO;
      if (modo == BORRADO_PROFUNDO) {
        char* bloqueAEscribir = encontrarBloquedeDatos(bloque);
        *bloqueAEscribir = '\0';
      }
      bloque = bloqueSiguiente;
    }
    return error;
  }

  /*
    * Busca un bloque de datos en la unidad
    * @param indiceAEncontrar: indice del bloque a encontrar
    * @return: bloque encontrado, nullptr si no se encuentra
  */
  char* encontrarBloquedeDatos(int indiceAEncontrar) {
    int fila = 0;
    int columna = 0;
    char* bloqueEncontrado = nullptr;

    fila = indiceAEncontrar / MAX_BLOQUE;
    columna = indiceAEncontrar % MAX_BLOQUE;
    bloqueEncontrado = &Unidad[fila][columna];

    return bloqueEncontrado;
  }

  // methods
 public:
  FS() {
    Unidad = new char *[MAX_BLOQUE];
    for (int i = 0; i < MAX_BLOQUE; i++) {
      Unidad[i] = new char[MAX_BLOQUE];
    }
    FAT = new int[MAX_CHAR];
    for (int i = 0; i < MAX_CHAR; i++) {
      FAT[i] = BLOQUE_VACIO;
    }
    raiz.entradas = new entradaDirectorio[MAX_CHAR];
  };

  /*
   * Crea un archivo en el directorio
   * @param nombre: nombre del archivo a crear
   * @return: 0 si se creo correctamente, -4 si no se pudo crear
   */
  int crear(std::string nombre) {
    int errorCrear = EXIT_SUCCESS;
    int bloqueAsignado = BLOQUE_VACIO;
    Directorio *directorioActual = &raiz;  // cambiar para manejar path
    entradaDirectorio *nuevoArchivo = &raiz.entradas[raiz.cantidadEntradas];
    bool archivoExiste = buscarEntrada(nombre, directorioActual) != nullptr;

    if (!archivoExiste) {
      bloqueAsignado = buscarBloqueVacio();
      if (bloqueAsignado != ESPACIO_LLENO) {
        errorCrear = agregarADirectorio(crearEntrada(nombre), directorioActual);
        if (errorCrear != ESPACIO_LLENO) {
          errorCrear = crearEntradaFAT(nuevoArchivo, bloqueAsignado);
        }
      } else {
        errorCrear = ESPACIO_LLENO;
      }
    } else {
      errorCrear = ARCHIVO_YA_EXISTE;
    }
    return errorCrear;
  };

  /*
    * Borra un archivo del directorio
    * @param nombre: nombre del archivo a borrar
    * @param modo: modo de borrado (borrado rapido o profundo). Por defecto es
    * borrado rapido
    * @return: 0 si se borro correctamente, ERROR_ARCHIVO si no se pudo borrar
  */
  int borrar(std::string nombre, int modo) {
    int error = EXIT_SUCCESS;
    Directorio *directorioActual = &raiz;
    entradaDirectorio *archivo = buscarEntrada(nombre, directorioActual);
    borraEntradaFAT(archivo, modo);
    // TODO(us) borrar entrada del directorio
    error = borrarEntrada(archivo);
    return error;
  }

  /*
    * Agrega un caracter a un archivo
    * @param nombre: nombre del archivo
    * @param caracter: caracter a agregar
    * @return: 0 si se agrego correctamente, 1 si no se encontro el archivo
    *         2 si no hay bloques vacios disponibles
  */
  int agregar(std::string nombre, char caracter) {
    int bloqueAEscribir = BLOQUE_VACIO;
    int errorAgregar = EXIT_SUCCESS;
    char caracterAEscribir = caracter;
    char* bloqueDelaUnidad = nullptr;
    // buscar archivo en directorio
    entradaDirectorio* entrada = buscarEntrada(nombre, &raiz);
    if (entrada != nullptr) { // using c error handling
      // actualizar FAT
      if (FAT[entrada->bloque] != ASIGNADO_NO_USADO) {
        bloqueAEscribir = buscarBloqueVacio();
      } else {
        bloqueAEscribir = entrada->bloque;
      }
      if (bloqueAEscribir != ESPACIO_LLENO) {
        errorAgregar = actualizarFATAEntrada(entrada, bloqueAEscribir);
        if (errorAgregar == EXIT_SUCCESS) {
          // agregar caracter al archivo
          bloqueDelaUnidad = encontrarBloquedeDatos(bloqueAEscribir);
          *bloqueDelaUnidad = caracterAEscribir; 
        } else {
          std::cerr << "No se pudo agregar el caracter" << std::endl;
        }
      } else {
        errorAgregar = ESPACIO_LLENO;
      }
    } else {
      errorAgregar = ERROR_ARCHIVO;
    }
    
    return errorAgregar;
  };

  void imprimirUnidad() {
    // Directorio
    std::cout << "Directorio:" << std::endl;
    for (int i = 0; i < MAX_CHAR; i++) {
      if (raiz.entradas[i].bloque != BLOQUE_VACIO) {
        std::cout << "\"" << raiz.entradas[i].nombre << "\"\t"
                  << raiz.entradas[i].bloque << std::endl;
      }
    }
    // FAT
    std::cout << std::endl << "FAT" << std::endl;
    for (int i = 0; i < MAX_IMPRIMIR; i++) {
      std::cout << FAT[i] << "\t";
    }
    std::cout << std::endl << std::endl;
    for (int i = 0; i < MAX_IMPRIMIR; i++) {
      std::cout << i << "\t";
    }
    std::cout << std::endl << std::endl;
    // Unidad
    std::cout << "UNIDAD" << std::endl;
    for (int i = 0; i < MAX_BLOQUE; i++) {
      for (int j = 0; j < MAX_BLOQUE; j++) {
        std::cout << Unidad[i][j] << " ";
      }
      std::cout << std::endl;
    }
  };
};

int main() {
  FS *fs = new FS();

  fs->crear("a.dat");
  fs->agregar("a.dat", 'a');
  fs->imprimirUnidad();  // imprime unidad, directorio y tabla fat

  fs->crear("b.dat");
  fs->agregar("b.dat", 'b');
  fs->imprimirUnidad();
  fs->agregar("b.dat", 'c');
  fs->imprimirUnidad();
  fs->borrar("b.dat", BORRADO_RAPIDO);
  fs->imprimirUnidad();
}

/*  IMPRIMIR UNIDAD al final

Directorio:
"b.dat"   1
"c.dat"   0


FAT
3 2 x 4 x
0 1 2 3 4 5 6 7


Cla/*

UNIDAD
c b b
c c _
_ _ _

*/
