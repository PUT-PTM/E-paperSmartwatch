/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
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
#include <unistd.h>

/* czesc od blutuf */
char c = 'a';
uint8_t u = 'a';
int j=0;


char Buffer[100];
char BufferCommand[100];
int i=0;

uint8_t usartGetChar(void)
{
    // czekaj na odebranie danych
       while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);

       return USART_ReceiveData(USART3);
}

void wifi_readReply(char* string){
	ekrany(string);
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		char temp=USART_ReceiveData(USART3);

			// po napotkaniu znaku konca linii na koncu wiadomosci jest umieszczany znak '\0'
			// odpowiedz jest analizowana, bufor jest resetowany

		if (temp == '\n') {
			Buffer[i]='\0';
			wifi_readReply(Buffer);
			i=0;
			}
				// znaki '\r' sa ignorowane, a pozostale wpisywane do bufora
		else if(temp != '\r'){
			Buffer[i++]=temp;
			if(i>=200) i=0;
			}

		}
}

void USART_put(char* s){
	int i=0;
	while(s[i]){
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE)==RESET);
		USART_SendData(USART3, s[i++]);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	}
	for(int i=0;i<10000000; i++);
}

// wyslanie pojedynczego znaku do modulu wifi
void USART_put_char(char c){
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE)==RESET);
	USART_SendData(USART3, c);
while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}
/*blutuf*/
int main(void) {



	wlaczwszystko();
    	char *pom;
    	for(;;)
    	{
    		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))
      	    {
    			GPIO_SetBits(GPIOD, GPIO_Pin_12); //zielona
    		    j++;
   		    }
    		else
    		{
   	    		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
    		}
    		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2))
       			{
    				GPIO_SetBits(GPIOD, GPIO_Pin_13);
       				j--;
       			}
    		else
    		{
   	    		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    		}
    	if (j==0)
    		    {
    		    	pom = "time\n";
    		    	USART_put(pom);
    		    }
    	else if (j==1)
    		    {
    		    	pom = "battery\n";
    		    	USART_put(pom);
    		    }
    	else if (j==2)
    		    {
    		    	pom = "sound\n";
    		    	USART_put(pom);
    		    	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3))
    		    	      	    {
    		    	    			GPIO_SetBits(GPIOD, GPIO_Pin_14); //niebieska?
    		    	    			pom = "chsound\n";
    		    	    			USART_put(pom);
    		    	   		    }
    		    }
    	else if (j==3)
    	{
    		pom = "nots\n";
    		USART_put(pom);
    	}
    	}




}
