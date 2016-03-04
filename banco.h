/*
 * Autor: Francisco Enrique Córdova González
 * Descripción:
 + Esta es una librería que permite emular un banco dado una hora
 + a la que el banco se abra y a la que se cierre, el número de cajeras
 + y el usuario es libre de meter clientes cuando le plazca.
 + Y el banco puede o no emular la actividad en tiempo real y al final,
 + independientemente muestra en pantalla una ola de datos importantes
 + como el max tiempo libre de una cajera, número de clientes totales, etc...
 * Última actualización: 3 de Marzo de 2016
 * La librería viene con un archivo .cpp de ejemplo que ilustra mejor cómo
 * sacarle el máximo partido a esta librería
 */
//Directivas
#include <iostream>
#include <string>
#include <cstdlib>
#include "cajera.h"
/*
 * Clase banco
 * Es una clase normal, no hay nada que destacar.
 */
class banco {
  //Ola de datos para mostrar en pantalla.
  size_t max_clientes; //Número máximo de clientes en el día
  size_t hora_cierre;
  size_t max_espera; //Máxima espera de un cliente en minutos
  size_t max_cola; //Cola más larga en el banco
  size_t max_tiempo_libre_cajera;
  size_t hora_actual;
  size_t cantidad_clientes; //Cantidad clientes actuales en el banco
  size_t cantidad_cajeras; //Cantidad cajeras actuales en el banco
  cajera *cajeras; //cajera es una estructura y las cajeras se almacenan en forma de pila
public:
  banco(size_t cajeras, size_t hora_abrir, size_t hora_cerrar);
  ~banco();
  void agregar_cliente(size_t hora_llegada, size_t tiempo_operacion);
  void aumentar_un_minuto();
  void agregar_cajera();
  void checa_cajera_libre(cajera *);
  void checa_cajeras_libre(cajera *);
  void c_solicita_cliente(cajera *);
  void c_desocupate(cajera *);
  void checa_cajeras_no_libres(cajera *);
  size_t obtenerNumeroCajeras(){return cantidad_cajeras;}
  size_t obtenerMaxCola(){return max_cola;}
  void imprimeBanco();

};
banco::banco (size_t _cajeras, size_t hora_abrir, size_t hora_cerrar) {
  max_clientes = 0;
  max_espera = 0;
  max_tiempo_libre_cajera = 0;
  cajeras = NULL;
  hora_actual = hora_abrir;
  hora_cierre = hora_cerrar;
  cantidad_clientes = 0;
  cantidad_cajeras = 0;
  max_cola = 0;
  for (size_t i = 0; i < _cajeras; ++i) agregar_cajera();
}
banco::~banco() {
  cajera *iterador;
  while (cajeras) {
    iterador = cajeras;
    cajeras = iterador->siguiente_cajera;
    delete iterador;
  }
  max_clientes = 0;
  max_espera = 0;
  cajeras = NULL;
  max_tiempo_libre_cajera = 0;
  hora_actual = 0;
  hora_cierre = 0;
  cantidad_clientes = 0;
  cantidad_cajeras = 0;
  max_cola = 0;
}
void banco::imprimeBanco() {
  cajera *cajera_actual = cajeras;
  system("clear");
  std::cout << "Banco: " << std::endl;
  for (size_t i = 0; i < cantidad_cajeras; i++) {
    std::cout << "Cajera " << (i+1) << ": ";
    if (cajera_actual->numero_clientes_en_cola > 0)
      for (size_t j = 0; j < cajera_actual->numero_clientes_en_cola; ++j) std::cout << " * ";
    else std::cout << "Sin clientes";
    std::cout<< std::endl;
    cajera_actual = cajera_actual->siguiente_cajera;
  }
  std::cout << "Maximo tiempo libre de una cajera: " << max_tiempo_libre_cajera << std::endl;
  std::cout << "Maxima cola: " << max_cola << std::endl;
  std::cout << "Max clientes: "<< max_clientes << std::endl;
  std::cout << "Clientes actuales: " << cantidad_clientes << std::endl;
  std::cout << "Max espera de un cliente: " << max_espera << std::endl;
  std::cout << "Hora actual: " << hora_actual << std::endl;
}
void banco::checa_cajera_libre(cajera *cajera_actual) {
  cajera *_cajera = cajera_actual;
  if (_cajera->libre) {
    _cajera->tiempo_libre++;
    _cajera->max_tiempo_libre++;
    if (max_tiempo_libre_cajera < _cajera->max_tiempo_libre)
      max_tiempo_libre_cajera = _cajera->max_tiempo_libre;
  }
}
void banco::checa_cajeras_libre(cajera *cajera_inicio) {
  cajera *_cajera = cajera_inicio;
  while (_cajera) {
    checa_cajera_libre(_cajera);
    _cajera = _cajera->siguiente_cajera;
  }
}
void banco::c_solicita_cliente(cajera *cajera_actual) {
  cajera *_cajera = cajera_actual;
  cliente *_cliente = _cajera->cliente_actual;
  _cajera->cliente_actual = _cliente->siguiente_cliente;

  if (max_espera < (hora_actual - _cliente->hora_llegada))
    max_espera = hora_actual - _cliente->hora_llegada;

  delete _cliente;

  _cajera->tiempo_liberar_cliente = hora_actual + _cajera->cliente_actual->tiempo_accion;
  _cajera->tiempo_ocupada += _cajera->cliente_actual->tiempo_accion;
  _cajera->numero_clientes_en_cola--;
  --cantidad_clientes;
}
void banco::c_desocupate(cajera *cajera_actual) {
  cajera *_cajera = cajera_actual;
  cliente *_cliente = _cajera->cliente_actual;
  _cajera->cliente_actual = NULL;
  _cajera->ultimo_cliente = NULL;
  _cajera->libre = true;
  _cajera->numero_clientes_en_cola = 0;
  _cajera->max_tiempo_libre = 0;
  if (max_espera < (hora_actual - _cliente->hora_llegada))
    max_espera = hora_actual - _cliente->hora_llegada;
  delete _cliente;
  --cantidad_clientes;
}
void banco::checa_cajeras_no_libres(cajera *cajera_inicio) {
  cajera *_cajera = cajera_inicio;
  while (_cajera) {
    if (!_cajera->libre && _cajera->tiempo_liberar_cliente <= hora_actual)
      (_cajera->cliente_actual->siguiente_cliente) ? c_solicita_cliente(_cajera) : c_desocupate(_cajera);
    _cajera = _cajera->siguiente_cajera;
  }
}
void banco::aumentar_un_minuto() {
  ++hora_actual;
  checa_cajeras_libre(cajeras);
  checa_cajeras_no_libres(cajeras);
}

void banco::agregar_cliente(size_t hora_llegada, size_t tiempo_operacion) {

  cliente *nuevo_cliente = new cliente;
  nuevo_cliente->hora_llegada = hora_llegada;
  nuevo_cliente->tiempo_accion = tiempo_operacion;
  nuevo_cliente->siguiente_cliente = NULL;
  cajera *n_cajera = cajeras;
  //Método de busqueda de cajeras libres.
  for (size_t i = 0; i < cantidad_cajeras; ++i) {
    if (n_cajera->libre) {
      if (hora_actual + tiempo_operacion > hora_cierre){
        std::cout << "El banco esta cerrando, se ha negado un cliente" << std::endl;
        delete nuevo_cliente;
        return;
      }
      n_cajera->cliente_actual = nuevo_cliente;
      n_cajera->ultimo_cliente = nuevo_cliente;
      n_cajera->tiempo_ocupada += tiempo_operacion;
      n_cajera->libre = false;
      n_cajera->numero_clientes_en_cola++;
      if (max_cola < n_cajera->numero_clientes_en_cola) max_cola = n_cajera->numero_clientes_en_cola;
      n_cajera->tiempo_liberar_cliente = hora_actual + tiempo_operacion;
      max_clientes++;
      cantidad_clientes++;
      return;
    }
    n_cajera = n_cajera->siguiente_cajera;
  }
  //Si estamos en esta parte quiere decir que no hay cajeras desocupadas, así que
  //Toca poner al cliente en una cola
  //Por motivos lógicos se pondría al cliente en la cola más corta
  //Pero como aquí tenemos el poder de saber la mejor cola para el cliente
  //Lo pondremos en la mejor cola que no es necesariamente la más corta.
  //Reiniciamos nuestro puntero de cajera
  n_cajera = cajeras;
  //Buscamos la cajera que tenga el menor tiempo en desocuparse (Si fuese otro caso podríamos poner el menor tamaño de cola)
  //n_cajera la usaremos para iterar en la pila de cajeras así que creamos
  //otro puntero donde tendremos a la mejor cajera para el cliente.
  cajera *mejor_cajera = n_cajera;
  size_t tiempo_necesario = mejor_cajera->tiempo_ocupada;
  //Algoritmo que busca la cajera con el menor tiempo para desocuparse
  for (size_t i = 0; i < cantidad_cajeras; i++) {
    if (tiempo_necesario > n_cajera->tiempo_ocupada){
      tiempo_necesario = n_cajera->tiempo_ocupada;
      mejor_cajera = n_cajera;
    }
    n_cajera = n_cajera->siguiente_cajera;
  }
  //Decimos que el siguiente cliente del último cliente es igual al nuevo cliente
  //Y ahora el último cliente es el nuevo cliente.
  if (hora_actual + tiempo_necesario + tiempo_operacion > hora_cierre) {
    std::cout << "Las filas son demasiado largas, ya no se aceptaran clientes" << std::endl;
    delete nuevo_cliente;
    return;
  }
  mejor_cajera->ultimo_cliente->siguiente_cliente = nuevo_cliente;
  mejor_cajera->ultimo_cliente=nuevo_cliente;
  mejor_cajera->tiempo_ocupada += tiempo_operacion;
  mejor_cajera->max_tiempo_libre = 0;
  mejor_cajera->numero_clientes_en_cola += 1;
  if (max_cola < mejor_cajera->numero_clientes_en_cola) max_cola = mejor_cajera->numero_clientes_en_cola;
  max_clientes++;
  cantidad_clientes++;
}
/*
  Esta función se encarga de añadir cajeras al banco.
 */
void banco::agregar_cajera() {
  //Creamos un puntero a cajera
  cajera *nueva_cajera = new cajera;
  //Como ahora tenemos una cajera aumentamos la cantidad de cajeras
  ++cantidad_cajeras;
  //Aquí no es necesario checar si había antes una cajera
  //Ya que en este caso es indiferente.
  //Las agregamos en forma de pila.
  nueva_cajera->siguiente_cajera = cajeras;
  //Ponemos todos sus valores en ceros y nulos.
  nueva_cajera->tiempo_libre = 0;
  nueva_cajera->max_tiempo_libre = 0;
  nueva_cajera->numero_clientes_en_cola = 0;
  nueva_cajera->libre = true;
  nueva_cajera->cliente_actual = NULL;
  nueva_cajera->tiempo_ocupada = 0;
  nueva_cajera->tiempo_liberar_cliente = 0;
  //La ponemos al inicio de la pila
  cajeras = nueva_cajera;
}
