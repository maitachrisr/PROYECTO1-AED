struct Node{
    int value;
    Node * next;
};

bool isVoid(Node **p){
    if (!p) return true;
    return false;
}

int contar(Node *inicio) {
    int contador = 0;
    Node *actual = inicio;
    while (actual != NULL) {
        contador++;
        actual = actual -> next;
    }
    return contador;
}

// PILAS/COLAS

int pop(Node **p){
    if (!*p) return -1;
    Node *aux = *p;
    int value = aux -> value;
    *p = (*p) -> next;
    delete aux;
    return value;
}

int top(Node *p){
    if (!p) return -1;
    return p -> value;
}

void push(Node **p, int value){
    Node *newElement = new Node;
    newElement -> value = value;
    newElement -> next = *p;
    *p = newElement;
}

// LISTAS DINAMICAS

void insertarenlista(Node **p, int element, int index) {
    // 1 es cola, 0 es insercion
    Node *newElement = new Node;
    newElement -> value = element;
    if (!index){
        newElement -> next = * p;
        *p = newElement;
    }
    else{
        newElement -> next = NULL;
        Node *aux = *p;
        while(aux && aux -> next) aux = aux -> next;
        if (aux) aux -> next = newElement;
        else *p = newElement;
    }
    newElement = 0;
    delete newElement;
}