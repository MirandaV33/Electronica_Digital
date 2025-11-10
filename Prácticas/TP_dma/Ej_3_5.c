/*
Implementar los ejercicios 5 y 8 de la guía “Primeros pasos con Blue Pill” utilizando la
estructura DEVMAP.

5. Implementar un código C de bajo nivel que lea el estado de un pin en particular haciendo
polling y que muestre el resultado en un led. El estado del pin debe manejarse utilizando un
cable conectado a 3.3 V o a masa (IMPORTANTE). Tener en cuenta las
resistencias de pull-up y pull-down del pin de entrada

*/


//Defino el tamaño y direccion de memoria de la RAM
#define SRAM_SIZE		((uint32_t) 0x00005000)
#define SRAM_BASE		((uint32_t) 0x20000000)


// Calcula la dirección final de la SRAM para establecer ahí el puntero de la pila (stack).
#define STACKINIT		((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef int			   int32_t;
typedef short		   int16_t;
typedef char		   int8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

typedef void(*interrupt_t)(void);

//Definimos un struct union para mapear los registros de los periféricos
typedef union {
	uint8_t  byte[4];
	uint16_t hword[2];
	uint32_t word;
} word_t;

typedef word_t page[0x400/sizeof(uint32_t)];

// Memory map: lista de constante y estructuras que representan los periféricos del microcontrolador
// --> Serian como indices de los registros
enum {TIM2	= 0, TIM3  = 1, TIM4  = 2 };
enum {GPIOA = 0, GPIOB = 1, GPIOC = 2, GPIOD = 3, GPIOE = 4, GPIOF = 5 };
enum {DMA1	= 0 };
enum {CHN1	= 0, CHN2  = 1, CHN3  = 2, CHN4  = 3, CHN5	= 4, CHN6  = 5, CHN7 = 6, CHN8 = 7 };
enum {ADC1	= 0 };
struct {
	union {
		struct {
			uint32_t CR1;
			uint32_t CR2;
			uint32_t SMCR;
			uint32_t DIER;
			uint32_t SR;
			uint32_t EGR;
			uint32_t CCMR1;
			uint32_t CCMR2;
			uint32_t CCER;
			uint32_t CNT;
			uint32_t PSC;
			uint32_t ARR;
			uint32_t RES1;
			uint32_t CCR1;
			uint32_t CCR2;
			uint32_t CCR3;
			uint32_t CCR4;
			uint32_t BDTR;
			uint32_t DCR;
			uint32_t DMAR;
		} REGs;
		page reserved;
	} TIMs[3]; //Asignamos memorias para 3 timers (TIM2, TIM3, TIM4)

	word_t reserved1[(0x40002800-0x40000c00)/sizeof(word_t)];
	page RTC;
	page WWDG;
	page IWDG;
	word_t reserved2[(0x40003800-0x40003400)/sizeof(word_t)];
	page SPI2;
	word_t reserved3[(0x40004400-0x40003c00)/sizeof(word_t)];
	page USART[2];
	word_t reserved4[(0x40005400-0x40004c00)/sizeof(word_t)];
	page I2C[2];
	page USB;
	page USBCAN_SRAM;
	page bxCAN;
	word_t reserved5[(0x40006c00-0x40006800)/sizeof(word_t)];
	page BKP;
	page PWR;
	word_t reserved6[(0x40010000-0x40007400)/sizeof(word_t)];

	page AFIO;
	page EXTI;

	union {
		struct {
			uint32_t CRL;
			uint32_t CRH;
			uint32_t IDR;
			uint32_t ODR;
			uint32_t BSRR;
			uint32_t BRR;
			uint32_t LCKR;
		} REGs;
		page reserved;
	} GPIOs[5]; // Asignamos memorias para 5 puertos GPIO (GPIOA, GPIOB, GPIOC, GPIOD, GPIOE)
	word_t reserved7[(0x40012400-0x40011C00)/sizeof(word_t)];
	union {
		struct {
			uint32_t SR;
			uint32_t CR1;
			uint32_t CR2;
			uint32_t SMPR1;
			uint32_t SMPR2;
			uint32_t JOFR;
			uint32_t HTR;
			uint32_t LTR;
			uint32_t SQR1;
			uint32_t SQR2;
			uint32_t SQR3;
			uint32_t JSQR;
			uint32_t JDR;
			uint32_t DR;
		} REGs;
		page reserved;
	} ADC[2]; // Asignamos memorias para 2 ADC (ADC1, ADC2)
	page TIM1;
	page SPI1;
	word_t reserved8[(0x40013800-0x40013400)/sizeof(word_t)];
	union  {
		struct {
			uint32_t SR;
			uint32_t DR;
			uint32_t BRR;
			uint32_t CR1;
			uint32_t CR2;
			uint32_t CR3;
			uint32_t GTPR;
		} REGs;
		page reserved;
	} USART1;
	word_t reserved9[(0x40020000-0x40013C00)/sizeof(word_t)];
	union {
		struct {
			uint32_t ISR;
			uint32_t IFCR;
			struct {
				uint32_t CCR;
				uint32_t CNDTR;
				uint32_t CPAR;
				uint32_t CMAR;
				uint32_t RESERVED;
			} CHN[8];
		} REGs;
		page reserved;
	} DMAs[1]; // Asignamos memoria para 1 DMA (DMA1)
	word_t reservedA[(0x40021000-0x40020400)/sizeof(word_t)];

	union {
		struct {
			uint32_t CR;
			uint32_t CFGR;
			uint32_t CIR;
			uint32_t APB2RSTR;
			uint32_t APB1RSTR;
			uint32_t AHBENR;
			uint32_t APB2ENR;
			uint32_t APB1ENR;
			uint32_t BDCR;
			uint32_t CSR;
			uint32_t AHBRSTR;
			uint32_t CFGR2;
		} REGs;
		page reserved;
	} RCC; // Asignamos memoria para el RCC
	word_t reservedB[(0x40022000-0x40021400)/sizeof(word_t)];

	union {
		struct {
			uint32_t ACR;
			uint32_t KEYR;
			uint32_t OPTKEYR;
			uint32_t SR;
			uint32_t CR;
			uint32_t AR;
			uint32_t reserved;
			uint32_t OBR;
			uint32_t WRPR;
		} REGs;
		page reserved;
	} FLASH; // Asignamos memoria para el FLASH
} volatile *const DEVMAP = (void *) 0x40000000;// Esto es lo mas improtante! Este puntero asigna TODO el mapa de memoria de los perifericos a la variable DEVMAP!
// Se usa 0x40000000, que es donde todos los periféricos

// Declaracion de funciones
int  main(void);

int main(void){

    // Habilitar reloj para el LED (Puerto C)
	DEVMAP->RCC.REGs.APB2ENR |= (1 << (GPIOC + 2)); 
	
	// Habilitar reloj para el botón (Puerto A)
	DEVMAP->RCC.REGs.APB2ENR |= (1 << (GPIOA + 2));

    // Configurar como Salida Push-Pull 50MHz (MODE=11, CNF=00 -> 0x3)
	DEVMAP->GPIOs[GPIOC].REGs.CRH &= ~(0xF << 20);
	DEVMAP->GPIOs[GPIOC].REGs.CRH |= (0x3 << 20);

    // Configurar como Entrada con Pull-up/down (MODE=00, CNF=10 -> 0x8)
	DEVMAP->GPIOs[GPIOA].REGs.CRL &= ~(0xF << 0);
	DEVMAP->GPIOs[GPIOA].REGs.CRL |= (0x8 << 0);
    
    // Activar la resistencia de PULL-UP interna

	DEVMAP->GPIOs[GPIOA].REGs.ODR |= (1 << 0);
	for(;;){
		//PAO = 0  -> Botón presionado -> Enciendo LED
 		if ( (DEVMAP->GPIOs[GPIOA].REGs.IDR & (1 << 0)) == 0 ){
			DEVMAP->GPIOs[GPIOC].REGs.BSRR = (1 << 13);
	 	} 
		else 
        {
			// PA0 = 1 -> Botón no presionado -> Apago LED
			DEVMAP->GPIOs[GPIOC].REGs.BSRR = (1 << (13 + 16));
		}
	}
}

#define SRAM_SIZE ((uint32_t) 0x00005000)		
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef void(*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,
};
