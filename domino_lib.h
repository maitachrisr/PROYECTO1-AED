#include <iostream>

using namespace std;

// --- Constantes ---
const int LONGITUD_NOMBRE = 30; 
const int MAX_JUGADORES = 4;
const int TOTAL_FICHAS = 28;

// --- Estructuras de Datos ---

struct FichaDomino {
    int lado1;
    int lado2;

    // constructor simple (necesario para la creación de fichas)
    FichaDomino(int l1 = 0, int l2 = 0) {
        lado1 = l1;
        lado2 = l2;
    }

    bool esDoble() { return lado1 == lado2; }
    int obtenerMayorValor() { return (lado1 > lado2) ? lado1 : lado2; }
    int obtenerPuntuacion() { return lado1 + lado2; }
};

// nodo de la Lista Doblemente Enlazada
struct Node {
    FichaDomino datos;
    Node* anterior;
    Node* siguiente;
};

// === FUNCIONES AUXILIARES Y DE LÓGICA PURA ===

void swapInt(int& a, int& b) {
    int temp = a; a = b; b = temp;
}

void swapFicha(FichaDomino& t1, FichaDomino& t2) {
    int tempLado1 = t1.lado1;
    int tempLado2 = t1.lado2;
    
    t1.lado1 = t2.lado1; t1.lado2 = t2.lado2;
    t2.lado1 = tempLado1; t2.lado2 = tempLado2;
}

bool arrayContiene(int arr[], int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) return true;
    }
    return false;
}

// estructura para representar a un jugador
struct Jugador {
    char nombre[LONGITUD_NOMBRE];      
    Node* cabezaFichas;
    Node* colaFichas;
    int contadorFichas;
             
    int puntuacionTotal;             
    bool haPasado;             

    Jugador() {
        cabezaFichas = nullptr;
        colaFichas = nullptr;
        contadorFichas = 0;
        puntuacionTotal = 0;
        haPasado = false;
        // inicialización de nombre
        for(int i = 0; i < LONGITUD_NOMBRE; i++) nombre[i] = '\0';
    }
};

// === MANIPULACIÓN DE LISTAS (logica completa, obligatoria para manejo de memoria) ===

void liberarLista(Node*& cabeza, Node*& cola, int& contador) {
    Node* actual = cabeza;
    while (actual != nullptr) {
        Node* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    cabeza = nullptr;
    cola = nullptr;
    contador = 0;
}

void agregarFichaLista(Node*& cabeza, Node*& cola, int& contador, FichaDomino ficha) {
    Node* newNode = new Node; 
    newNode->datos = ficha;
    newNode->anterior = nullptr;
    newNode->siguiente = nullptr;
    
    if (cabeza == nullptr) {
        cabeza = newNode;
        cola = newNode;
    } else {
        newNode->anterior = cola;
        cola->siguiente = newNode;
        cola = newNode;
    }
    contador++;
}

FichaDomino removerNodeLista(Node*& cabeza, Node*& cola, int& contador, Node* nodeARemover) {
    if (nodeARemover == nullptr) return FichaDomino(-1, -1);
    
    if (nodeARemover->anterior != nullptr) {
        nodeARemover->anterior->siguiente = nodeARemover->siguiente;
    } else {
        cabeza = nodeARemover->siguiente; 
    }

    if (nodeARemover->siguiente != nullptr) {
        nodeARemover->siguiente->anterior = nodeARemover->anterior;
    } else {
        cola = nodeARemover->anterior; 
    }
    
    FichaDomino fichaRemovida = nodeARemover->datos;
    delete nodeARemover; 
    contador--;
    return fichaRemovida;
}

Node* obtenerNodePorIndiceLista(Node* cabeza, int contador, int indice) {
    if (indice < 1 || indice > contador) return nullptr;
    
    Node* actual = cabeza;
    for (int i = 1; i < indice; i++) {
        actual = actual->siguiente;
    }
    return actual;
}


// estructura principal del juego
struct JuegoDomino {
    Jugador jugadores[MAX_JUGADORES];
    
    // lista del pozo
    Node* cabezaPozo;        
    Node* colaPozo;
    int contadorPozo;

    // lista de la cadena de mesa
    Node* cabezaMesa;  
    Node* colaMesa;
    int contadorMesa;
    
    int rondasJugadas;
    int indiceJugadorActual;
    int numJugadores;
    
    FichaDomino ultimaFichaJugada; 

    // variables de estado
    int valor1; 
    int valor2; 
    FichaDomino fichaA; 
    FichaDomino fichaB; 
    int verificarArray[TOTAL_FICHAS]; 
    int tamanioArray;
    int valorAVerificar;
    bool resultadoContieneArray;
    int ladoFichaI;
    int ladoFichaJ;
    int contadorReparto;
    int indiceJugadorReparto;
    int banderaEstadoRonda; 
    int resultadoJugadorInicial; 
    int indiceJugadorFichaInicial; 
    FichaDomino fichaInicialJugada; 
    int valorExtremoIzquierdo; 
    int valorExtremoDerecho; 
    FichaDomino fichaParaVerificar; 
    bool resultadoEsJugable; 
    int indicesJugables[TOTAL_FICHAS]; 
    int contadorJugables; 
    bool resultadoIntentarRobarPozo; 
    int indiceFichaJugar; 
    int extremoElegido; 
    FichaDomino resultadoEjecutarJugada; 
    int resultadoPuntuacionMano; 
    int indiceGanadorTranca; 
    bool banderaEmpateTranca; 
    int resultadoGanadorGeneral; 

    JuegoDomino() : 
        cabezaPozo(nullptr), colaPozo(nullptr), contadorPozo(0),
        cabezaMesa(nullptr), colaMesa(nullptr), contadorMesa(0),
        banderaEstadoRonda(0), 
        resultadoJugadorInicial(-1),
        resultadoContieneArray(false),
        resultadoEsJugable(false),
        resultadoIntentarRobarPozo(false)
        {}
};

// el resto de las funciones serán definidas en main.cpp para mantener la estructura del juego