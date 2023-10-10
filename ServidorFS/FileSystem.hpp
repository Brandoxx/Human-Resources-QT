/**
 * @author Brandon Mora Umaña - C15179
 * @author Diego Fernández Montealegre - C02946
 * @author Ulises Fonseca Hurtado - C03046
 * @author Allison Foster Mc Leod - B22611
 */

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <iostream>

#include <cmath>

  const int MAX_BYTES = 1024;
  const int MAX_CHAR_POR_BLOQUE = 64;
  const int MAX_BLOQUE = MAX_BYTES / MAX_CHAR_POR_BLOQUE;
  const int MAX_IMPRIMIR = sqrt(MAX_BYTES);

  struct entradaDirectorio {
    entradaDirectorio(std::string nombre = "", int bloque = -2,
                      time_t fecha = time(0), char* permisos = "715");
    int bloque;
    std::string nombre;
    time_t fecha;
    char* permisos;
    std::string propietario;
  };

  /*
   * Estructura del directorio
   * @param entradas: arreglo de entradas del directorio
   * @param cantidadEntradas: cantidad de entradas en el directorio
   */
  struct Directorio {
    Directorio();
    entradaDirectorio *entradas;
    int cantidadEntradas;
  };


class FS {
public:
  const int BORRADO_RAPIDO = 0;
  const int BORRADO_PROFUNDO = 1;

  enum error_codes {
    FIN_ARCHIVO = 1,
    BLOQUE_VACIO,
    ASIGNADO_NO_USADO,
    ERROR_FAT,
    ESPACIO_LLENO,
    ERROR_ARCHIVO,
    ARCHIVO_YA_EXISTE,
    ERROR_LECTURA
  };

  FS();
  ~FS();

  /*
   * Crea un archivo en el directorio
   * @param nombre: nombre del archivo a crear
   * @param usuario: usuario propietario del archivo (identificado en el FileSystem)
   * @return: 0 si se creo correctamente, error_code si hubo fallos
   */
  int crear(std::string nombre, std::string usuario);

  /*
   * Borra un archivo del directorio
   * @param nombre: nombre del archivo a borrar
   * @param modo: modo de borrado (borrado rapido o profundo).
   * @return: 0 si se borro correctamente, ERROR_ARCHIVO si no se pudo borrar
   */
  int borrar(std::string nombre, int modo);

  /*
   * Agrega un caracter a un archivo
   * @param nombre: nombre del archivo
   * @param caracter: caracter a agregar
   * @return: 0 si se agrego correctamente, 1 si no se encontro el archivo
   *         2 si no hay bloques vacios disponibles
   */
  int agregar(std::string nombre, char *caracteres);

  /*
   * Lee un caracter de un archivo
   * @param usuario: usuario auntenticado en el FileSystem
   * @param nombre: nombre del archivo
   * @param posicion: posicion del caracter a leer
   * @return: el caracter "posicion" del archivo. Caracter nulo si "posicion"
   * esta fuera de rango.
   */
  char leer(std::string usuario, std::string nombre, int posicion);

  /**
   * Escribe encima de los elementos de un archivo
   * @param caracteres: caracteres a escribir en el archivo
   * @param nombre: nombre del archivo
   * @param user: nombre del usuario que desea cambiar el archivo
   * @return: retorna 0 si logro escribir con exito, 1 sino
  */
  int escribir(char *caracteres, std::string nombre, std::string user = "");

  void imprimirUnidad();


private:
  char **Unidad;
  Directorio raiz; // si
  int *FAT;
  
  /*
   * Crea una entrada para el directorio
   * @param nombre: nombre del archivo
   * @param usuario: usuario identificado en el FileSystem
   * @return: entrada creada
   */
  entradaDirectorio crearEntrada(std::string nombre, std::string usuario);

  /*
   * Borra una entrada
   * @param entradaABorrar: entrada a borrar
   * @return: 0 si se borro correctamente, ERROR_ARCHIVO si no se pudo borrar
   */
  int borrarEntrada(entradaDirectorio *entradaABorrar);

   /*
   * Crea una entrada en la FAT
   * @param entrada: entrada del archivo a crear
   * @param bloqueAEscribir: bloque vacio a agregar al archivo
   * @return: 0 si se creo correctamente, ERROR_FAT si no se pudo crear
   */
  int crearEntradaFAT(entradaDirectorio *entrada, int bloqueAEscribir);

  /*
   * Borra una entrada de la FAT
   * @param archivo: entrada del archivo a borrar
   * @param modo: modo de borrado (borrado rapido o profundo)
   * @return: 0 si se borro correctamente, ERROR_FAT si no se pudo borrar
   */
  int borrarEntradaFAT(entradaDirectorio *archivo, int modo);

  /*
   * Actualiza la FAT para agregar un nuevo bloque al archivo. Escribe en el
   * error si no se puede actualizar
   * @param entrada: entrada del archivo a actualizar
   * @param bloqueAEscribir: bloque vacio a agregar al archivo
   * @return: 0 si se actualizo correctamente, ERROR_FAT si no se pudo actualizar
   */
  int actualizarEntradaFAT(entradaDirectorio *entrada, int bloqueAEscribir);

   /*
   * Busca el bloque "NumeroDePasos" del archivo. Asume encontrar el ultimo bloque
   * @param indiceFAT: indice del primer bloque del archivo
   * @param NumeroDePasos: OPCIONAl (Asume encontrar el final del archivo). Pasos
   * a dar en los bloques del archivo. Si los pasos son mayores al tamaño del
   * archivo, se asume que se quiere encontrar el final del archivo.
   * @return: indice del bloque numero NumeroDePasos del archivo
   */
  int buscarBloqueArchivo(int indiceFAT, int NumeroDePasos = MAX_BLOQUE);

  /*
   * Escribe caracteres en un bloque de la unidad
   * @param bloqueAEscribir: bloque en el que se escribiran los caracteres
   * @param caracteresAEscribir: caracteres a escribir
   * @param posicionAEscribir: OPCIONAL (posicion en la que se escribiran los
   * caracteres. Si no se especifica, se asume que se escribiran al final del
   * archivo)
   * @return: caracteres restantes por escribir. nullptr si se escribieron todos
   */
  char *escribirEnUnidad(int bloqueAEscribir, char *caracteresAEscribir,
                         int posicionAEscribir = -FIN_ARCHIVO);

  /*
   * Busca una entrada en el directorio
   * @param nombreABuscar: nombre del archivo a buscar
   * @param directorio: directorio en el que se buscara el archivo
   * @return: entrada del archivo encontrado. nullptr si no se encontro
   */
  entradaDirectorio *buscarEntrada(std::string nombreABuscar,
                                   Directorio *directorio);

  /*
   * Busca un bloque vacio en la FAT
   * @return: indice del bloque vacio. ESPACIO_LLENO si no hay bloques vacios
   */
  int buscarBloqueVacio();

  /*
   * Agrega una entrada al directorio
   * @param nuevoArchivo: entrada a agregar
   * @param directorio: directorio al que se le agregara la entrada
   * @return: 0 si se agrego correctamente, ESPACIO_LLENO si no se pudo agregar
   */
  int agregarADirectorio(entradaDirectorio nuevoArchivo,
                         Directorio *directorio);

};

#endif 
