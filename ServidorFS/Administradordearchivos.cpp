#include "Administradordearchivos.h"

AdministradorDeArchivos::AdministradorDeArchivos()
{
    this->fs = new FS();
}

int AdministradorDeArchivos::buscarUsuario(std::string username) {
    int encontrado = -USUARIO_NO_ENCONTRADO;
    
    for (int indice = 0; indice < Usuarios.size(); indice++) {
        if (Usuarios[indice].nombre == username) {
            encontrado = indice;
        }
    }

    return encontrado;
};

bool AdministradorDeArchivos::leerDatosUsuarios(std::string archivo) {
    std::string datos = AdministradorDeArchivos::leerArchivoCompleto(archivo);
    std::stringstream ss(datos);
    Usuario Usuario_agregar;
    bool agregado = true;
    this->Usuarios.clear();

    try {
        // Para cada linea del string, delimitada por cambios de linea
        for (std::string linea_de_Usuario; std::getline(ss, linea_de_Usuario);) {
            std::vector<std::string> datos_Usuario;
            std::stringstream ss2(linea_de_Usuario);
            // Se implementa con un vector para mejorar legibilidad, ademas del
            // largo del codigo
            for (std::string dato; std::getline(ss2, dato, ',');) {
                datos_Usuario.push_back(dato);
            }
            Usuario_agregar.nombre = datos_Usuario[0];
            Usuario_agregar.contrasena = datos_Usuario[1];
            Usuario_agregar.nivelDeAcceso = std::stoi(datos_Usuario[2]);
            Usuarios.push_back(Usuario_agregar);
        }
        this->inicio_de_datos = NO_HAY_CAMBIOS; // Ya actualizamos, por lo que lo
            // indicamos en la variable
    } catch (std::exception &e) {
        agregado = false;
    }
    return agregado;
};

bool AdministradorDeArchivos::actualizarDatosUsuarios(std::string archivo) {
    bool actualizado = true;
    for (int indice = 0; indice < Usuarios.size(); indice++) {
        std::string datos = "\n" + Usuarios[indice].nombre + "," +
                            Usuarios[indice].contrasena + "," +
                            std::to_string(Usuarios[indice].nivelDeAcceso);
        int error = this->escribirArchivo(archivo, datos);
        if (error != 0) {
            actualizado = false;
        }
    }
    this->cambios_hechos = NO_HAY_CAMBIOS; // Ya actualizamos, por lo que lo
        // indicamos en la variable
    return actualizado;
};


int AdministradorDeArchivos::verificarContrasena(int indice_Usuario, std::string contrasena) {
    int nivelDeAcceso = -CONTRASENA_INCORRECTA;
    Usuario mi_Usuario = Usuarios[indice_Usuario];
    if (Usuarios[indice_Usuario].contrasena == contrasena) {
        nivelDeAcceso = Usuarios[indice_Usuario].nivelDeAcceso;
    }
    return nivelDeAcceso;
};

std::string AdministradorDeArchivos::leerArchivoCompleto(std::string nombreDeArchivo, std::string UsuarioFS) {
    std::string datos;
    char caracter_leido = ' ';
    int indice = 0;
    while (caracter_leido != '\0') {
        caracter_leido = fs->leer(UsuarioFS, nombreDeArchivo, indice);
        datos += caracter_leido;
        indice++;
    }
    return datos;
};


int AdministradorDeArchivos::autenticarUsuario(std::string username, std::string contrasena, std::string nombreArchivo) {
    int nivelDeAcceso = 0;
    char caracter_a_agregar = ' ';
    int indice = 0;
    int posicion_en_vector = 0;
    
    if (this->inicio_de_datos == HAY_CAMBIOS) {
        // call actualizarDatosUsuarios here
        AdministradorDeArchivos::leerDatosUsuarios(nombreArchivo);
        this->inicio_de_datos = NO_HAY_CAMBIOS;
    }

    if (this->cambios_hechos == HAY_CAMBIOS) {
        // call actualizarDatosUsuarios here
        AdministradorDeArchivos::actualizarDatosUsuarios(nombreArchivo);
    }

    posicion_en_vector = AdministradorDeArchivos::buscarUsuario(username);
    if (posicion_en_vector != -USUARIO_NO_ENCONTRADO) {
        nivelDeAcceso = AdministradorDeArchivos::verificarContrasena(posicion_en_vector, contrasena);
    } else {
        nivelDeAcceso = -USUARIO_NO_ENCONTRADO;
    }
    return nivelDeAcceso;
};

int AdministradorDeArchivos::crearArchivo(std::string nombreDeArchivo,  std::string UsuarioFS) {
    // TODO (us): Se pueden manejar cada uno de los errores que puede indicar el
    // FS
    int error = this->fs->crear(nombreDeArchivo, UsuarioFS);
    return error;
};

int AdministradorDeArchivos::escribirArchivo(std::string nombreDeArchivo, std::string datos, std::string UsuarioFS) {
    // TODO (us): Se pueden manejar cada uno de los errores que puede indicar el
    // FS
    int error = fs->escribir((char *)datos.c_str(), nombreDeArchivo);
    return error;
};

bool  AdministradorDeArchivos::borrarArchivo(std::string nombreDeArchivo, std::string UsuarioFS) {
    // TODO (us): Se pueden manejar cada uno de los errores que puede indicar el
    // FS
    int error = fs->borrar(nombreDeArchivo, 0);
    return error;
};

bool AdministradorDeArchivos::agregarDatosArchivo(std::string nombreDeArchivo, std::string datos, std::string UsuarioFS) {
    // TODO (us): Se pueden manejar cada uno de los errores que puede indicar el
    // FS
    int error = fs->agregar(nombreDeArchivo, (char *)datos.c_str());
    return error;
};

int AdministradorDeArchivos::crearUsuario(std::string username, std::string contrasena,
                  int nivelDeAcceso) {
    int error = 0;
    if (this->inicio_de_datos == HAY_CAMBIOS) {
        // call actualizarDatosUsuarios here
        AdministradorDeArchivos::leerDatosUsuarios("Usuario.dat");
        this->inicio_de_datos = NO_HAY_CAMBIOS;
    }

    int indice = AdministradorDeArchivos::buscarUsuario(username);
    if (indice == -USUARIO_NO_ENCONTRADO) {
        Usuario Usuario_a_agregar;
        Usuario_a_agregar.nombre = username;
        Usuario_a_agregar.contrasena = contrasena;
        Usuario_a_agregar.nivelDeAcceso = nivelDeAcceso;
        this->Usuarios.push_back(Usuario_a_agregar);
        this->cambios_hechos = HAY_CAMBIOS;
    } else {
        error = -USUARIO_YA_EXISTE;
    }    
    return error;
};

int AdministradorDeArchivos::borrarUsuario(std::string username, std::string nombreArchivo) {
    int error = 0;
    int indice = AdministradorDeArchivos::buscarUsuario(username);

    if (this->cambios_hechos == HAY_CAMBIOS) {
        // call actualizarDatosUsuarios here
        AdministradorDeArchivos::actualizarDatosUsuarios();
    }

    if (indice != -USUARIO_NO_ENCONTRADO) {
        this->Usuarios.erase(this->Usuarios.begin() + indice);
        this->cambios_hechos = HAY_CAMBIOS;
    } else {
        error = -USUARIO_NO_ENCONTRADO;
    }
    return error;
};


