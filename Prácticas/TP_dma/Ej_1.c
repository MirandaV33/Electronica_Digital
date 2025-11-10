/*
1. Implementar un código C de bajo nivel (sin utilizar las funciones de biblioteca), utilizando
DMA, que genere una salida por el led de la placa de tal manera que muestre un patrón
distinto al simple parpadeo.
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
	IRQ_TIM2	  = 28,
	IRQ_USART1	  = 37,
};

// Declaracion de funciones
int  main(void);
void handler_systick(void);
void handler_dma1chn2(void);
void handler_adc1_2(void);
void handler_tim2(void);
void handler_usart1(void);


// Establecemos la tabla de vectores de interrupción DE HARDWARE, el Cortex-M3 está "cableado" para buscar en esta dirección exacta cuando se enciende o cuando ocurre una interrupción. 
// El hardware espera encontrar cosas específicas en posiciones (índices) específicas de este array. 
const interrupt_t vector_table[] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,										// 0x0000_0004 Reset
	0,														// 0x0000_0008
	0,														// 0x0000_000C
	0,														// 0x0000_0010
	0,														// 0x0000_0014
	0,														// 0x0000_0018
	0,														// 0x0000_001C
	0,														// 0x0000_0020
	0,														// 0x0000_0024
	0,														// 0x0000_0028
	0,														// 0x0000_002C
	0,														// 0x0000_0030
	0,														// 0x0000_0034
	0,														// 0x0000_0038
	0,														// 0x0000_003C
	0,														// 0x0000_0040
	0,														// 0x0000_0044
	0,														// 0x0000_0048
	0,														// 0x0000_004C
	0,														// 0x0000_0050
	0,														// 0x0000_0054
	0,														// 0x0000_0058
	0,														// 0x0000_005C
	0,														// 0x0000_0060
	0,														// 0x0000_0064
	0,														// 0x0000_0068
	0,														// 0x0000_006C
	handler_dma1chn2,										// 0x0000_0070 DMA1_CHN2
	0,														// 0x0000_0074
	0,														// 0x0000_0078
	0,														// 0x0000_007C
	0,														// 0x0000_0080
	0,														// 0x0000_0084
	0,														// 0x0000_0088
	0,														// 0x0000_008C
	0,														// 0x0000_0090
	0,														// 0x0000_0094
	0,														// 0x0000_0098
	0,														// 0x0000_009C
	0,														// 0x0000_00A0
	0,														// 0x0000_00A4
	0,														// 0x0000_00A8
	0,														// 0x0000_00AC
	handler_tim2,											// 0x0000_00B0 TIM2
};

void handler_dma1chn2(void)
{
//	DEVMAP->GPIOs[GPIOC].REGs.ODR ^= -1;
// El devmap una estructura gigante que representa todo el bloque de memoria donde viven los periféricos
// Dentro de DEVMAP, hay sub-estructuras para cada "piso" (Timers, GPIOs, ADC, DMA, RCC, etc.). Y dentro de cada piso, están las "oficinas" (los registros específicos).
	DEVMAP->DMAs[DMA1].REGs.IFCR |= (0xf << 1);
	CLR_IRQ(IRQ_DMA1CHN2);
}

void handler_tim2(void)
{
	DEVMAP->TIMs[TIM2].REGs.SR &= ~(1 << 0);
	CLR_IRQ(IRQ_TIM2);
}

// One cycle first order sigma delta sin signal

//Es un array (vector) constante (const) de 256 números de 32 bits (uint32_t). Está guardado en la memoria FLASH del micro.
// Es la lista de tareas que le vamos a mandar al DMA para que las ejecute automáticamente.
uint32_t const data[256] = {
	0x00000000, 0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00000000, 0x00002000, 0x00000000,
	0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00000000,
	0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00002000, 0x00000000, 0x00002000,
	0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00000000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00000000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00000000, 0x00002000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00002000,
	0x00002000, 0x00002000, 0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00002000,
	0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00002000,
	0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00000000,
	0x00002000, 0x00000000, 0x00002000, 0x00002000, 0x00000000, 0x00002000, 0x00000000, 0x00002000,
	0x00000000, 0x00002000, 0x00000000, 0x00002000, 0x00000000, 0x00002000, 0x00000000, 0x00000000,
	0x00002000, 0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00000000,
	0x00002000, 0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x00002000,
	0x00000000, 0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00002000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00002000, 0x00000000,
	0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x00002000, 0x00000000,
	0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00002000,
	0x00000000, 0x00000000, 0x00002000, 0x00000000, 0x00002000, 0x00000000, 0x00002000, 0x00000000};


int main(void)
{

	// Configuracion del RELOJ
	DEVMAP->RCC.REGs.CR   |= (1 << 16);	// Habilita el clk externo de alta velocida 			// Enable HSE
	while (!(DEVMAP->RCC.REGs.CR & (1 << 17)));	

	DEVMAP->RCC.REGs.CR   &= ~(1 << 24); // Deshabilita el pll		
	DEVMAP->RCC.REGs.CFGR |= (0b0111 << 18); // Establece el multiplicador del pll en 9			// Set PLLMULL to 9. Set PLL output clock to 72 Mhz
	DEVMAP->RCC.REGs.CFGR |=  (1 << 16); // Selecciona el hse como fuente de reloj del pll
	DEVMAP->RCC.REGs.CR   |=  (1 << 24); // Enable PLL
	while (!(DEVMAP->RCC.REGs.CR & (1 << 25)));	// Wait for PLL este listo

	DEVMAP->FLASH.REGs.ACR |= (0b010 << 0);	// Set FLASH WAIT STATE to 2
	DEVMAP->RCC.REGs.CFGR  |= (0b0000 << 4); // Set AHB HPRE division to 1. Set AHB clock to 72 Mhz
	DEVMAP->RCC.REGs.CFGR  |= (0b100 << 8);	// Set APB1 PPRE1 division to 2. Set AHB clock to 36 Mhz

	DEVMAP->RCC.REGs.CFGR |= (0b10 << 0);					// Select PLL clock as the system clock
	while (!(DEVMAP->RCC.REGs.CFGR & (0b10 << 2)));			// Wait for PLL clock to be selected

	// Configuracion Perifericos (DMA y Timer). Se encienden los perifericos que se van a usar
	DEVMAP->RCC.REGs.APB2ENR |= (1 << 4); // Enable GPIOC clock.
	DEVMAP->RCC.REGs.APB1ENR |= (1 << 0); // Enable TIM2 clock.
	DEVMAP->RCC.REGs.AHBENR  |= (1 << 0); // Enable DMA1 clock.

    // Se configura todo el puerto C como salida: VEO SEÑAL DE SALIDA
	DEVMAP->GPIOs[GPIOC].REGs.CRL  = 0x33333333;			// Make low GPIOC output
	DEVMAP->GPIOs[GPIOC].REGs.CRH  = 0x33333333;			// Make high GPIOC output
    //	DEVMAP->GPIOs[GPIOC].REGs.ODR ^= -1;

    // Configuracion del canal 2 de la DMA  
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CNDTR = sizeof(data)/sizeof(uint32_t); // Tamaño de datos a transferir
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CMAR	= (uint32_t) data;	// direccion de memoria de los datos
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CPAR	= (uint32_t) &DEVMAP->GPIOs[GPIOC].REGs.ODR; // direccion de salida (puerto c)

    // Configuracion del comportamiento del canal
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR  = 0;				// Reset CCR
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR &= ~(1 << 14);	// Disable memory to memory transfer on DMA1 channel 2
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |=  (0b11 << 12); // PRIORIDAD ALTA
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |=  (0b10 << 10); // Tamaño de datos a transferir: 32-bits
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |=  (0b10 << 8);	// Set peripheral transfer size to 32-bits
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |=  (1 << 7);		// desp de cada transfferencia se apunta al siguiente a transferir
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR &= ~(1 << 6);		// Disable peripheral increment mode
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |=  (1 << 5);		// cuando se llega al final del array vuelve a inciar
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |=  (1 << 4);		// Read from memory
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |=  (1 << 2);		// se habilitan medias interrupciones -> AVISA CUANDO LELGA A LA MITAD DE LA TRASNFERENCIA
	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |=  (1 << 1);		// interrupciones completas -> LE AVISA A LA CPU CUANDO TRANSFIIO TODOS LOS DATOS
	ENA_IRQ(IRQ_DMA1CHN2);									// Enable DMA1 Channel2 inturrupt on NVIC

	DEVMAP->DMAs[DMA1].REGs.CHN[CHN2].CCR |= (1 << 0);		// Enable DMA

   // CONFIGURACION TIMER 2 
	ENA_IRQ(IRQ_TIM2);										// Enable TIM2 interrupt on NVIC
	DEVMAP->TIMs[TIM2].REGs.CR1  = 0x0000;					// Reset CR1 just in case
//	DEVMAP->TIMs[TIM2].REGs.CR1  |= (1 << 4);				// Down counter mode

    // PRESCALER
        // registro divide la frecuencia del reloj principal para hacer que el temporizador cuente más lento.
	DEVMAP->TIMs[TIM2].REGs.PSC   = (72e6/8)/(sizeof(data)/sizeof(data[0]))-1;	// fCK_PSC / (PSC[15:0] + 1)
                                                    // data se definio con 256 elementos, c/u es un entero que ocupa 4 bytes
                                                    // entonces 256x4 = 1024 bytes
                                                    // sizeof(data[0]) da el tamaño de un solo int = 4 bytes
                                                    // (Tamaño total del arreglo) / (Tamaño de un elemento) = Número de elementos
                                                    // = 256 en este caso
                                                    // CALCULA EL NUMERO DE ELEMENTOS
                                // psa = 35155
    // Frecuencia Intermedia = 72,000,000 / 35,156 ≈ 2,048 Hz
	DEVMAP->TIMs[TIM2].REGs.ARR   = 8-1; // -> segundo divisor de frecuencia para bajar velocidad
    // Frecuencia Final = 2,048 Hz / 8 = 256 Hz

	DEVMAP->TIMs[TIM2].REGs.DIER |= (1 << 14);				// Trigger DMA request enable
	DEVMAP->TIMs[TIM2].REGs.DIER |= (1 <<  8);	// Sollicitud del dma cuando llega al final
//	DEVMAP->TIMs[TIM2].REGs.DIER |= (1 <<  6);				// Enable interrupt
//	DEVMAP->TIMs[TIM2].REGs.DIER |= (1 <<  0);				// Update interrupt enable

	DEVMAP->TIMs[TIM2].REGs.CR1  |= (1 << 0);				// Finally enable TIM1 module


	for(;;);

	return 0;
}
