/*
Implementar los ejercicios 5 y 8 de la guía “Primeros pasos con Blue Pill” utilizando la
estructura DEVMAP.

8. Implementar el punto 5 pero utilizando interrupciones. Es decir, en este caso, el botón externo debe generar la interrupción--> El objetivo es que el BOTÓN (un evento externo) sea el que "despierte" al CPU, 

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

#define ENA_IRQ(IRQ) {CTX->NVIC.REGs.ISER[((uint32_t)(IRQ) >> 5)] = (1 << ((uint32_t)(IRQ) & 0x1F));}
/* ¿Cómo funciona esto?
- CTX es un puntero a la estructura que mapea los registros del sistema.
- NVIC es una subestructura dentro de CTX que representa el controlador de interrupciones.
- REGs es otra subestructura dentro de NVIC que contiene los registros específicos.
- ISER es un array de registros que permiten habilitar interrupciones específicas.

ENTONCES:
ENA_IRQ(IRQ): Es un atajo para "Habilitar Interrupción".
	IRQ: Es el número de la interrupción (ej. IRQ_TIM2 = 28).
	CTX->NVIC.REGs.ISER[...]: Accede a los registros de habilitación del NVIC que acabamos de definir.

Las siguientes son MUY similares en funcionamiento, solo que deshabilitan o limpian las interrupciones.
*/
#define DIS_IRQ(IRQ) {CTX->NVIC.REGs.ICER[((uint32_t)(IRQ) >> 5)] = (1 << ((uint32_t)(IRQ) & 0x1F));}
#define CLR_IRQ(IRQ) {CTX->NVIC.REGs.ICPR[((uint32_t)(IRQ) >> 5)] = (1 << ((uint32_t)(IRQ) & 0x1F));}


/* Definimos un struct union para mapear los registros del sistema (NVIC, SYSTICK, etc.)
CTX: Es otro "mapa", pero este apunta a la dirección 0xE0000000.
	SYSTICK: Define la estructura del SysTick (CSR, RVR, CVR).
	NVIC (Nested Vectored Interrupt Controller): Define las señales de alarma para interrupciones
*/
struct {
	word_t reversed0[(0xe000e010-0xe0000000)/sizeof(word_t)];
	union {
		struct {
			uint32_t CSR;
			uint32_t RVR;
			uint32_t CVR;
			uint32_t CALIB;
		} REGs;
	} SYSTICK;
	word_t reversed1[(0xe000e100-0xe000e020)/sizeof(word_t)];
	union {
		struct {
			uint32_t ISER[8];
			uint32_t RES0[24];
			uint32_t ICER[8];
			uint32_t RES1[24];
			uint32_t ISPR[8];
			uint32_t RES2[24];
			uint32_t ICPR[8];
			uint32_t RES3[24];
			uint32_t IABR[8];
			uint32_t RES4[56];
			uint8_t  IPR[240];
			uint32_t RES5[644];
			uint32_t STIR;
		} REGs;
	} NVIC;
} volatile *const CTX = ((void *) 0xE0000000); // Finalmente, asignamos el puntero CTX a la dirección 0xE0000000


// Definimos las interrupciones que vamos a usar con etiquetas segun el MANUAL
enum IRQs {
	IRQ_DMA1CHN2  = 12,
	IRQ_ADC1_2	  = 18,
	IRQ_EXTI0     = 6,
	IRQ_TIM2	  = 28,
	IRQ_USART1	  = 37,
};

// Declaracion de funciones
int  main(void);
void EXTI0_IRQHandler(void);

int main(void)
{
	//Activamos los relojes 
	DEVMAP->RCC.REGs.APB2ENR |= (1 << (GPIOC + 2)); 
	DEVMAP->RCC.REGs.APB2ENR |= (1 << (GPIOA + 2));
    DEVMAP->RCC.REGs.APB2ENR |= (1 << 0); 

	// Habilito PINC como salida
	DEVMAP->GPIOs[GPIOC].REGs.CRH &= ~(0xF << 20);
	DEVMAP->GPIOs[GPIOC].REGs.CRH |= (0x3 << 20);

	// Habilito PINA0 como entrada con PULL-UP
	DEVMAP->GPIOs[GPIOA].REGs.CRL &= ~(0xF << 0);
	DEVMAP->GPIOs[GPIOA].REGs.CRL |= (0x8 << 0);
    
    // Activar la resistencia de PULL-UP interna
	DEVMAP->GPIOs[GPIOA].REGs.ODR |= (1 << 0); 

    (*(volatile uint32_t*)0x40010008) &= ~(0xF << 0); 
	// Conecto PA0 a la terminal EXTI0
    (*(volatile uint32_t*)0x40010400) |= (1 << 0); // EXTI_IMR (Desenmascarar línea 0)
    (*(volatile uint32_t*)0x4001040C) |= (1 << 0); // EXTI_FTSR (Flanco descendente en línea 0)
	
	// Habilitar la interrupción en el "Cerebro" 
	ENA_IRQ(IRQ_EXTI0); // Habilitar la interrupción número 6
	
	for(;;){

	}
	return 0;
}

void EXTI0_IRQHandler(void)
{
	DEVMAP->GPIOs[GPIOC].REGs.ODR ^= (1 << 13);
	(*(volatile uint32_t*)0x40010414) |= (1 << 0); 
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
