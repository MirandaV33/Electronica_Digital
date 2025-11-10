// Implementar un código C de bajo nivel que lea el estado de un pin en particular haciendo
// polling y que muestre el resultado en un led. El estado del pin debe manejarse utilizando un
// cable conectado a 3.3 V o a masa (IMPORTANTE)

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
#define GPIOA_IDR               (*(volatile uint32_t*)(GPIOA_BASE + GPIO_IDR_OFFSET))

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
    GPIOA_CRL |= (0x1 << 2);
	
    while(1) {
       // POLLING: Leer el estado de PA0
        if ( (GPIOA_IDR & (1 << 0)) != 0 ) {
            // Si PA0 está en ALTO (3.3V) ENCENDEMOS el LED (PC13 = ALTO).
            GPIOC_BSRR = (1 << 13);
        } 
        else {
            // Si PA0 está en BAJO (GND) APAGAMOS el LED (PC13 = BAJO).
            GPIOC_BSRR = (1 << 13);
        }
    }
    // Un microcontrolador nunca debería salir de main
    // return 0;
}

#define SRAM_SIZE ((uint32_t) 0x00005000)
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef void(*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,										// 0x0000_0004 Reset
};
