#include <iostream>
#include "domino_lib.h" 

using namespace std;

// (esto es necesario para que las funciones más abajo llamen a las de arriba)
int manejarTurnoJugador(JuegoDomino& juego);

// ---------------------------------------------------------------------

// juega la ficha inicial (doble 6 o la más alta) y la coloca en la mesa.
FichaDomino jugarFichaInicial(JuegoDomino& juego, int indiceJugadorInicial) { 
    Jugador& jugadorInicial = juego.jugadores[indiceJugadorInicial];
    Node* nodeAJugar = nullptr;
    FichaDomino fichaMayor(0, 0); // Inicializada a doble cero

    // 1. encontrar la mejor ficha (doble 6 o la mayor)
    Node* actual = jugadorInicial.cabezaFichas;
    while (actual != nullptr) {
        if (actual->datos.lado1 == 6 && actual->datos.lado2 == 6) {
            nodeAJugar = actual;
            break; 
        }
        if (actual->datos.obtenerMayorValor() > fichaMayor.obtenerMayorValor()) {
            fichaMayor = actual->datos;
            nodeAJugar = actual;
        }
        actual = actual->siguiente;
    }
    
    if (nodeAJugar != nullptr) {
        FichaDomino ficha = removerNodeLista(jugadorInicial.cabezaFichas, jugadorInicial.colaFichas, jugadorInicial.contadorFichas, nodeAJugar);
        
        // la primera ficha se juega sin rotación
        agregarFichaLista(juego.cabezaMesa, juego.colaMesa, juego.contadorMesa, ficha);
        
        // al ser la primera jugada, ambos extremos toman el valor de los lados.
        // asumimos que la ficha se coloca con lado1 a la izquierda y lado2 a la derecha.
        juego.valorExtremoIzquierdo = ficha.lado1;
        juego.valorExtremoDerecho = ficha.lado2;
        
        return ficha;
    }
    return FichaDomino(-1, -1); 
}

// === FUNCIONES DE INICIALIZACIÓN Y FLUJO (La logica del Juego) ===

// Determina el jugador inicial (quien tiene el doble 6 o la ficha más alta).
int determinarJugadorInicial(JuegoDomino& juego) {
    int indiceMejorJugador = -1;
    FichaDomino fichaMayor(-1, -1);

    for (int i = 0; i < juego.numJugadores; ++i) {
        Node* actual = juego.jugadores[i].cabezaFichas;
        while (actual != nullptr) {
            // regla 1: Busca el doble 6
            if (actual->datos.lado1 == 6 && actual->datos.lado2 == 6) {
                return i;
            }
            // regla 2: Busca la ficha de mayor valor 
            if (actual->datos.obtenerMayorValor() > fichaMayor.obtenerMayorValor()) {
                fichaMayor = actual->datos;
                indiceMejorJugador = i;
            }
            actual = actual->siguiente;
        }
    }
    return indiceMejorJugador;
}

// suma los puntos de las fichas restantes al total acumulado del jugador
int puntuarManoJugador(Jugador& jugador) { 
    int puntuacionRonda = 0;
    Node* actual = jugador.cabezaFichas;
    while (actual != nullptr) {
        puntuacionRonda += actual->datos.obtenerPuntuacion();
        actual = actual->siguiente;
    }
    jugador.puntuacionTotal += puntuacionRonda;
    return puntuacionRonda;
}

void inicializarFichas(JuegoDomino& juego) {
    for (int i = 0; i <= 6; ++i) {
        for (int j = i; j <= 6; ++j) {
            agregarFichaLista(juego.cabezaPozo, juego.colaPozo, juego.contadorPozo, FichaDomino(i, j));
        }
    }
}

void mezclarFichas(JuegoDomino& juego) {
    if (juego.contadorPozo == 0) return;

    Node* nodes[TOTAL_FICHAS]; 
    Node* actual = juego.cabezaPozo;
    int i = 0;
    while (actual != nullptr) {
        nodes[i++] = actual;
        actual = actual->siguiente;
    }
    
    // algoritmo Fisher-Yates (usando una semilla simple)
    int seed = 12345; 
    for (int j = 0; j < TOTAL_FICHAS; j++) {
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
        int indiceAleatorio = seed % TOTAL_FICHAS;

        swapFicha(nodes[j]->datos, nodes[indiceAleatorio]->datos);
    }
}

void repartirFichas(JuegoDomino& juego) {
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < juego.numJugadores; ++j) {
            if (juego.cabezaPozo != nullptr) {
                FichaDomino ficha = removerNodeLista(juego.cabezaPozo, juego.colaPozo, juego.contadorPozo, juego.cabezaPozo);
                Jugador& jugador = juego.jugadores[j];
                agregarFichaLista(jugador.cabezaFichas, jugador.colaFichas, jugador.contadorFichas, ficha);
            }
        }
    }
}

void reiniciarEstadoRonda(JuegoDomino& juego) {
    liberarLista(juego.cabezaMesa, juego.colaMesa, juego.contadorMesa);
    liberarLista(juego.cabezaPozo, juego.colaPozo, juego.contadorPozo);

    for (int i = 0; i < juego.numJugadores; ++i) {
        Jugador& jugador = juego.jugadores[i];
        liberarLista(jugador.cabezaFichas, jugador.colaFichas, jugador.contadorFichas);
        jugador.haPasado = false;
    }

    inicializarFichas(juego);
    mezclarFichas(juego);
    repartirFichas(juego);
}

// ---------------------------------------------------------------------

// muestra la ficha en formato [l1|l2]
void mostrarFicha(FichaDomino ficha) {
    if (ficha.lado1 == -1) {
        cout << "(N/A)";
        return;
    }
    cout << "[" << ficha.lado1 << "|" << ficha.lado2 << "]"; 
}

// muestra todas las fichas en la mano del jugador con su índice
void mostrarMano(const Jugador& jugador) {
    cout << "La mano de " << jugador.nombre << " --- (" << jugador.contadorFichas << " fichas): ";
    int indice = 1;
    Node* actual = jugador.cabezaFichas;
    while (actual != nullptr) {
        cout << "(" << indice++ << ")";
        mostrarFicha(actual->datos);
        cout << " ";
        actual = actual->siguiente;
    }
    cout << "\n" << "\n";
}

// muestra toda la cadena de fichas en la mesa
void mostrarCadenaMesa(const JuegoDomino& juego) {
    Node* actual = juego.cabezaMesa;
    if (actual == nullptr) {
        cout << " (Vacía) ";
        return;
    }
    while (actual != nullptr) {
        mostrarFicha(actual->datos);
        if (actual->siguiente != nullptr) {
            cout << " - ";
        }
        actual = actual->siguiente;
    }
}

int verificarGanadorTranca(JuegoDomino& juego, bool& empate) {
    int puntuacionMinima = 10000;
    int indiceGanador = -1;
    empate = false;
    
    for (int i = 0; i < juego.numJugadores; ++i) {
        int puntuacion = 0;
        Node* actual = juego.jugadores[i].cabezaFichas;
        while (actual != nullptr) {
            puntuacion += actual->datos.obtenerPuntuacion();
            actual = actual->siguiente;
        }
        
        if (puntuacion < puntuacionMinima) {
            puntuacionMinima = puntuacion;
            indiceGanador = i;
            empate = false;
        } else if (puntuacion == puntuacionMinima) {
            empate = true;
        }
    }
    return indiceGanador; 
}

// ---------------------------------------------------------------------

// logica de la ronda completa
int jugarRonda(JuegoDomino& juego) {
    reiniciarEstadoRonda(juego);
    juego.rondasJugadas++;

    cout << "\n\n===============================================" << "\n";
    cout << "=========== INICIO DE RONDA #" << juego.rondasJugadas << " ===========" << "\n";
    cout << "===============================================" << "\n";
    
    int ganadorRonda = -1;
    juego.indiceJugadorActual = determinarJugadorInicial(juego);
    
    if (juego.indiceJugadorActual != -1) {
        FichaDomino fichaInicial = jugarFichaInicial(juego, juego.indiceJugadorActual);
        cout << "\n¡" << juego.jugadores[juego.indiceJugadorActual].nombre << " inicia con ";
        mostrarFicha(fichaInicial);
        cout << "!" << "\n" << "\n";
        // el turno pasa al siguiente jugador después de la jugada inicial
        juego.indiceJugadorActual = (juego.indiceJugadorActual + 1) % juego.numJugadores; 
    } else {
        cerr << "Error: No se pudo determinar el jugador inicial." << "\n";
        return -1;
    }

    bool rondaFinalizada = false;
    while (!rondaFinalizada) {
        Jugador& jugadorActual = juego.jugadores[juego.indiceJugadorActual];

        manejarTurnoJugador(juego);

        // 1. condición de victoria (se quedó sin fichas)
        if (jugadorActual.contadorFichas == 0) {
            cout << "\n*** ¡FIN DE RONDA! " << jugadorActual.nombre << " ha jugado su última ficha. ***" << "\n";
            ganadorRonda = juego.indiceJugadorActual;
            rondaFinalizada = true;
        }
        
        // 2. condición de tranca
        int contadorPasadas = 0;
        for (int i = 0; i < juego.numJugadores; i++) {
            if (juego.jugadores[i].haPasado) {
                contadorPasadas++;
            }
        }
        
        if (contadorPasadas == juego.numJugadores) {
            cout << "\n*** ¡FIN DE RONDA! Se ha producido una **TRANCA** (todos pasaron). ***" << "\n";
            
            bool empate = false;
            ganadorRonda = verificarGanadorTranca(juego, empate);
            
            if (empate || ganadorRonda == -1) {
                cout << "¡Hay un empate en la tranca o no hay un ganador claro!" << "\n";
                ganadorRonda = -1; 
            } else {
                cout << juego.jugadores[ganadorRonda].nombre << " gana la tranca con el menor puntaje restante." << "\n";
            }
            
            rondaFinalizada = true;
        }
        
        if (rondaFinalizada) {
            cout << "\n--- Suma de Puntos de la Ronda ---" << "\n";
            for (int i = 0; i < juego.numJugadores; ++i) {
                int puntuacion = puntuarManoJugador(juego.jugadores[i]);
                cout << juego.jugadores[i].nombre << " sumó " << puntuacion << " puntos. Total Acumulado: " << juego.jugadores[i].puntuacionTotal << "\n";
            }
        }
        
        if (!rondaFinalizada) {
            // avanzar al siguiente jugador si la ronda no ha terminado
            juego.indiceJugadorActual = (juego.indiceJugadorActual + 1) % juego.numJugadores;
        }
    }
    
    return ganadorRonda;
}

// devuelve el índice del jugador con la menor puntuación total.
int obtenerGanadorGeneral(JuegoDomino& juego) {
    int indiceGanador = -1;
    int puntuacionTotalMinima = 100000;
    
    for (int i = 0; i < juego.numJugadores; ++i) {
        if (juego.jugadores[i].puntuacionTotal < puntuacionTotalMinima) {
            puntuacionTotalMinima = juego.jugadores[i].puntuacionTotal;
            indiceGanador = i;
        }
    }
    return indiceGanador; 
}

// controla el flujo de múltiples rondas.
void iniciarJuego(JuegoDomino& juego) {
    if (juego.numJugadores == 0) return;
    
    // 3 rondas
    for (int r = 1; r <= 3; ++r) {
        int ganadorRonda = jugarRonda(juego);
        
        if (r < 3) {
            // determinar quién inicia la siguiente ronda
            if (ganadorRonda != -1) {
                juego.indiceJugadorActual = ganadorRonda; // el ganador de la ronda inicia
            } else {
                // si tranca o empate, el jugador con menor puntaje total acumulado inicia
                int menorPuntuacionTotal = 100000;
                int jugadorInicio = 0;
                for (int i = 0; i < juego.numJugadores; ++i) {
                    if (juego.jugadores[i].puntuacionTotal < menorPuntuacionTotal) {
                        menorPuntuacionTotal = juego.jugadores[i].puntuacionTotal;
                        jugadorInicio = i;
                    }
                }
                juego.indiceJugadorActual = jugadorInicio;
            }
        }
    }
    
    cout << "\n\n===============================================" << "\n";
    cout << "============= ¡FIN DE LA PARTIDA! =============" << "\n";
    cout << "===============================================" << "\n";
    
    int ganadorGeneral = obtenerGanadorGeneral(juego);
    
    if (ganadorGeneral != -1) {
        int puntuacionFinal = juego.jugadores[ganadorGeneral].puntuacionTotal;
        cout << "\n*** ¡EL GANADOR ES " << juego.jugadores[ganadorGeneral].nombre << " con " << puntuacionFinal << " puntos! ***" << "\n";
    } else {
        cout << "La partida terminó en un empate total o no hubo un ganador claro." << "\n";
    }
}

// === LOGICA DE TURNO ===

int obtenerValorExtremoIzquierdo(Node* cabezaMesa) {
    if (cabezaMesa == nullptr) return -1;
    return cabezaMesa->datos.lado1;
}

int obtenerValorExtremoDerecho(Node* colaMesa) {
    if (colaMesa == nullptr) return -1;
    return colaMesa->datos.lado2;
}

bool esJugable(FichaDomino ficha, int valorIzquierdo, int valorDerecho) {
    if (ficha.lado1 == valorIzquierdo || ficha.lado2 == valorIzquierdo) return true;
    if (ficha.lado1 == valorDerecho || ficha.lado2 == valorDerecho) return true;
    return false;
}

int encontrarFichasJugables(Jugador& jugador, int valorIzquierdo, int valorDerecho, int indicesJugables[]) {
    int contador = 0;
    int indice = 1;
    Node* actual = jugador.cabezaFichas;
    
    while (actual != nullptr) {
        if (esJugable(actual->datos, valorIzquierdo, valorDerecho)) {
            indicesJugables[contador++] = indice;
        }
        actual = actual->siguiente;
        indice++;
    }
    return contador;
}

bool intentarRobarPozo(JuegoDomino& juego, Jugador& jugador, int valorIzquierdo, int valorDerecho) {
    bool encontradaJugable = false;
    juego.ultimaFichaJugada = FichaDomino(-1, -1);

    while (juego.cabezaPozo != nullptr) {
        FichaDomino fichaRobada = removerNodeLista(juego.cabezaPozo, juego.colaPozo, juego.contadorPozo, juego.cabezaPozo);
        agregarFichaLista(jugador.cabezaFichas, jugador.colaFichas, jugador.contadorFichas, fichaRobada);
        juego.ultimaFichaJugada = fichaRobada;
        
        if (esJugable(fichaRobada, valorIzquierdo, valorDerecho)) {
            encontradaJugable = true;
            break; 
        }
    }
    return encontradaJugable;
}

FichaDomino ejecutarJugada(JuegoDomino& juego, Jugador& jugador, int indiceFicha, int extremoElegido) {
    Node* nodeAJugar = obtenerNodePorIndiceLista(jugador.cabezaFichas, jugador.contadorFichas, indiceFicha);
    if (nodeAJugar == nullptr) return FichaDomino(-1, -1);
    
    FichaDomino fichaJugada = removerNodeLista(jugador.cabezaFichas, jugador.colaFichas, jugador.contadorFichas, nodeAJugar);
    
    // si la mesa está vacía (solo ocurre si jugadaInicial es -1), usamos los lados de la ficha.
    // esto es un caso de guardia, ya que jugarFichaInicial debería manejar la primera jugada.
    if (juego.cabezaMesa == nullptr && juego.colaMesa == nullptr) {
        // la ficha es la primera
        Node* newNode = new Node;
        newNode->datos = fichaJugada;
        newNode->anterior = nullptr;
        newNode->siguiente = nullptr;
        juego.cabezaMesa = newNode;
        juego.colaMesa = newNode;
        juego.contadorMesa = 1;
        
        jugador.haPasado = false;
        return fichaJugada;
    }
    
    // continuar con la lógica normal si la mesa ya tiene fichas
    int valorCoincidencia;
    
    if (extremoElegido == 1) { // jugar a la izquierda (cabeza)
        valorCoincidencia = obtenerValorExtremoIzquierdo(juego.cabezaMesa);
        
        // si lado 1 coincide (ej: 6 vs [6|4]), rótala a ([4|6] vs 6). El 6 queda en lado 2
        if (fichaJugada.lado1 == valorCoincidencia && fichaJugada.lado2 != valorCoincidencia) { 
            swapInt(fichaJugada.lado1, fichaJugada.lado2);
        }
        
        Node* newNode = new Node;
        newNode->datos = fichaJugada;
        newNode->siguiente = juego.cabezaMesa;
        newNode->anterior = nullptr;

        juego.cabezaMesa->anterior = newNode;
        juego.cabezaMesa = newNode;
        juego.contadorMesa++;
        
    } else { // jugar a la derecha (cola)
        valorCoincidencia = obtenerValorExtremoDerecho(juego.colaMesa);
        
        // si lado 2 coincide (ej: [4|6] vs 6), rótala a (6 vs [6|4]). El 6 queda en lado 1
        if (fichaJugada.lado2 == valorCoincidencia && fichaJugada.lado1 != valorCoincidencia) {
            swapInt(fichaJugada.lado1, fichaJugada.lado2); 
        } 

        Node* newNode = new Node;
        newNode->datos = fichaJugada;
        newNode->anterior = juego.colaMesa;
        newNode->siguiente = nullptr;
        
        juego.colaMesa->siguiente = newNode;
        juego.colaMesa = newNode;
        juego.contadorMesa++;
    }
    
    jugador.haPasado = false;
    return fichaJugada;
}

int manejarTurnoJugador(JuegoDomino& juego) {
    Jugador& jugadorActual = juego.jugadores[juego.indiceJugadorActual];
    
    int valorIzquierdo = obtenerValorExtremoIzquierdo(juego.cabezaMesa);
    int valorDerecho = obtenerValorExtremoDerecho(juego.colaMesa);

    cout << "Cadena: "; mostrarCadenaMesa(juego); cout << "\n" << "\n";
    
    cout << "\n--- Turno de " << jugadorActual.nombre << " ---" << "\n";
    //cout << "Cadena: (I: " << // valorIzquierdo << "): ";
    //cout << " (D: " << valorDerecho << ")" << "\n";
    
    mostrarMano(jugadorActual);
    
    int indicesJugables[TOTAL_FICHAS];
    int contadorJugables = encontrarFichasJugables(jugadorActual, valorIzquierdo, valorDerecho, indicesJugables);
    
    // logica de 'pasar' o 'robar del pozo'
    if (contadorJugables == 0) {
        if (juego.numJugadores == 4) {
            cout << jugadorActual.nombre << " no tiene jugadas. **PASA** su turno." << "\n";
            jugadorActual.haPasado = true;
            return 0; 
        } else {
            cout << jugadorActual.nombre << " no tiene jugadas y debe tomar del pozo..." << "\n";
            
            if (intentarRobarPozo(juego, jugadorActual, valorIzquierdo, valorDerecho)) {
                cout << "Ficha tomada: ";
                mostrarFicha(juego.ultimaFichaJugada);
                cout << ". ¡Es jugable! Intenta jugar ahora." << "\n";

                contadorJugables = encontrarFichasJugables(jugadorActual, valorIzquierdo, valorDerecho, indicesJugables);
                
                if (contadorJugables == 0) {
                    cout << jugadorActual.nombre << " aún no puede jugar. Pasa." << "\n";
                    jugadorActual.haPasado = true;
                    return 0; 
                }
            } else {
                cout << "El pozo se ha agotado. **PASA** su turno." << "\n";
                jugadorActual.haPasado = true;
                return 0;
            }
        }
    }
    
    // selección y ejecución de jugada
    int indiceFicha = -1;
    
    cout << "Fichas jugables (índices): ";
    for (int i = 0; i < contadorJugables; i++) {
        cout << indicesJugables[i] << " ";
    }
    
    while (!arrayContiene(indicesJugables, contadorJugables, indiceFicha)) {
        cout << "\nSelecciona el número de la ficha a jugar: ";
        if (!(cin >> indiceFicha)) {
            cin.clear();
            indiceFicha = -1;
        }
    }

    Node* nodeAJugar = obtenerNodePorIndiceLista(jugadorActual.cabezaFichas, jugadorActual.contadorFichas, indiceFicha);
    int extremoElegido = 0;
    
    if (nodeAJugar != nullptr) {
        // verificar dónde puede jugar
        bool puedeIzquierda = (nodeAJugar->datos.lado1 == valorIzquierdo || nodeAJugar->datos.lado2 == valorIzquierdo);
        bool puedeDerecha = (nodeAJugar->datos.lado1 == valorDerecho || nodeAJugar->datos.lado2 == valorDerecho);
        
        if (puedeIzquierda && puedeDerecha) {
            while (extremoElegido != 1 && extremoElegido != 2) {
                cout << "¿En qué extremo quieres jugar? (1: Izquierda, 2: Derecha): ";
                if (!(cin >> extremoElegido)) {
                    cin.clear();
                    extremoElegido = 0;
                }
            }
        } else if (puedeIzquierda) {
            extremoElegido = 1;
        } else if (puedeDerecha) {
            extremoElegido = 2;
        } else {
            // no debería ocurrir si contadorJugables > 0
            return -1; 
        }
    } else {
        return -1;
    }

    cout << "\n" << "===============================================" << "\n" << "\n";
    
    FichaDomino fichaJugada = ejecutarJugada(juego, jugadorActual, indiceFicha, extremoElegido);
    
    cout << jugadorActual.nombre << " juega "; mostrarFicha(fichaJugada); cout << " al extremo ";
    if (extremoElegido == 1) {
        cout << "izquierdo" << "." << "\n" << "\n";;
    } else {
        cout << "derecho" << "." << "\n" << "\n";;
    }
    return 1;
}

// ---------------------------------------------------------------------

int main() {
    int numPlayers = 0;

    // 1. configuración de jugadores
    while (numPlayers < 2 || numPlayers > 4) {
        cout << "--- Juego de Dominó ---" << "\n";
        cout << "Introduce el número de jugadores (2 a 4): ";
        if (!(cin >> numPlayers)) {
            cin.clear();
            cin.ignore(256, '\n');
            numPlayers = 0;
        } else if (numPlayers < 2 || numPlayers > 4) {
            cout << "Número de jugadores no permitido. Debe ser entre 2 y 4." << "\n";
        }
    }

    JuegoDomino juego;
    juego.numJugadores = numPlayers;

    cout << "--- Nombres de Jugadores ---" << "\n";
    for (int i = 0; i < juego.numJugadores; ++i) {
        cout << "Introduce el nombre del Jugador " << i + 1 << ": ";
        cin >> juego.jugadores[i].nombre; 
    }
    
    // 2. iniciar el juego
    iniciarJuego(juego);

    return 0;
}