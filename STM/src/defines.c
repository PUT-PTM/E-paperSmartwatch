/*
 * defines.c
 *
 *  Created on: 09.06.2018
 *      Author: AD
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_hd44780.h"
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


    uint8_t customChar[] = {
        0x1F,    // xxx 11111
        0x11,    // xxx 10001
        0x11,    // xxx 10001
        0x11,    // xxx 10001
        0x11,    // xxx 10001
        0x11,    // xxx 10001
        0x11,    // xxx 10001
        0x1F     // xxx 11111
    };

    uint8_t filledChar[] = {
            0x1F,    // xxx 11111
			0x1F,    // xxx 10001
			0x1F,    // xxx 10001
			0x1F,    // xxx 10001
			0x1F,    // xxx 10001
			0x1F,    // xxx 10001
			0x1F,    // xxx 10001
			0x1F     // xxx 11111
        };

    int8_t list1[] = {
    			0x1F,
    			0x18,
    			0x14,
    			0x12,
    			0x11,
    			0x10,
    			0x10,
    			0x1F
            };
    int8_t list2[] = {
    			0x1F,
    			0x3,
    			0x5,
    			0x9,
    			0x11,
    			0x01,
    			0x01,
    			0x1F
            };
    int8_t list3[] = {
                0x1F,    // xxx 11111
    			0x1F,    // xxx 10001
    			0x1F,    // xxx 10001
    			0x1F,    // xxx 10001
    			0x1F,    // xxx 10001
    			0x1F,    // xxx 10001
    			0x1F,    // xxx 10001
    			0x1F     // xxx 11111
            };
void wlaczwszystko()
{
	/* czesc od blutuf */
	// wlaczenie taktowania wybranego portu
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		// wlaczenie taktowania wybranego uk豉du USART
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		// zezwolenie na przerwania od przepe軟ienia dla timera 3
		TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM5, ENABLE);

		GPIO_InitTypeDef  GPIO_InitStructure;
		/* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
		// konfiguracja linii Rx i Tx
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* przyciski */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //wejscie
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/*przyciski */
		// ustawienie funkcji alternatywnej dla pin闚 (USART)
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

		USART_InitTypeDef USART_InitStructure;
		// predkosc transmisji (mozliwe standardowe opcje: 9600, 19200, 38400, 57600, 115200, ...)
		USART_InitStructure.USART_BaudRate = 9600;
		// d逝go s這wa (USART_WordLength_8b lub USART_WordLength_9b)
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		// liczba bit闚 stopu (USART_StopBits_1, USART_StopBits_0_5, USART_StopBits_2, USART_StopBits_1_5)
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		// sprawdzanie parzystoi (USART_Parity_No, USART_Parity_Even, USART_Parity_Odd)
		USART_InitStructure.USART_Parity = USART_Parity_No;
		// sprz皻owa kontrola przep造wu (USART_HardwareFlowControl_None, USART_HardwareFlowControl_RTS, USART_HardwareFlowControl_CTS, USART_HardwareFlowControl_RTS_CTS)
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		// tryb nadawania/odbierania (USART_Mode_Rx, USART_Mode_Rx )
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		// konfiguracja
		USART_Init(USART3, &USART_InitStructure);

		// wlaczenie ukladu USART
		USART_Cmd(USART3, ENABLE);


		//struktura do konfiguracji kontrolera NVIC
		NVIC_InitTypeDef NVIC_InitStructure;
		// wlaczenie przerwania zwi頊anego z odebraniem danych (pozostale zrodla przerwan zdefiniowane sa w pliku stm32f4xx_usart.h)
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;

		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		// konfiguracja kontrolera przerwan
		NVIC_Init(&NVIC_InitStructure);
		// wlaczenie przerwan od ukladu USART
		NVIC_EnableIRQ(USART3_IRQn);

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/*blutuf*/


    //Rectangle for custom character
    //xxx means doesn't care, lower 5 bits are important for LCD

    //Initialize system
    SystemInit();

    //Initialize LCD 20 cols x 4 rows
    TM_HD44780_Init(20, 4);
    TM_HD44780_Puts(0, 0, "TEST test TESTS");
}
char ClearDisplay[20];
void ekrany(char* string)
{
	char Display[20];
	TM_HD44780_Clear();
		if (string[0] == '0')
		{
			TM_HD44780_Clear();
			strncpy(Display, string+2, strlen(string)-2);
			TM_HD44780_Puts(0, 0, Display);
			TM_HD44780_Puts(0, 1, string);
			strncpy(Display, ClearDisplay, 20);
		}
		else if (string[0] == '1')
			{
				div_t output;
				char bperc[3];
				strncpy(bperc, string+2, strlen(string)-3);
				int jakis;
				jakis = strtol(bperc, NULL, 10);
					output = div(jakis, 20);
					if (output.quot == 0)
					{
						TM_HD44780_CreateChar(0, &customChar[0]);
						TM_HD44780_PutCustom(0, 1, 0);
						TM_HD44780_PutCustom(1, 1, 0);
						TM_HD44780_PutCustom(2, 1, 0);
						TM_HD44780_PutCustom(3, 1, 0);
						TM_HD44780_PutCustom(4, 1, 0);
					}
					else if (output.quot == 1)
					{
						TM_HD44780_CreateChar(0, &customChar[0]);
						TM_HD44780_CreateChar(1, &filledChar[0]);
						TM_HD44780_CreateChar(2, &list1[0]);
						TM_HD44780_CreateChar(3, &list2[0]);
						TM_HD44780_PutCustom(0, 1, 1);
						TM_HD44780_PutCustom(1, 1, 0);
						TM_HD44780_PutCustom(2, 1, 0);
						TM_HD44780_PutCustom(3, 1, 0);
						TM_HD44780_PutCustom(4, 1, 0);
						TM_HD44780_PutCustom(6, 1, 2);
						TM_HD44780_PutCustom(7, 1, 3);
					}

					else if (output.quot == 2)
					{
						TM_HD44780_CreateChar(0, &customChar[0]);
						TM_HD44780_CreateChar(1, &filledChar[0]);
						TM_HD44780_PutCustom(0, 1, 1);
						TM_HD44780_PutCustom(1, 1, 1);
						TM_HD44780_PutCustom(2, 1, 0);
						TM_HD44780_PutCustom(3, 1, 0);
						TM_HD44780_PutCustom(4, 1, 0);
					}

					else if (output.quot == 3)
					{
						TM_HD44780_CreateChar(0, &customChar[0]);
						TM_HD44780_CreateChar(1, &filledChar[0]);
						TM_HD44780_PutCustom(0, 1, 1);
						TM_HD44780_PutCustom(1, 1, 1);
						TM_HD44780_PutCustom(2, 1, 1);
						TM_HD44780_PutCustom(3, 1, 0);
						TM_HD44780_PutCustom(4, 1, 0);
					}

					else if (output.quot == 4)
					{
						TM_HD44780_CreateChar(0, &customChar[0]);
						TM_HD44780_CreateChar(1, &filledChar[0]);
						TM_HD44780_PutCustom(0, 1, 1);
						TM_HD44780_PutCustom(1, 1, 1);
						TM_HD44780_PutCustom(2, 1, 1);
						TM_HD44780_PutCustom(3, 1, 1);
						TM_HD44780_PutCustom(4, 1, 0);
					}

					else if (output.quot == 5)
					{
						TM_HD44780_CreateChar(0, &customChar[0]);
						TM_HD44780_CreateChar(1, &filledChar[0]);
						TM_HD44780_PutCustom(0, 1, 1);
						TM_HD44780_PutCustom(1, 1, 1);
						TM_HD44780_PutCustom(2, 1, 1);
						TM_HD44780_PutCustom(3, 1, 1);
						TM_HD44780_PutCustom(4, 1, 1);
					}
				strncpy(Display, string+2, strlen(string)-2);
				TM_HD44780_Puts(0, 0, Display);
				strncpy(Display, ClearDisplay, 20);
			}
		else if (string[0] == '2')
			{

			}
		else if (strstr(string, "10")) {
				GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
				TM_HD44780_Clear();
				TM_HD44780_Puts(0, 0, "Bambosz to pedal");
			}
			else if(strstr(string, "20")) {
				TM_HD44780_Clear();
				TM_HD44780_Puts(0, 0, "Hehehehe");
				GPIO_ToggleBits(GPIOD, GPIO_Pin_13);

			}
			else if(strstr(string, "3")) {
				TM_HD44780_Clear();
				TM_HD44780_Puts(0, 0, "Zarcik :)");
				GPIO_ToggleBits(GPIOD, GPIO_Pin_14);

			}
		else if(strstr(string, "4")) {
			GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
			TM_HD44780_Clear();
			TM_HD44780_Puts(0, 0, "Artur to najle");
			TM_HD44780_Puts(0, 1, "pszy programista");
}
}
