// 5. Codificar un procedimiento que intercambie dos enteros, por medio de la utilización de
// punteros. Verificarlo mediante el llamado del mismo desde un código externo con impresión del  resultado (valores antes y después del intercambio).
#include <stdio.h>

void swap(int *a, int *b){
    int temp = *a; // 1. Guardamos el valor al que apunta 'a' (el valor de x)
    *a = *b;       // 2. Sobrescribimos el valor en 'a' con el valor de 'b'
    *b = temp;     // 3. Sobrescribimos el valor en 'b' con el valor guardado
}

int main(){
    int x = 1, y = 2;
    printf("Valores originales:\tx = %d, y = %d\n", x, y);
    
    // Pasamos las DIRECCIONES de x e y
    swap(&x, &y); 
    
    printf("Valores nuevos:\t\tx = %d, y = %d\n", x, y);
    return 0;
} 