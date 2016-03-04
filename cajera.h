struct cliente {
  size_t hora_llegada;
  size_t tiempo_accion;
  cliente *siguiente_cliente;
};
struct cajera {
  cajera *siguiente_cajera;
  size_t max_tiempo_libre;
  size_t tiempo_libre;
  size_t tiempo_ocupada;
  size_t numero_clientes_en_cola;
  cliente *cliente_actual;
  cliente *ultimo_cliente;
  size_t tiempo_liberar_cliente;
  bool libre;
};
