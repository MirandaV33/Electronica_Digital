// Implementar el punto 5 pero utilizando interrupciones.
// Es decir, en este caso, el botón externo debe generar la interrupción--> El objetivo es que el BOTÓN (un evento externo) sea el que "despierte" al CPU, 


typedef int            int32_t;
typedef short          int16_t;
typedef char           int8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

//Defino dirección del reloj 
#define RCC_BASE               ((uint32_t) 0x40021000)
#define RCC_APB2ENR_offset     ((uint32_t)0x18)
// Puntero! 
#define RCC_APB2ENR            (*(volatile uint32_t*)(RCC_BASE + RCC_APB2ENR_offset))

//Defino direcciones de memoria de periféricos C 
#define GPIOC_BASE              ((uint32_t)0x40011000)

//Defino el offset del registro
#define GPIO_CRH_OFFSET        ((uint32_t)0x04)
#define GPIO_CRL_offset        ((uint32_t)0x00)
#define GPIO_ODR_OFFSET        ((uint32_t)0x0C)
#define GPIO_IDR_OFFSET         ((uint32_t)0x08) 
#define GPIO_BSRR_OFFSET        ((uint32_t)0x10) 

//Defino el registro
#define GPIOC_CRH              (*(volatile uint32_t*)(GPIOC_BASE + GPIO_CRH_OFFSET))
#define GPIOC_ODR              (*(volatile uint32_t*)(GPIOC_BASE + GPIO_ODR_OFFSET))
#define GPIOC_CRL              (*(volatile uint32_t*)(GPIOC_BASE + GPIO_CRL_offset))
#define GPIOC_BSRR             (*(volatile uint32_t*)(GPIOC_BASE + GPIO_BSRR_OFFSET))

//Defino direcciones de memoria de periféricos A 
#define GPIOA_BASE              ((uint32_t)0x40010800) 

//Defino el registro del puerto A
#define GPIOA_CRL              (*(volatile uint32_t*)(GPIOA_BASE + GPIO_CRL_offset))
#define GPIOA_IDR              (*(volatile uint32_t*)(GPIOA_BASE + GPIO_IDR_OFFSET))
#define GPIOA_ODR              (*(volatile uint32_t*)(GPIOA_BASE + GPIO_ODR_OFFSET))

// Función alternativa, para eventos externos como entradas
#define AFIO_BASE               ((uint32_t)0x40010000)
#define AFIO_EXTICR1            (*(volatile uint32_t*)(AFIO_BASE + 0x08))

// Detectores de interrupcion (con OFFSET)
#define EXTI_IMR        (*(volatile uint32_t*)0x40010400) 
#define EXTI_FTSR       (*(volatile uint32_t*)0x4001040C) 
#define EXTI_PR         (*(volatile uint32_t*)0x40010414) 

//Interruptor 
#define NVIC_ISER0              (*(volatile uint32_t*)0xE000E100)

//Declaracion de funciones
int main(void);
void EXTI0_IRQHandler(void);

int main(void)
{
    // Codigo programable en FLASH
    //Activo el reloj del GPIOC
    RCC_APB2ENR |= (1 << 4); //Pongo el bit 4 en 1
    // Activo reloj del GPIOA
    RCC_APB2ENR |= (1 << 2);
    // Habilito el reloj del AFIO
    RCC_APB2ENR |= (1 << 0); 

    // CONFIGURAR SOLO PC13 COMO SALIDA (SEGURO)
    GPIOC_CRH &= ~(0xF << 20);    // Limpiar bits 20-23
    GPIOC_CRH |= (0x3 << 20);     // MODE13 = 11 (50MHz)
    GPIOC_CRH |= (0x0 << 22);     // CNF13 = 00 (Push-pull)
    
    // CONFIGURAR PA0 COMO ENTRADA
    GPIOA_CRL &= ~(0xF << 0);      // Limpiar bits 0-3 (CNF0 y MODE0)
    // importante! Configuramos CRL para que lo reconozca como input con pull-up/pull-down, es decir que use alguna de sus resistecias
    GPIOA_CRL |= (0x8 << 0);
    //Activamos el pull-up
    GPIOA_ODR |= (1 << 0);
	
    // Conectamos PA0 a la temrinal EXIT
    AFIO_EXTICR1 &= ~(0xF << 0);
    
    // Configuro las salidas
    EXTI_IMR |= (1 << 0);
    EXTI_FTSR |= (1 << 0);
    // FALTA desenmascaras las interruociones Event Mask Register (EXTI_ETR)
    // Habilito interrupciones
    NVIC_ISER0 |= (1 << 6);

    for(;;){
        //Esta vacío, porque espera que la interrupcion ocurra. 
    }
}

void EXTI0_IRQHandler(void)
{
    GPIOC_ODR ^= (1 << 13);
    EXTI_PR |= (1 << 0);
}

#define SRAM_SIZE ((uint32_t) 0x00005000)
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef void(*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,  // 0x0000_0004 Reset
    0, 0, 0, 0, 
    //NVIC --> mapa de vectores. EXT deberia estar en posicion 58 aprox 
    EXTI0_IRQHandler, //Tabla de vectores
    // Esto es lo importante, la funcion de EXTI0_IRQHandler se usa cuanod se reconoce una interrupcion en NVIC pausando inmediatamente el main y desde la lista de vextores, se va a la funcion
    0,0,0,0,0,0,0,0,0,
};



// WORKFLOW!

// 1.El CPU arranca y salta a main (porque está en la Posición 1 de la tabla).
// 2. main configura todo (GPIO, AFIO, EXTI, NVIC).
// 3. main se queda "atrapado" en el for(;;) vacío.
// 4. Tú presionas el botón.
// 5. El Hardware (EXTI y NVIC) lo detecta.
// 6. El Hardware pausa main.
// 7. El Hardware mira la Tabla de Vectores en la posición #6.
// 8. El Hardware ve la dirección de EXTI0_IRQHandler y salta a esa función.
// 9. Se ejecuta tu código (el toggle del LED y la limpieza de la bandera).
// 10. La función EXTI0_IRQHandler termina.
// 11. El Hardware regresa automáticamente a main, donde se había quedado (en el for(;;) vacío), y sigue esperando la próxima presión.

