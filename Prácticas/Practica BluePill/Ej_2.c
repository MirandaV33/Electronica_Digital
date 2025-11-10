typedef int            int32_t;
typedef short          int16_t;
typedef char           int8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

// Implementar un código C de bajo nivel para que el led se encienda y se apague (parpadee) de acuerdo a un retardo que también deberán crear.


//Defino dirección del reloj 
#define RCC_BASE               ((uint32_t) 0x40021000)
#define RCC_APB2ENR_offset     ((uint32_t)0x18)
// Puntero! 
#define RCC_APB2ENR            (*(volatile uint32_t*)(RCC_BASE + RCC_APB2ENR_offset))

//Defino direcciones de memoria de periféricos. 
#define GPIOC_BASE          ((uint32_t)0x40011000)
//Defino el offset del registro
#define GPIOC_CRH_OFFSET    ((uint32_t)0x04)
#define GPIOC_CRL_offset       ((uint32_t)0x00)
#define GPIOC_ODR_OFFSET        ((uint32_t)0x0C)

//Defino el registro
#define GPIOC_CRH              (*(volatile uint32_t*)(GPIOC_BASE + GPIOC_CRH_OFFSET))
#define GPIOC_ODR              (*(volatile uint32_t*)(GPIOC_BASE + GPIOC_ODR_OFFSET))
#define GPIOC_CRL              (*(volatile uint32_t*)(GPIOC_BASE + GPIOC_CRL_offset))


int main(void)
{
    // Codigo programable en FLASH
    //Activo el reloj del GPIO
    RCC_APB2ENR |= (1 << 4); //Pongo el bit 4 en 1

    // CONFIGURAR SOLO PC13 COMO SALIDA (SEGURO)
    GPIOC_CRH &= ~(0xF << 20);    // Limpiar bits 20-23
    GPIOC_CRH |= (0x3 << 20);     // MODE13 = 11 (50MHz)
    GPIOC_CRH |= (0x0 << 22);     // CNF13 = 00 (Push-pull)
    
    // ENCENDER LED (PC13 = LOW)
    GPIOC_ODR &= ~(1 << 13);
	
    while(1) {
        GPIOC_ODR ^= (1 << 13);
        
        // Delay ajustable
        for(volatile uint32_t i = 0; i < 500000; i++);
    }

	return 0;
}

#define SRAM_SIZE ((uint32_t) 0x00005000)
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef void(*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,										// 0x0000_0004 Reset
};
