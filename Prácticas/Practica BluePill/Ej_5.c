// Repetir el punto anterior pero en lugar de usar un cable incluir un botón. Tener en cuenta las
// resistencias de pull-up y pull-down del pin de entrada

//¿Cómo lo hago? 
// Un boton es un interruptor. Si está abierto funciona como un circuito abierto: Los contactos internos no se tocan. Esto es exactamente igual a dejar el cable "al aire"
// Si está cerrado El circuito se cierra. Los contactos internos se tocan. Sin embargo, el boton por si solo no funciona. 
// Ya que al no estar presionado, el pin flor porque no está conectado a nada, para eso es necesario darle un "estado por defecto" y esto se hace Esto se hace con una resistencia (la interna del pin)
// Dos formas: 
// ---> Pull down: Conectamos una resistencia (ej. 10kΩ) entre PA0 y GND. Esto "tira" del pin hacia abajo. Cuando se apreta el boton pasa de 0 a 1
// ---> Pull up: Conectamos una resistencia (ej. 10kΩ) entre PA0 y 3.3V. Esto "tira" del pin hacia arriba. Cuando se aprieta el boton pasa de 1 a 0 

// Importante, la direccion del los GPIOs cuando queremos activar las resistencias internas ES DISTINTO! 
// Direccion normal de pines 0x4
// Direccion con resistencias de pines 0x8


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

int main(void)
{
    // Codigo programable en FLASH
    //Activo el reloj del GPIOC
    RCC_APB2ENR |= (1 << 4); //Pongo el bit 4 en 1
    // Activo reloj del GPIOA
    RCC_APB2ENR |= (1 << 2);

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
	
    //Misma logica ejercicio 4, invertimos ahora leemos si etsa en 0 
    while(1) {
        if ( (GPIOA_IDR & (1 << 0)) == 0 ) {
            // Botón PRESIONADO (conectado a GND)
            // ENCENDEMOS el LED (PC13 = ALTO).
            GPIOC_BSRR = (1 << 13);
        } 
        else {
            // Botón NO presionado (Pull-up lo mantiene en ALTO)
            // APAGAMOS el LED (PC13 = BAJO).
            GPIOC_BSRR = (1 << (13 + 16));
    }
    // Un microcontrolador nunca debería salir de main
    // return 0;
    }
}

#define SRAM_SIZE ((uint32_t) 0x00005000)
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef void(*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,										// 0x0000_0004 Reset
};

