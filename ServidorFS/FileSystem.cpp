/**
 * @author Brandon Mora Umaña - C15179
 * @author Diego Fernández Montealegre - C02946
 * @author Ulises Fonseca Hurtado - C03046
 * @author Allison Foster Mc Leod - B22611
 */

#include "FileSystem.hpp"

// Public

FS::FS() {
  Unidad = new char *[MAX_BLOQUE];
  for (int i = 0; i < MAX_BLOQUE; i++) {
    Unidad[i] = new char[MAX_CHAR_POR_BLOQUE];
  }
  FAT = new int[MAX_BLOQUE];
  for (int i = 0; i < MAX_BLOQUE; i++) {
    FAT[i] = -BLOQUE_VACIO;
  }
  raiz.entradas = new entradaDirectorio[MAX_BLOQUE];
};

FS::~FS() {
  for (int i = 0; i < MAX_BLOQUE; i++) {
    delete[] Unidad[i];
  }
  delete[] Unidad;
  delete[] FAT;
  delete[] raiz.entradas;
};

/*
 * Crea un archivo siguiendo los siguientes pasos:
 * 1. Busca la entrada por coincidencias. Si existe, retorna error
 * 2. Busca un bloque vacio. Si no existe, retorna error
 * 3. Crea la entrada en el directorio
 * 4. Crea la entrada en la FAT
 * Es un metodo que solamente llama a otros metodos auxiliares
 */
int FS::crear(std::string nombre, std::string usuario) {
  int errorCrear = EXIT_SUCCESS;
  int bloqueAsignado = -BLOQUE_VACIO;
  Directorio *directorioActual = &raiz; // cambiar para manejar path
  entradaDirectorio *nuevoArchivo = &raiz.entradas[raiz.cantidadEntradas];
  bool archivoExiste = buscarEntrada(nombre, directorioActual) != nullptr;

  // 1. Verifica que el archivo no exista
  if (!archivoExiste) {
    // 2. Busca un bloque vacio
    bloqueAsignado = buscarBloqueVacio();
    if (bloqueAsignado != -ESPACIO_LLENO) {
      // 3. Agrega una entrada al directorio actual
      errorCrear =
          agregarADirectorio(crearEntrada(nombre, usuario), directorioActual);
      // 4. Crea una entrada en la FAT
      if (errorCrear != -ESPACIO_LLENO) {
        errorCrear = crearEntradaFAT(nuevoArchivo, bloqueAsignado);
      }
    } else {
      errorCrear = -ESPACIO_LLENO;
    }
  } else {
    errorCrear = -ARCHIVO_YA_EXISTE;
  }
  return errorCrear;
};

int FS::borrar(std::string nombre, int modo) {
  int error = EXIT_SUCCESS;
  Directorio *directorioActual = &raiz;
  entradaDirectorio *archivo = buscarEntrada(nombre, directorioActual);
  error = borrarEntradaFAT(archivo, modo);
  error = borrarEntrada(archivo);
  return error;
};

int FS::agregar(std::string nombre, char *caracteres) {
  int bloqueAEscribir = -BLOQUE_VACIO;
  int errorAgregar = EXIT_SUCCESS;
  char *caracteresAEscribir = caracteres;
  bool finalEscritura = false;

  // buscar archivo en directorio
  entradaDirectorio *entrada = buscarEntrada(nombre, &raiz);
  if (entrada != nullptr) {
    // actualizar FAT
    bloqueAEscribir = buscarBloqueArchivo(entrada->bloque);
    // TODO(us): Manejar error en buscarBloqueArchivo
    while (!finalEscritura) {
      // actualizarFATAEntrada no realiza ningun cambio si se le llama con el
      // bloque final de un archivo
      caracteresAEscribir =
          escribirEnUnidad(bloqueAEscribir, caracteresAEscribir);
      finalEscritura = caracteresAEscribir == nullptr;
      if (!finalEscritura) {
        bloqueAEscribir = buscarBloqueVacio();
        if (bloqueAEscribir == -ESPACIO_LLENO) {
          finalEscritura = true;
          errorAgregar = -ESPACIO_LLENO;
        } else {
          errorAgregar = actualizarEntradaFAT(entrada, bloqueAEscribir);
        }
      }
    }
  } else {
    errorAgregar = -ERROR_ARCHIVO;
  }

  return errorAgregar;
};

char FS::leer(std::string usuario, std::string nombre, int posicion) {
  entradaDirectorio *archivoExiste = buscarEntrada(nombre, &raiz);
  int ubicacionBloque = 0;  // Numero de bloque en el que se encuentra
  int posicionEnBloque = 0; // offset desde el inicio del bloque
  int bloqueAEscribir = 0;  // Bloque ya encontrado en la FAT
  char caracter = '\0';

  // TODO (Manejar permisos)
  if (archivoExiste != nullptr) {
    ubicacionBloque = posicion / MAX_CHAR_POR_BLOQUE;
    posicionEnBloque = posicion % MAX_CHAR_POR_BLOQUE;
    bloqueAEscribir =
        buscarBloqueArchivo(archivoExiste->bloque, ubicacionBloque);
    caracter = Unidad[bloqueAEscribir][posicionEnBloque];
  }
  return caracter;
};

int FS::escribir(char *caracteres, std::string nombre, std::string user) {
  int error = EXIT_SUCCESS;
  bool finalEscritura = false;
  bool finDeArchivo = false;
  entradaDirectorio *archivoExiste = buscarEntrada(nombre, &raiz);

  int cursor = 0;

  if (archivoExiste != nullptr) {
    int bloqueActual = archivoExiste->bloque;
    // Mientras no se haya terminado de escribir y no se haya llegado al final
    // del bloque
    while (finalEscritura == false) {
      // Se obtiene la direccion de memoria en la que se debe escribir
      char *espacioAEscribir = &Unidad[bloqueActual][cursor];

      // Se escribe el caracter
      *espacioAEscribir = *caracteres;

      // Se encontro un fin de archivo
      if (*espacioAEscribir == '\0') {
        finDeArchivo = true;
      }

      // Se incrementa el cursor para escribir el siguiente caracter
      cursor++;

      // Se incrementa el puntero para escribir el siguiente caracter
      caracteres++;
      // Si el cursor es del tamaño del bloque, se pasa a un siguiente bloque
      if (cursor == MAX_CHAR_POR_BLOQUE) {
        // Si llegamos aqui, y el siguiente bloque FAT es negativo, podemos
        // llamar a agregar
        if (FAT[bloqueActual] > 0) {
          cursor = 0;
          bloqueActual = FAT[bloqueActual];
        } else {
          error = this->agregar(nombre, caracteres);
          *caracteres = '\0';
        }
      }
      // Si se escribieron todos los caracteres se termina el ciclo
      if (*caracteres == '\0') {
        finalEscritura = true;
        // Si se encontro un fin de archivo antes, se pone uno nuevo
        if (finDeArchivo == true) {
          *espacioAEscribir = '\0';
        }
      }
    }
  } else {
    error = EXIT_FAILURE;
  }
  return error;
}

void FS::imprimirUnidad() {
  // Directorio
  std::cout << "Directorio:" << std::endl;
  for (int i = 0; i < raiz.cantidadEntradas; i++) {
    if (raiz.entradas[i].bloque != -BLOQUE_VACIO) {
      std::cout << "\"" << raiz.entradas[i].nombre << "\"\t"
                << raiz.entradas[i].bloque << "\t" << raiz.entradas[i].permisos
                << std::endl;
    }
  }
  // FAT
  std::cout << std::endl << "FAT" << std::endl;
  for (int i = 0; i < MAX_BLOQUE; i++) {
    std::cout << FAT[i] << "\t";
  }
  std::cout << std::endl << std::endl;
  for (int i = 0; i < MAX_BLOQUE; i++) {
    std::cout << i << "\t";
  }
  std::cout << std::endl << std::endl;
  // Unidad
  std::cout << "UNIDAD" << std::endl;
  for (int i = 0; i < MAX_BLOQUE; i++) {
    for (int j = 0; j < MAX_CHAR_POR_BLOQUE; j++) {
      std::cout << Unidad[i][j] << " ";
    }
    std::cout << std::endl;
  }
};

entradaDirectorio::entradaDirectorio(std::string nombre, int bloque,
                                     time_t fecha, char *permisos) {
  this->bloque = bloque;
  this->nombre = nombre;
  this->fecha = fecha;
  this->permisos = permisos;
};

Directorio::Directorio() {
  entradas = new entradaDirectorio[MAX_BLOQUE];
  cantidadEntradas = 0;
};

entradaDirectorio FS::crearEntrada(std::string nombre, std::string usuario) {
  // Crea una entrada de directorio con el nombre y el usuario dados
  entradaDirectorio entrada(nombre);
  // Devuelve la entrada de directorio creada
  return entrada;
};
int FS::crearEntradaFAT(entradaDirectorio *entrada, int bloqueAEscribir) {
  int error = EXIT_SUCCESS;
  // Si no hay error y el bloque esta vacio, entonces escribimos en el.
  if (entrada != nullptr && FAT[bloqueAEscribir] == -BLOQUE_VACIO) {
    // Aclaramos que no hemos aun escrito en este bloque.
    FAT[bloqueAEscribir] = -ASIGNADO_NO_USADO;
    entrada->bloque = bloqueAEscribir;
  } else {
    error = -ERROR_FAT;
  }
  return error;
};

// Borra una entrada de un directorio
int FS::borrarEntrada(entradaDirectorio *entradaABorrar) {
  // Declara una variable para almacenar el error
  int error = EXIT_SUCCESS;
  // Chequea que la entrada sea válida
  if (entradaABorrar != nullptr) {
    // Si es válida, se borra
    entradaABorrar->bloque = BLOQUE_VACIO;
    entradaABorrar->nombre = "";
    entradaABorrar->fecha = time(0);
  } else {
    // En caso de no ser válida, se guarda el error correspondiente
    error = ERROR_ARCHIVO;
  }
  // Devuelve el error
  return error;
};

int FS::borrarEntradaFAT(entradaDirectorio *archivo, int modo) {
  int error = EXIT_SUCCESS;
  int bloque = archivo->bloque;
  int bloqueSiguiente = 0;
  int limiteDeSaltos = 0;
  // Mientras el bloque no sea negativo y no se haya saltado más de MAX_BLOQUE
  // bloques
  while (bloque >= 0 && limiteDeSaltos < MAX_BLOQUE) {
    // Guarda el bloque siguiente en la FAT
    bloqueSiguiente = FAT[bloque];
    // Le asigna -BLOQUE_VACIO al bloque actual
    FAT[bloque] = -BLOQUE_VACIO;
    if (modo == BORRADO_PROFUNDO) {
      // Si se quiere borrar el archivo de manera profunda, se le asigna un
      // valor nulo
      char *bloqueAEscribir = Unidad[bloque];
      for (int i = 0; i < MAX_CHAR_POR_BLOQUE; i++) {
        bloqueAEscribir[i] = '\0';
      }
    }
    // Se actualiza el bloque
    bloque = bloqueSiguiente;
    // Se actualiza el límite de saltos
    limiteDeSaltos++;
  }
  return error;
};

int FS::actualizarEntradaFAT(entradaDirectorio *entrada, int bloqueAEscribir) {
  // agregar caracter a la entrada
  int ultimoBloque = -FIN_ARCHIVO;
  int error = EXIT_SUCCESS;

  if (entrada != nullptr) {
    FAT[bloqueAEscribir] = -FIN_ARCHIVO;

    if (entrada->bloque != bloqueAEscribir) {
      ultimoBloque = buscarBloqueArchivo(entrada->bloque);
      FAT[ultimoBloque] = bloqueAEscribir;
    }
  } else {
    error = -ERROR_FAT;
  }

  return error;
}

char *FS::escribirEnUnidad(int bloqueAEscribir, char *caracteresAEscribir,
                           int posicionAEscribir) {
  char *caracteresRestantes = nullptr;
  int cursor = posicionAEscribir;
  bool finDeArchivo;

  // Si se tiene el valor por defecto, buscamos el fin de archivo
  if (posicionAEscribir <= 0) {
    cursor = 0;
    char caracterEnUnidad = Unidad[bloqueAEscribir][cursor];
    while (caracterEnUnidad != '\0' && cursor < MAX_CHAR_POR_BLOQUE) {
      cursor++;
      caracterEnUnidad = Unidad[bloqueAEscribir][cursor];
    }
  }

  // Mientras no se haya terminado de escribir y no se haya llegado al final
  // del bloque
  while (cursor < MAX_CHAR_POR_BLOQUE && *caracteresAEscribir != '\0') {
    // Se obtiene la direccion de memoria en la que se debe escribir
    char *espacioAEscribir = &Unidad[bloqueAEscribir][cursor];

    // Se debe manejar la condicion para definir si se debe poner un fin de
    // archivo al terminar
    if (*espacioAEscribir == '\0') {
      finDeArchivo = true;
    }

    // Se escribe el caracter
    *espacioAEscribir = *caracteresAEscribir;

    // Se incrementa el cursor para escribir el siguiente caracter
    cursor++;

    // Se incrementa el puntero para escribir el siguiente caracter
    caracteresAEscribir++;
  }

  // Si se termino de escribir y se tiene que poner un fin de archivo
  if (cursor != MAX_CHAR_POR_BLOQUE && finDeArchivo) {
    // Se pone el fin de archivo
    Unidad[bloqueAEscribir][cursor] = '\0';
    caracteresRestantes = nullptr;
  } else {
    // Se devuelven los caracteres restantes
    caracteresRestantes = caracteresAEscribir;
  }

  return caracteresRestantes;
};

int FS::buscarBloqueArchivo(int indiceFAT, int NumeroDePasos) {
  int indiceBloque = indiceFAT;
  while (NumeroDePasos > 0 && FAT[indiceBloque] != -FIN_ARCHIVO &&
         FAT[indiceBloque] != -ASIGNADO_NO_USADO) {
    indiceBloque = FAT[indiceBloque];
    NumeroDePasos--;
  }
  return indiceBloque;
}

entradaDirectorio *FS::buscarEntrada(std::string nombreABuscar,
                                     Directorio *directorio) {
  // Recorrer todas las entradas del directorio.
  int cantidadDeEntradas = directorio->cantidadEntradas;
  entradaDirectorio *entrada = nullptr;
  int cantidadRecorrida = 0; // Cantidad de entradas no vacias recorridas
  int indiceEntrada = 0;     // Indice para iterar sobre las entradas

  // Mientras no se haya recorrido todas las entradas y no se haya encontrado
  // la entrada
  while (cantidadRecorrida < cantidadDeEntradas && entrada == nullptr &&
         indiceEntrada < MAX_BLOQUE) {
    // Si la entrada no es vacia
    if (directorio->entradas[indiceEntrada].nombre != "") {
      // Si el nombre de la entrada es igual al nombre a buscar
      if (directorio->entradas[indiceEntrada].nombre == nombreABuscar) {
        // Se guarda la entrada
        entrada = &directorio->entradas[indiceEntrada];
      }
      // Se incrementa la cantidad de entradas no vacias recorridas
      cantidadRecorrida++;
    }
    // Se incrementa el indice de entrada
    indiceEntrada++;
  }

  return entrada;
};

// Busca un bloque vacio en FAT
int FS::buscarBloqueVacio() {
  // Asume que no hay espacio
  int bloqueAEscribir = -ESPACIO_LLENO;
  // Indice para recorrer FAT
  int indice = 0;
  // Asume que no se ha encontrado un bloque vacio
  bool encontrado = false;
  // Recorre FAT
  while (indice < MAX_BLOQUE && !encontrado) {
    // Si encuentra un bloque vacio
    if (FAT[indice] == -BLOQUE_VACIO) {
      // Guarda el indice
      bloqueAEscribir = indice;
      // Indica que se encontro un bloque vacio
      encontrado = true;
    }
    // Incrementa indice
    indice++;
  }
  // Necesario para evitar errores con basura despues de un borrado
  Unidad[bloqueAEscribir][0] = '\0';
  // Regresa el indice del bloque
  return bloqueAEscribir;
};

int FS::agregarADirectorio(entradaDirectorio nuevoArchivo,
                           Directorio *directorio) {
  // agregar entrada al directorio
  int error = EXIT_SUCCESS;
  // buscar una entrada vacía en el directorio
  entradaDirectorio *entradaAUsar = nullptr;
  entradaDirectorio *entradaParaIteracion = nullptr;
  int indice = 0;

  while (entradaAUsar == nullptr && indice < MAX_BLOQUE) {
    entradaParaIteracion = &directorio->entradas[indice];
    if (entradaParaIteracion->bloque == -BLOQUE_VACIO) {
      entradaAUsar = entradaParaIteracion;
    }
    indice++;
  }
  // si encontramos una entrada vacía, la usamos
  if (entradaAUsar != nullptr) {
    *entradaAUsar = nuevoArchivo;
    directorio->cantidadEntradas++;
  } else {
    // si no encontramos una entrada vacía, no podemos agregar el archivo
    error = -ESPACIO_LLENO;
  }
  return error;
}