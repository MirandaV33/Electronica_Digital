/* 
La siguiente función calcula el largo de una cadena de caracteres. Reformularla para que la
funcionalidad sea resuelta por medio del uso de un puntero auxiliar en lugar de la variable n.
int strlen(char *s){
int n;
for (n=0; *s != '\0'; s++)
n++;
return n;
} 
*/

#include <stdio.h>

int strlen_con_punteros(char *s) {
    char *inicio = s;
    // Avanzar el puntero hasta el final de la cadena
    while (*s != '\0') {
        s++;
    }
    
    // La longitud es la resta de las direcciones
    return s - inicio; 
}

int main() {
    char mi_cadena[] = "Hola"; 
    printf("Cadena: %s\n", mi_cadena);
    printf("Longitud: %d\n", strlen_con_punteros(mi_cadena));
    return 0;
}