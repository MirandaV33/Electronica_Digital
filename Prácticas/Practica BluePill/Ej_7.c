//Implementar un código C de bajo nivel que realice el parpadeo de un led a través del SysTick
// utilizando polling.

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

// Defino direcciones y registros del Systick 
#define SYSTICK_CSR  *(volatile int*)0xE000E010 // Encendemos el Systick
#define SYSTICK_RVR  *(volatile int*)0xE000E014 // Elegimos el numero desde el que queremos hacer a cuenta regresiva
#define SYSTICK_CVR  *(volatile int*)0xE000E018 // Muestra la cuenta en tiempo real. 

int main(void)
{
    //Activo el reloj del GPIO
    RCC_APB2ENR |= (1 << 4); //Pongo el bit 4 en 1

    // Configuro el de salida
    GPIOC_CRH &= ~(0xF << 20);    // Limpiar bits 20-23
    GPIOC_CRH |= (0x3 << 20);     // MODE13 = 11 (50MHz)
    GPIOC_CRH |= (0x0 << 22);     // CNF13 = 00 (Push-pull)

    // Arranca apagado
	GPIOC_ODR |= (1u << 13);

    //Configuramos el Systick 
    // Por defecto el reloj del CPU opera a 8MHz 
    // El bit 2, controla la VELOCIDAD del reloj, por lo que cambiando su estado podems modificar la velocidad a nuestro gusto. 
    // El bit 0 lo enciende o apaga
    // El bit 16 controla cuando para la cuenta regresiva

    // Cuentas: 
    // Estrategia: 1 tick = 1 microsegundo
    // Reloj CPU (HSI) = 8MHz.
    // Reloj SysTick = 8MHz / 8 = 1MHz (1,000,000 Hz)
    // Delay deseado = 0.5s = 500ms = 500,000µs
    SYSTICK_RVR = 500000 - 1; 
    SYSTICK_CVR = 0; //Limpiamos
    //Encendemos el Systick
    SYSTICK_CSR = (1 << 0);
    
    //Loop infinito
	for(;;){
        // POLLING 
        if ( (SYSTICK_CSR & (1 << 16)) != 0 ){
            // ¡SÍ! Han pasado 0.5 segundos.
            // La bandera se limpia sola al leer este registro.
            // Invertimos el estado del LED
            GPIOC_ODR ^= (1u << 13);
        }
    }
    return(0); 
}


#define SRAM_SIZE ((uint32_t) 0x00005000)
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef void(*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,										// 0x0000_0004 Reset
};
