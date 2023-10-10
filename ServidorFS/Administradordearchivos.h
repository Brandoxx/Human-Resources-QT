#ifndef ADMINISTRADORDEARCHIVOS_H
#define ADMINISTRADORDEARCHIVOS_H

#include "FileSystem.hpp"
#include <iostream>
#include <sstream>
#include <vector>

class AdministradorDeArchivos
{
private:
    struct Usuario {
        std::string nombre;
        std::string contrasena;
        int nivelDeAcceso;
    };

    enum Cambios {
        NO_HAY_CAMBIOS = 0,
        HAY_CAMBIOS,
    };

    enum Niveles_de_acceso {
        EMPLEADO = 1,
        SUPERVISOR,
        RRHH,
        ADMIN_USUARIOS,
        ADMIN_CONFIG,
        DEBUG,
    };
    // Variable importante para mantener actualizada la lista de Usuarios
    int cambios_hechos = HAY_CAMBIOS; // Se inicializa en 1 para que se actualice la lista de Usuarios al inicio
    int inicio_de_datos = HAY_CAMBIOS;
    FS *fs;
    std::vector<Usuario> Usuarios;

    /**
   * Busca un Usuario en el vector de Usuarios
   * @param username Nombre de Usuario
   * @return El indice del Usuario en el vector, -Usuario_NO_ENCONTRADO si no se
   * encuentra
   **/
    int buscarUsuario(std::string username);

    /**
   * Actualiza los datos de los Usuarios en el sistema
   * @param datos String donde se buscara el Usuario
   * @return true si se actualizo correctamente, false si no
   **/
    bool leerDatosUsuarios(std::string archivo = "Usuario.dat");

    /**
   * Chequea si la contrasena es correcta
   * @param indice_Usuario Indice del Usuario en el vector de Usuarios
   * @param contrasena Contrasena a verificar
   * @return nivel de acceso del Usuario si la contrasena es correcta,
   * CONTRASENA_INCORRECTA si no
   **/
    int verificarContrasena(int indice_Usuario, std::string contrasena);

    bool actualizarDatosUsuarios(std::string archivo = "Usuario.dat");


public:
    enum error_codes {
        USUARIO_NO_ENCONTRADO = 1,
        USUARIO_YA_EXISTE,
        CONTRASENA_INCORRECTA,
    };

    AdministradorDeArchivos();

   /**
   * @brief Lee un archivo completo
   * @param nombreDeArchivo Nombre del archivo a leer
   * @param UsuarioFS Usuario del FileSystem, para revision de permisos
   * @return El contenido del archivo
   **/
    std::string leerArchivoCompleto(std::string nombreDeArchivo, std::string UsuarioFS = "Grupazo");

    /**
     * @brief Escribe desde el inicio del archivo
     * @param nombreDeArchivo Nombre del archivo a escribir
     * @param datos Datos a escribir
     * @param UsuarioFS Usuario del FileSystem, para revision de permisos
     * @return true si se escribio correctamente, false si no
    */
    int escribirArchivo(std::string nombreDeArchivo, std::string datos, std::string UsuarioFS = "Grupazo");

   /**
   * @brief Auntentica un Usuario en el sistema
   * @param username Nombre de Usuario
   * @param contrasena Contrasena del Usuario
   * @return El nivel de acceso del Usuario, USUARIO_NO_ENCONTRADO si no se
   * encuentra CONTRASENA_INCORRECTA si la contrasena es incorrecta
   **/
    int autenticarUsuario(std::string username, std::string contrasena, std::string nombreArchivo = "Usuario.dat");

   /**
   * @brief Crea un archivo en el sistema
   * @param nombreDeArchivo Nombre del archivo a crear
   * @param UsuarioFS Usuario del FileSystem, para revision de permisos
   * @return true si se creo correctamente, false si no
   **/
    int crearArchivo(std::string nombreDeArchivo, std::string UsuarioFS = "Grupazo");

   /**
   * @brief Borra un archivo en el sistema
   * @param nombreDeArchivo Nombre del archivo a borrar
   * @param UsuarioFS Usuario del FileSystem, para revision de permisos
   * @return true si se borro correctamente, false si no
   **/
    bool borrarArchivo(std::string nombreDeArchivo, std::string UsuarioFS = "Grupazo");

   /**
   * @brief Agrega datos a un archivo en el sistema
   * @param nombreDeArchivo Nombre del archivo a agregar datos
   * @param datos Datos a agregar
   * @param UsuarioFS Usuario del FileSystem, para revision de permisos
   * @return el codigo de error proporcionado por el FS
   **/
    bool agregarDatosArchivo(std::string nombreDeArchivo, std::string datos, std::string UsuarioFS = "Grupazo");

   /**
   * @brief Crea un Usuario en el sistema
   * @param username Nombre de Usuario
   * @param contrasena Contrasena del Usuario
   * @param nivelDeAcceso Nivel de acceso del Usuario
   * @return true si se creo correctamente, false si no
   **/
    int crearUsuario(std::string username, std::string contrasena, int nivelDeAcceso);

    /**
     * @brief Borra un Usuario en el sistema
     * @param username Nombre de Usuario
     * @return true si se borro correctamente, false si no
     * */
    int borrarUsuario(std::string username, std::string nombreArchivo = "Usuario.dat");
};

#endif // ADMINISTRADORDEARCHIVOS_H
