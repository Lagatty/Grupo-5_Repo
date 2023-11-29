#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define maxbuffer 1000

//###NOTA: Todo fue realizado en replit, asi es como fuimos trabajando de forma síncrona. Al pasar aca algunas funciones
//ejecutan bien pero casi de forma aleatoria (usando mismo input).
// Supuestos: Todos los productos ingresados tienen distinto codigo de producto.

struct MercaditoLibre {
    struct NodoProducto *raizProducto;
    struct NodoCompraProveedor *headProveedor;
    struct NodoVenta *headVentas;
};

struct NodoProducto { //árbol binario
    struct Producto *datosProductos;
    struct NodoProducto *izquierda, *derecha;
};

struct Producto {
    int codigo;
    char *nombreProducto;
    char *marca;
    int stockProducto;
    int valorUnitario;
};

struct NodoCompraProveedor { // lista simplemente enlazada
    struct CompraProveedor *datosComprasP;
    struct NodoCompraProveedor *sig;
};

struct CompraProveedor {
    char *nombreProveedor;
    int codigoProducto;
    int cantCompra;
    char *fechaCompra;
    int codigoCompra;
};

struct NodoVenta {// Lista circular doblemente enlazada
    struct Venta *detallesVenta;
    struct NodoVenta *ant, *sig;
};

struct Venta { 
    char *fecha;
    char *rutCliente;
    int montoTotal;
    int codigoVenta;
    struct unidadVenta **vectorProductos;
    int plibre;
};

//unidad venta sera para el arreglo de referencias a los productos vendidos

struct unidadVenta{
  int codigoProducto;
  int cantidadVendida;
};

struct Producto *buscarProducto(struct NodoProducto *raiz, int codigoBuscado) {
    // Se realiza búsqueda en abb con el método pre-orden
    while (raiz != NULL) {
        if (codigoBuscado == raiz->datosProductos->codigo) {
            return raiz->datosProductos;
        } else if (codigoBuscado < raiz->datosProductos->codigo) {
            raiz = raiz->izquierda; // se pregunta si el codigo buscado es menor que
        }                         // el codigo,
            // ya que al lado izquierdo se encuentran los elementos de menor valor, es
            // para que no realiza una búsqueda en todos los subárboles de forma
            // innecesaria.
        else {
            raiz =
                    raiz->derecha; // si no es menor, quiere decir que se debe ir hacia la
            // derecha, donde se encuentran los con mayor valor.
        }
    }

    return NULL;
}

struct Producto *crearProducto() {
    // en este caso, se le agregan los nuevos productos que van trayendo los
    // proveedores y no estaban en MB.
    struct Producto *nuevoProducto = (struct Producto *)malloc(sizeof(struct Producto));
    char nombreProducto[maxbuffer];
    char marca[maxbuffer];
    int codigoProducto;
    int stockProducto;
    int valorProducto;
    if (nuevoProducto!=NULL) {
        printf("Ingrese el codigo del producto: \n");
        scanf("%d", &codigoProducto);
        printf("Ingrese el nombre del producto: \n");
        scanf(" %[^\n]s", nombreProducto); getchar();
        printf("Ingrese la marca del producto: \n");
        scanf(" %[^\n]s", marca); getchar();
        printf("Ingrese el stock del producto: ");
        scanf("%d", &stockProducto);
        printf("Ingrese el valor del producto");
        scanf("&d", &valorProducto);

        nuevoProducto->codigo = codigoProducto;

        nuevoProducto->nombreProducto = (char *) malloc(sizeof(char) * (strlen(nombreProducto) + 1));
        strcpy(nuevoProducto->nombreProducto, nombreProducto);

        nuevoProducto->marca = (char *) malloc(sizeof(char) * (strlen(marca) + 1));
        strcpy(nuevoProducto->marca, marca);

        nuevoProducto->stockProducto = stockProducto;
        nuevoProducto->valorUnitario = valorProducto;

        return nuevoProducto;
    }
    return NULL;
}

struct NodoProducto *agregarProducto(struct NodoProducto **raiz,
                                     struct Producto *nuevoProducto) {
    if (*raiz == NULL) {
        struct NodoProducto *nuevoNodo = (struct NodoProducto *)malloc(sizeof(struct NodoProducto));
        nuevoNodo->datosProductos = nuevoProducto;
        nuevoNodo->izquierda = NULL;
        nuevoNodo->derecha = NULL;
        *raiz = nuevoNodo; // Modificamos el puntero raiz original
        return *raiz;
    }

    if (nuevoProducto->codigo == (*raiz)->datosProductos->codigo)
        return *raiz;

    if (nuevoProducto->codigo > (*raiz)->datosProductos->codigo)
        (*raiz)->derecha = agregarProducto(&((*raiz)->derecha), nuevoProducto);
    else
        (*raiz)->izquierda = agregarProducto(&((*raiz)->izquierda), nuevoProducto);

    return *raiz;
}

// Funcion auxiliar para eliminar producto : reemplazo es predecesor en inorden

void reemplazar(struct NodoProducto **raiz, struct NodoProducto **aux) {
    if ((*raiz)->derecha == NULL) {
        (*aux)->datosProductos = (*raiz)->datosProductos;
        *aux = *raiz;
        *raiz = (*raiz)->izquierda;
    } else{
        reemplazar(&((*raiz)->derecha), &((*aux)));
    }
}

void eliminarProducto(struct NodoProducto **raiz, int codigoProductoBorrar) {
    struct NodoProducto *aux = NULL;
    if (*raiz == NULL) {
        return;
    }
    if ((*raiz)->datosProductos->codigo < codigoProductoBorrar) {
        eliminarProducto(&((*raiz)->derecha), codigoProductoBorrar);
    } else {
        if ((*raiz)->datosProductos->codigo > codigoProductoBorrar) {
            eliminarProducto(&((*raiz)->izquierda), codigoProductoBorrar);
        }
            // codigo de producto que visitamos es igual al codigo buscado
        else {
            if ((*raiz)->datosProductos->codigo == codigoProductoBorrar) {
                aux = *raiz;
                if ((*raiz)->izquierda == NULL) {
                    *raiz = (*raiz)->derecha;
                } else {
                    if ((*raiz)->derecha == NULL) {
                        *raiz = (*raiz)->izquierda;
                    }
                        // Caso 3: Nodo que se esta visitando tiene 2 hijos validos
                    else {
                        reemplazar(&((*raiz)->izquierda), &aux);
                    }
                    free(aux);
                }
            }
        }
    }
}
// Funcion de modificar producto especifica para su uso en el menu producto.

struct Producto *modificarProducto(struct NodoProducto *raiz,
                                   int codigoProducto, int stock) {
    // en este caso, se busca el producto en el arbol y se modifica su stock
    struct Producto *ProductoBuscado = NULL;
    ProductoBuscado = buscarProducto(raiz, codigoProducto);
    if (ProductoBuscado != NULL) {
        buscarProducto(raiz, codigoProducto)->stockProducto = stock;
    }
    return ProductoBuscado;
}

struct CompraProveedor *crearCompraProveedor(int codigoProducto, int cantidadComprada, char *fechaCompra, int codigoCompra, char *nombreProveedor) {
    struct CompraProveedor *nuevaCompra = (struct CompraProveedor *)malloc(sizeof(struct CompraProveedor));
    if (nuevaCompra) {
        nuevaCompra->codigoProducto = codigoProducto;
        nuevaCompra->cantCompra = cantidadComprada;
        nuevaCompra->fechaCompra = strdup(fechaCompra);
        nuevaCompra->codigoCompra = codigoCompra;
        nuevaCompra->nombreProveedor = strdup(nombreProveedor);
    }
    return nuevaCompra;
}

struct NodoCompraProveedor *agregarCompraProveedor(struct NodoCompraProveedor *head, struct CompraProveedor *nuevaCompra) {
    struct NodoCompraProveedor *nuevoNodo =
            (struct NodoCompraProveedor *)malloc(sizeof(struct NodoCompraProveedor));
    nuevoNodo->datosComprasP = nuevaCompra;
    nuevoNodo->sig = NULL;
    if (head == NULL)
        return nuevoNodo;
    else {
        // en caso de que la lista no esté vacía, se le agrega el nodo al final.
        struct NodoCompraProveedor *rec = head;
        while (rec->sig != NULL)
            rec = rec->sig;
        rec->sig = nuevoNodo;
    }

    return head;
}

void registrarCompra(struct MercaditoLibre *mercado, int codigoProducto, int cantComprada, char *fechaCompra, int *codigoCompraActual, char *nombreProveedor) { // compraProveedores
    struct Producto *productoEncontrado = buscarProducto(mercado->raizProducto, codigoProducto);
    if (productoEncontrado) productoEncontrado->stockProducto += cantComprada;
    else {
        printf("El producto no existe en el inventario, ingrese los datos para registrarlo: \n");
        agregarProducto(&mercado->raizProducto, crearProducto());
    }
    struct CompraProveedor *nuevaCompra = crearCompraProveedor(codigoProducto, cantComprada, fechaCompra,*codigoCompraActual, nombreProveedor);
    nuevaCompra->codigoCompra = (*codigoCompraActual)++;
    mercado->headProveedor = agregarCompraProveedor(mercado->headProveedor, nuevaCompra);
}

struct Venta *crearVenta(char *rutCliente, char *fechaVenta, int *codigo, int monto, struct unidadVenta **arreglo, int plibre){
    struct Venta *nuevaVenta = (struct Venta *)malloc(sizeof(struct Venta));
    if(nuevaVenta){
        nuevaVenta->montoTotal = monto;
        if(rutCliente) nuevaVenta->rutCliente = strdup(rutCliente);
        if(fechaVenta) nuevaVenta->fecha = strdup(fechaVenta);
        nuevaVenta->codigoVenta = (*codigo);
        nuevaVenta->vectorProductos = arreglo;
        nuevaVenta->plibre = plibre;
    }
    return nuevaVenta;
}


struct NodoVenta *agregarVenta(struct NodoVenta **head, struct Venta *nuevaVenta) {
    struct NodoVenta *nuevoNodo = (struct NodoVenta *)malloc(sizeof(struct NodoVenta));
    nuevoNodo->detallesVenta = nuevaVenta;
    if (*head == NULL) {
        nuevoNodo->ant = nuevoNodo;
        nuevoNodo->sig = nuevoNodo;
        (*head) = nuevoNodo;
    } else {
        struct NodoVenta *rec = (*head);
        do {
            rec = rec->sig;
        } while (rec->sig != (*head));
        nuevoNodo->sig = (*head);
        nuevoNodo->ant = rec;
        rec->sig = nuevoNodo;
        (*head)->ant = nuevoNodo;
    }
    return (*head);
}

//*Se busca venta por codigo (se modifica un indice especifico del arreglo de productos de la venta) y fecha, se modifica la cantidad de venta
// encontrada. Finalmente se retorna el nodo de la venta

struct NodoVenta *modificarVenta(struct MercaditoLibre *mercado, int codigoVenta) {
    if (mercado != NULL && mercado->headVentas != NULL) {
        struct NodoVenta *rec = mercado->headVentas;
        int nuevaCantidad,i,codigoProducto, antiguaCantidad;
        do {
            if (rec->detallesVenta->codigoVenta == codigoVenta) {
                printf("Ingrese el codigo del producto a modificar: \n");
                scanf("%d", &codigoProducto);
                for(i=0;i<rec->detallesVenta->plibre;i++){
                  if(rec->detallesVenta->vectorProductos[i]->codigoProducto==codigoProducto){
                    do{
                    printf("Ingrese la nueva cantidad: \n");
                    scanf("%d", &nuevaCantidad);
                    }while(nuevaCantidad<=0 || nuevaCantidad>=rec->detallesVenta->vectorProductos[i]->cantidadVendida);
                    antiguaCantidad = rec->detallesVenta->vectorProductos[i]->cantidadVendida;
                    rec->detallesVenta->vectorProductos[i]->cantidadVendida = nuevaCantidad;
                    
                    //Se suma o resta el stock correspondiente y retorna la venta modificada
                    buscarProducto(mercado->raizProducto, codigoProducto)->stockProducto += antiguaCantidad - nuevaCantidad;
                    return rec;
                }
                }
            }
            rec = rec->sig;
        }while(rec != mercado->headVentas);
        printf("Venta no encontrada.\n");
    }
    return NULL;
}


struct NodoVenta *buscarVentaFecha(struct NodoVenta *head, char *fechaBuscada)
{
    if (head != NULL)
    {
        struct NodoVenta *rec = head;
        do
        {
            if (strcmp(rec->detallesVenta->fecha,fechaBuscada)== 0)
            {
                return rec;
            }
            rec = rec -> sig;
        } while (rec != head);
    }
    return NULL;
}


struct unidadVenta *crearUnidadVenta(int codigoProducto ,int cantVendida){
  struct unidadVenta *nuevaUnidad = (struct unidadVenta *)malloc(sizeof(struct unidadVenta));
  if(nuevaUnidad){
    nuevaUnidad->codigoProducto = codigoProducto;
    nuevaUnidad->cantidadVendida = cantVendida;
  }
  return nuevaUnidad;
}

int calcularVentasProducto(struct NodoVenta *headVenta, int codigoProducto){
  if(headVenta == NULL) return 0;
  struct NodoVenta *rec = headVenta;
  int i, cantTotal = 0;
  do{
    for(i = 0; i<rec->detallesVenta->plibre; i++){
      if(rec->detallesVenta->vectorProductos[i]->codigoProducto == codigoProducto) 
        cantTotal+=rec->detallesVenta->vectorProductos[i]->cantidadVendida;
    }
    rec = rec->sig;
  }while(rec != headVenta);
  return cantTotal;
}

int getRotacionProducto(struct MercaditoLibre *mercadito) {
  if(mercadito!= NULL && mercadito->headVentas != NULL){
    struct NodoVenta *rec = mercadito->headVentas;
    int productoMasVendido = rec->detallesVenta->vectorProductos[0]->codigoProducto;
    int cantMasVendida = -1;
    int i;
    
    do{
      for(i=0; i< rec->detallesVenta->plibre; i++){
        int cantidadVendida = calcularVentasProducto(mercadito->headVentas, rec->detallesVenta->vectorProductos[i]->codigoProducto);
        if(cantidadVendida > cantMasVendida){
          cantMasVendida = cantidadVendida;
          productoMasVendido = rec->detallesVenta->vectorProductos[i]->codigoProducto;
        }
      }
      rec = rec->sig;
    }while(rec != mercadito->headVentas);
    return productoMasVendido;
        
  }
  return -1;
}

// Esta funcion lista todos los productos "in-order".

void listarProductos(struct NodoProducto *raiz, struct NodoVenta *headVentas) {

    if (raiz != NULL) {
        // Recorre el subárbol izquierdo
        listarProductos(raiz->izquierda, headVentas);
        // Imprime el producto del nodo actual
        printf("Imprimiendo nodo actual...\n");
        printf("Codigo: %d,Stock : %d, Nombre: %s, Rotacion: \n", raiz->datosProductos->codigo, raiz->datosProductos->stockProducto, raiz->datosProductos->nombreProducto);
          /*
          OJITO ACA ABAJOOOO
          OJITO ACA ABAJOOOO
          OJITO ACA ABAJOOOO
          OJITO ACA ABAJOOOO
          OJITO ACA ABAJOOOO
          */
              //raiz->datosProductos->nombreProducto, getRotacionProducto(raiz->datosProductos->codigo, headVentas));
        // Recorre el subárbol derecho
        listarProductos(raiz->derecha, headVentas);
    }
}

// hice la funcion de mostrar en forma de tabla quizas sea util
// añadi cosas en la struct de nodo venta para hacer esto y las siguientes
// funciones

void mostrarVentas(struct NodoVenta *head) {
    if (head != NULL) {
        struct NodoVenta *rec = head;
        int i;
        printf("Lista de ventas: \n");
        do {
            printf("\n- Rut cliente: %s, ", rec->detallesVenta->rutCliente);
            printf("Fecha venta: %s, ", rec->detallesVenta->fecha);
            printf("Codigo venta: %d. \n Productos: \n", rec->detallesVenta->codigoVenta);
            for(i=0;i<rec->detallesVenta->plibre;i++){
            printf("%d - Codigo producto: %d ,",i+1,  rec->detallesVenta->vectorProductos[i]->codigoProducto);
              //PROBLEMA AQUI
              //SE DEBE CREAR STRUCT O OTRO ARREGLO PARA MANEJAR CANTIDAD DE PRODUCTO VENDIDA
            printf("Cantidad vendida: %d.", rec->detallesVenta->vectorProductos[i]->cantidadVendida);
             }
            rec = rec->sig;
        } while (rec != head);
    } else
        printf("No hay ventas\n");
}

// Es una funcion para tener al mejor cliente el que mas gasta
// primero almacena el rut del cliente  y luego se itera para ver
// cuantas veces el cliente hace una compra y asi calcula la suma total
// de sus gastos luego se compara con los otros clientes retornando su rut


int calcularMontoTotal(struct NodoVenta *head, char *rut){
    if(head == NULL) return 0;

    struct NodoVenta *rec = head;
    int monto = 0;

    do {
        if (strcmp(rec->detallesVenta->rutCliente, rut) == 0)
            monto += rec->detallesVenta->montoTotal;
        rec = rec->sig;
    }while(rec != head);

    return monto;
}

void mejorCliente(struct NodoVenta *head){
    if (head != NULL){
        struct NodoVenta *rec = head;
        int maxVenta = 0;
        char *rutMejorCliente = (char*) malloc(maxbuffer * sizeof(char));
        int monto;
        do{
            monto = calcularMontoTotal(head, rec->detallesVenta->rutCliente);
            if (monto > maxVenta){
                maxVenta = monto;
                strcpy(rutMejorCliente, rec->detallesVenta->rutCliente);
            }
            rec = rec->sig;
        } while (rec != head);
        if (rutMejorCliente != NULL) {
            printf("El mejor cliente es %s\n", rutMejorCliente);
        } else {
            printf("No hay mejor cliente\n");
        }
    }
}
// Esta funcion elimina una venta a traves del codigo de la misma primero
// verifica que la lista de ventas exista posteriormente recorre la lista hasta
// encontrar el nodo de la venta a //eliminar Si la venta es eliminada con exito
// se retorna 1, si no 0.

int eliminarVenta(struct NodoVenta **head, int codigoVenta,
                  struct NodoProducto *raizProductos) {
    int i = 0;
    if (*head != NULL) {
        struct NodoVenta *rec = (*head);
        do {
            if (rec->detallesVenta->codigoVenta == codigoVenta) {
                // Caso en el que es el primer nodo y el unico
                if (rec == (*head)) {
                    if (rec == (*head)->sig) {
                        //Actualizar stock de los productos
                        for(i=0; i<rec->detallesVenta->plibre; i++){
                        if (buscarProducto(raizProductos, rec->detallesVenta->vectorProductos[i]->codigoProducto)) {
                            buscarProducto(raizProductos, rec->detallesVenta->vectorProductos[i]->codigoProducto)->stockProducto += rec->detallesVenta->vectorProductos[i]->cantidadVendida;
                        }
                        }
                        (*head) = NULL;
                        return 1;
                    }
                    (*head) = (*head)->sig;
                }
                // Se valida si es que el producto de la venta eliminada es valido, si
                // lo es, suma la cantidad de la venta
                //Actualizar stock de los productos
              for(i=0; i<rec->detallesVenta->plibre; i++){
                if (buscarProducto(raizProductos, rec->detallesVenta->vectorProductos[i]->codigoProducto)) {
                    buscarProducto(raizProductos, rec->detallesVenta->vectorProductos[i]->codigoProducto)->stockProducto +=
                            rec->detallesVenta->vectorProductos[i]->cantidadVendida;
                }
                }
                //Eliminarla de la lista
                rec->ant->sig = rec->sig;
                rec->sig->ant = rec->ant;
                rec = NULL;
                return 1;
            }
            rec = rec->sig;
        } while (rec != *head);
    }
    return 0;
}

// Esta funcion recorre las ventas y suma las unidades vendidas de cada venta
// para obtener la cantidad vendida final

struct CompraProveedor *buscarCompra(struct NodoCompraProveedor *head,
                                     int codigoCompra) {
    struct NodoCompraProveedor *rec = head;
    while (rec != NULL) {
        struct CompraProveedor *compra = rec->datosComprasP;
        if (compra->codigoCompra == codigoCompra) {
            return compra;
            rec = rec->sig;
        }
    }
    return NULL;
}

void eliminarCompra(struct MercaditoLibre *mercado, int codigoCompra) {
    // Buscar la compra en la lista de compras de proveedores
    struct NodoCompraProveedor *rec = mercado->headProveedor;
    struct NodoCompraProveedor *anterior = NULL;

    while (rec != NULL) {
        struct CompraProveedor *compra = rec->datosComprasP;

        if (compra->codigoCompra == codigoCompra) {
            // Se encontró la compra, verificar si se debe eliminar parte o toda la
            // compra
            if (anterior != NULL) {
                anterior->sig = rec->sig;
            } else {
                // Actualizar la cabeza de la lista si se eliminó el primer nodo
                mercado->headProveedor = rec->sig;
            }
            rec->datosComprasP = NULL;
            rec->sig = NULL;

            // Actualizar el stock del producto
            struct Producto *producto =
                    buscarProducto(mercado->raizProducto, compra->codigoProducto);
            if (producto != NULL) {
                producto->stockProducto -= compra->cantCompra;
            }

            printf("Compra con codigo %d eliminada exitosamente.\n", codigoCompra);
            return;
        }

        anterior = rec;
        rec = rec->sig;
    }

    printf("No se encontro una compra con el codigo de compra especificado: %d\n",
           codigoCompra);
}

void modificarCompra(struct MercaditoLibre *mercado, int codigoCompra) {
    struct NodoCompraProveedor *rec = mercado->headProveedor;
    while (rec != NULL) {
        struct CompraProveedor *compra = rec->datosComprasP;
        if (compra->codigoCompra == codigoCompra) {
            int nuevaCantidad;
            int nuevoCodigoProducto;
            printf("Compra encontrada:\n");
            printf("Codigo de compra: %d\n", compra->codigoCompra);
            printf("Codigo de producto actual: %d\n", compra->codigoProducto);
            printf("Cantidad comprada del producto actualmente: %d\n",
                   compra->cantCompra);
            printf("Fecha de compra: %s\n", compra->fechaCompra);

            printf("Ingrese el nuevo codigo de producto: \n");
            scanf("%d", &nuevoCodigoProducto);
            printf("Ingrese la nueva cantidad: \n");
            scanf("%d", &nuevaCantidad);

            struct Producto *productoAnterior = buscarProducto(mercado->raizProducto, compra->codigoProducto);
            struct Producto *productoNuevo = buscarProducto(mercado->raizProducto, nuevoCodigoProducto);
            if (productoAnterior != NULL) productoAnterior->stockProducto -= compra->cantCompra;


            if(productoNuevo != NULL){
                productoNuevo->stockProducto += nuevaCantidad;
                compra->codigoProducto = nuevoCodigoProducto;
                compra->cantCompra = nuevaCantidad;
                compra->cantCompra = nuevaCantidad;
                printf("Compra modificada de manera correcta\n");
            }
            else agregarProducto(&mercado->raizProducto, crearProducto());
            return;
        }
        rec = rec->sig;
    }
    printf("No se encontro una compra con el codigo entregado.\n");
}

void listarCompras(struct NodoCompraProveedor *head){
    if (head == NULL) {
        printf("No hay compras realizadas.\n");
        return;
    }

    printf("**Lista de compra de proveedores**\n");
    struct NodoCompraProveedor *rec = head;
    int numero = 1;
    while (rec != NULL) {
        struct CompraProveedor *compra = rec->datosComprasP;
        printf("#%d\nNombre proveedor: %s\nCodigo producto: %d\nCantidad de compra: %d\nFecha de compra: %s\nCodigo de compra: %d\n",
               numero, compra->nombreProveedor, compra->codigoProducto,
               compra->cantCompra, compra->fechaCompra, compra->codigoCompra);
        numero++;
        rec = rec->sig;
    }
    printf("\n");
}

void proveedorMasCompras(struct NodoCompraProveedor *headCompras){
    if(headCompras == NULL){
        return;
    }
    char *proveedorMaximo = NULL;
    int maxCompras = 0;
    struct NodoCompraProveedor *rec = headCompras;
    while(rec != NULL){
        if(rec->datosComprasP->cantCompra > maxCompras){
            maxCompras = rec->datosComprasP->cantCompra;
            proveedorMaximo = rec->datosComprasP->nombreProveedor;
        }
        rec = rec->sig;
    }
    if(proveedorMaximo != NULL) printf("El proveedor con más movimiento en la empresa es %s\n", proveedorMaximo);
    else printf("No hay movimiento de proveedores.\n");
}

void menuSwitchProductos(struct MercaditoLibre *mercado) {
    int opcion;
    int codigoProducto;
    char *nombreProducto = (char *)malloc(sizeof(char) * maxbuffer);
    int stockProducto;
    struct Producto *nuevoProducto;
    do {
        printf("\n** Lista de opciones de menú producto**\n");
        printf("1. Agregar producto\n");
        printf("2. Eliminar producto\n");
        printf("3. Buscar producto\n");
        printf("4. Modificar producto\n");
        printf("5. Listar productos\n");
        printf("6. Producto con mayor rotación\n");
        printf("7. Volver\n");
        printf("Ingrese el número de opcion: ");
        scanf("%d", &opcion);
        // fflush(stdout);
        switch (opcion) {
            case 1: // Agregar

                nuevoProducto = crearProducto();
                printf("Producto creado satisfactioriamente...");
                agregarProducto((&mercado->raizProducto),
                                nuevoProducto); // Insertar producto en arbol
                break;
            case 2: // Eliminar
                printf("Ingrese el codigo del producto a eliminar: \n");
                scanf("%d", &codigoProducto);
                eliminarProducto(&mercado->raizProducto, codigoProducto);
                break;

            case 3: // Buscar
                printf("Ingrese codigo de producto buscado: ");
                scanf("%d", &codigoProducto);
                if (buscarProducto(mercado->raizProducto, codigoProducto)) {
                    printf("\nProducto encontrado");
                } else {
                    printf("\nProducto no encontrado\n");
                }
                break;

            case 4: // Modificar
                printf("Ingrese codigo de producto a modificar: ");
                scanf("%d", &codigoProducto);
                printf("\nIngrese nuevo stock para el producto: ");
                scanf("%d", &stockProducto);
                modificarProducto(mercado->raizProducto, codigoProducto, stockProducto);
                break;
            case 5: // Listar
                printf("Lista de productos: \n");
                listarProductos(mercado->raizProducto, mercado->headVentas);
                break;
            case 6:
                printf("El código del producto con mayor rotación %d\n",getRotacionProducto(mercado->raizProducto));
                break;
            case 7: // Volver
                printf("Saliendo del menu, hasta luego.\n");
                break;
            default:
                printf("Opcion no válida, por favor ingrese una de las opciones "
                       "anteriormente mencionadas.\n");
        }

    } while (opcion != 7);
}


struct Venta *buscarVenta(struct NodoVenta *head, int codigoVenta){

    struct NodoVenta *rec = head;

    do{
        if(rec->detallesVenta->codigoVenta == codigoVenta) return rec->detallesVenta;
        rec = rec->sig;
    }while(rec != head);

    return NULL;
}

struct unidadVenta *buscarProductoEnArreglo(struct unidadVenta **vector, int pLibre, int codigoProducto){

    int i = 0, enc = 0;

    while((!enc) && (i < pLibre)){

        if(vector[i]->codigoProducto == codigoProducto) enc = 1;
        else i++;
    }

    if(enc == 1) return vector[i];
    return NULL;


}

void opcionBusqueda(struct MercaditoLibre *ML){

    int idVenta, codigoProducto;

    printf("Ingresa el codigo de la venta :");
    scanf("%d", &idVenta);

    struct Venta *venta = buscarVenta(ML->headVentas, idVenta);

    if(venta){

        printf("Ingrese el codigo del producto :");
        scanf("%d", &codigoProducto);

        struct unidadVenta *idProducto = buscarProductoEnArreglo(venta->vectorProductos, venta->plibre, codigoProducto);

        if(idProducto){

            struct Producto *producto = buscarProducto(ML->raizProducto, idProducto->codigoProducto);
            printf("Id Producto: %d | Nombre: %s | Unidades Compradas: %d", producto->codigo, producto->nombreProducto, idProducto->cantidadVendida);
        }
        else{
            printf("El producto no ha sido encontrado \n");
            return;
        }
    }
    else{
        printf("No se ha encontrado la venta \n");
    }
}

void mostrarProductos(struct unidadVenta **arreglo, struct NodoProducto *raiz, int pLibre){
    int i;

    for(i = 0; i < pLibre; i++){

        struct Producto *producto = buscarProducto(raiz, arreglo[i]->codigoProducto);
        printf("Id Producto: %d | Nombre: %s | Unidades Compradas: %d \n", producto->codigo, producto->nombreProducto, arreglo[i]->cantidadVendida);
    }
}

void ordenarProductos(struct unidadVenta **arreglo, int pLibre){

    int i, k;
    struct unidadVenta *aux = NULL;

    for(i = 0; i < pLibre-1; i++){

        for(k = 0; k < pLibre-1; k++){

            if(arreglo[k]->codigoProducto > arreglo[k+1]->codigoProducto){

                aux = arreglo[k];
                arreglo[k]= arreglo[k+1];
                arreglo[k+1] = aux;
            }
        }
    }
}

void opcionMostrar(struct MercaditoLibre *ML){

    int codigoVenta;

    printf("ingresa el codigo de venta :");
    scanf("%d", &codigoVenta);

    struct Venta *venta = buscarVenta(ML->headVentas, codigoVenta);

    if(venta){

        ordenarProductos(venta->vectorProductos, venta->plibre);
        mostrarProductos(venta->vectorProductos, ML->raizProducto, venta->plibre);
    }
    else{
        printf("No se ha encontrado la venta \n");
    }
}


void gestionarVenta(struct MercaditoLibre *mercado, int *codigoVentaActual){

    int codigoProducto, cantVendida = 0, montoVenta, cantProductos, indice = 0;
    char *rutCliente = (char *)malloc(sizeof(char) * 12);
    char *fechaVenta = (char *)malloc(sizeof(char) * 10);

    printf("Ingrese la cantidad de productos a comprar : ");
    scanf("%d", &cantProductos);

    printf("Ingrese el rut del cliente: \n");
    scanf("%11s", rutCliente);

    printf("Ingrese la fecha de la venta:\n");
    scanf("%s", fechaVenta);

    printf("Ingrese el monto de la venta:\n");
    scanf("%d", &montoVenta);

    struct unidadVenta **arreglo = (struct unidadVenta **) malloc(sizeof(struct unidadVenta *) * cantProductos);
    while(true) {
        printf("Ingrese el codigo del producto: ");
        scanf("%d", &codigoProducto);
        printf("\nIngrese la cantidad a comprar: ");
        scanf("%d", &cantVendida);
        struct Producto *productoEncontrado = buscarProducto(mercado->raizProducto, codigoProducto);
        if(productoEncontrado) {
            if (productoEncontrado->stockProducto >= cantVendida) {
                productoEncontrado->stockProducto -= cantVendida;
                //SE CREA LA UNIDAD DE VENTA
                struct unidadVenta *unidadVenta = crearUnidadVenta(codigoProducto, cantVendida);
                if(productoEncontrado->stockProducto <= 10)
                    printf("ALERTA: Queda poco stock del producto. Unidades restantes : %d\n", productoEncontrado->stockProducto);

                //SE AGREGA LA UNIDAD DE VENTA CREADA AL ARREGLO
                arreglo[indice] = unidadVenta;
                indice++;
                if(indice == cantProductos) break;
            }
            else{
              printf("No hay suficiente stock para la venta.\n");
              cantProductos--;
            }
        }
        else {
            printf("No contamos con el producto en inventario.\n");
            //return;
            cantProductos--;
        }
    }
    struct Venta *nuevaVenta = crearVenta(rutCliente, fechaVenta,codigoVentaActual,montoVenta, arreglo, cantProductos);
    nuevaVenta->codigoVenta = (*codigoVentaActual)++;
    agregarVenta(&(mercado->headVentas), nuevaVenta);
    printf("Gracias por su compra\n");
}

void menuSwitchVentas(struct MercaditoLibre *mercado, int *codigoVentaActual) {
    int opcion;
    int codigoProducto;
    char *nombreProducto = (char *)malloc(sizeof(char) * 20);
    int stockProducto;
    int cantVendida = 0;
    int montoVenta;
    int codigoVenta;
    char *rutCliente = (char *)malloc(sizeof(char) * 12);
    char *fechaVenta = (char *)malloc(sizeof(char) * 10);
      do {
          printf("\n** Lista de opciones de menu ventas a clientes**\n");
          printf("1. Agregar venta\n");
          printf("2. Eliminar venta\n");
          printf("3. Buscar  venta\n");
          printf("4. Modificar venta\n");
          printf("5. Listar ventas\n");
          printf("6. Mejor cliente \n");
          printf("7. Buscar producto en venta \n");
          printf("8. Mostrar productos de una venta ordenados \n");
          printf("9. Volver\n");
          printf("Ingrese el numero de opcion: ");
          scanf("%d", &opcion);
          switch (opcion) {
              case 1: // Agregar

                  gestionarVenta(mercado, codigoVentaActual);
                  break;
              case 2: // Eliminar
                  printf("Ingrese el codigo del producto a eliminar de las ventas: ");
                  scanf("%d", &codigoProducto);
                  if (eliminarVenta(&(mercado->headVentas), codigoProducto,
                                    mercado->raizProducto) == 1) {
                      printf("\nVenta eliminada de la lista de ventas.\n");
                  } else {
                      printf("\nCodigo de venta no encontrado.\n");
                  }
                  break;

              case 3: // Buscar
                  printf("Ingrese codigo de producto de producto de la venta buscada: ");
                  scanf("%d", &codigoProducto);
                  printf("\nIngrese fecha de la venta buscada: ");
                  scanf("%s", fechaVenta);
                  if(buscarProducto(mercado->raizProducto, codigoProducto)) {
                      if (buscarVentaFecha(mercado->headVentas, fechaVenta)) {
                          printf("\nVenta encontrada.\n");
                      }
                      else
                          printf("\nNo se encuentra una venta en la fecha.\n");
                  }
                  else
                      printf("\nProducto no encontrado\n");
                  break;

              case 4: // Modificar
                  printf("Ingrese codigo de venta a modificar: ");
                  scanf("%d", &codigoVenta);
                  modificarVenta(mercado, codigoVenta);
                  break;
              case 5: // Listar
                  mostrarVentas(mercado->headVentas);
                  break;

              case 6: //Mejor cliente
                  mejorCliente(mercado ->headVentas);
                  break;

              case 7: //Buscar producto en venta
                  opcionBusqueda(mercado);
                  break;

              case 8: //Mostrar productos de una venta ordenados
                  opcionMostrar(mercado);

              case 9: // Volver
                  printf("Saliendo del menu, hasta luego.\n");
                  break;
              default:
                  printf("Opcion no válida, por favor ingrese una de las opciones "
                         "anteriormente mencionadas.\n");
          }
      }
      while (opcion != 9);
  }

void menuSwitchCompras(struct MercaditoLibre * mercado, int *codigoCompraActual) {
    int opcion;
    int codigoProducto;
    int cantidadComprada;
    char *fechaCompra;
    int codigoCompra;
    char *nombreProveedor = (char *)malloc(sizeof(char) * 30);
    fechaCompra = (char *)malloc(sizeof(char) * 10);
    do {
        printf("\n** Menu de Compras a Proveedores **\n");
        printf("1. Registrar compra\n");
        printf("2. Eliminar compra\n");
        printf("3. Buscar compra\n");
        printf("4. Modificar compra\n");
        printf("5. Listar compras\n");
        printf("6. Proveedor con más movimiento en la empresa\n");
        printf("7. Volver al menu principal\n");
        printf("Ingrese el numero de opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: // Registrar compra
                printf("Ingrese el codigo del producto comprado: \n");
                scanf("%d", &codigoProducto);
                printf("Ingrese la cantidad comprada: \n");
                scanf("%d", &cantidadComprada);
                printf("Ingrese la fecha de la compra: \n");
                scanf("%10s", fechaCompra);
                printf("Ingrese el nombre del proveedor: \n");
                scanf("%s", nombreProveedor);
                registrarCompra(mercado, codigoProducto, cantidadComprada, fechaCompra, codigoCompraActual, nombreProveedor);
                break;
            case 2: // Eliminar compra
                printf("Ingrese el codigo de la compra que desea eliminar: \n");
                scanf("%d", &codigoCompra);
                eliminarCompra(mercado, codigoCompra);
                break;
            case 3: // Buscar compra
                printf("\nIngrese codigo de la compra de proveedor buscada: ");
                scanf("%d", &codigoCompra);
                if(buscarCompra(mercado->headProveedor, codigoCompra)){
                    printf("Compra encontrada.\n");
                }
                else
                    printf("\nNo se encuentra una compra con ese codigo.\n");
                break;
            case 4: // Modificar compra
                printf("Ingrese codigo de la compra a modificar: ");
                scanf("%d", &codigoCompra);
                modificarCompra(mercado, codigoCompra);
                break;
            case 5: // Listar compras
                listarCompras(mercado->headProveedor);
                break;

            case 6: //Proveedor con más movimiento en la empresa
                proveedorMasCompras(mercado->headProveedor);
                break;
            case 7: // Volver al menú principal
                printf("Volviendo al menu principal.\n");
                break;
            default:
                printf("Opcion no válida, por favor ingrese una de las opciones "
                       "mencionadas.\n");
        }

    }while (opcion != 7);
}

int main(){
    struct MercaditoLibre *mercado =
            (struct MercaditoLibre *)malloc(sizeof(struct MercaditoLibre));

    int codigoCompraActual = 1;
    int codigoVentaActual = 1;
    if (mercado == NULL) {
        printf("Error: No se pudo asignar memoria para la estructura "
               "MercaditoLibre.\n");
        return 1;
    }
    int opcion;
    do {
        printf("\n** Lista de opciones de menu**\n");
        printf("1. Productos\n");
        printf("2. Ventas de productos a clientes\n");
        printf("3. Compra de productos a proveedores \n");
        printf("4. Salir\n");
        printf("Ingrese el numero de opcion: ");
        scanf("%d", &opcion);
        switch (opcion) {
            case 1:
                menuSwitchProductos(mercado);
                break;
            case 2:
                menuSwitchVentas(mercado, &codigoVentaActual);
                break;
            case 3:
                menuSwitchCompras(mercado, &codigoCompraActual);
                break;
            case 4:
                printf("Saliendo del menu, hasta luego.\n");
                break;
            default:
                printf("Opcion no valida, por favor ingrese una de las opciones "
                       "anteriormente mencionadas.\n");
        }
    } while (opcion != 4);

    return 0;
}
