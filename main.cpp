#include <iostream>
#include "lib.h"

using namespace std;

// IDEA
// realizar un dominó con c++
// asegurarnos que sea lo más eficiente y que tenga buen rendimiento
// publicar los cambios a GIT

// 0.0.0.4 - IMPLEMENTACIÓN DE LIBRERÍAS Y FUNCIONES GLOBALES - acá pongan su progreso y un breve resumen de lo que hicieron

// --FUNCIONES GLOBALES PARA LAS COLAS-- 

void pushBottom(Node **p, int value){
    if (isVoid(p)) push(p, value);
    else{
        int currentValue = pop(p);
        pushBottom(p, value);
        push(p, currentValue);
    }
}

// bool compare(Node **A, Node **B){
    // if (isVoid(A) && isVoid(B)) return true;
    // if (isVoid(A) || isVoid(B)) return false;
    // int valueA = pop(A), valueB = popBottom(B);
    // bool result = false;
    // if(valueA == valueB) result = compare(A, B);
    // push(A, valueA);
    // pushBottom(B, valueB);
    // return result;
// }

int popBottom(Node **p){
    if (isVoid(p)) return -1;
    int value = pop(p);
    int result = popBottom(p);
    if (result == -1) return value;
    push(p, value);
    return result;
}


int main(){
    cout<<"Hola mundo, acá la función principal";



    return 0;
}