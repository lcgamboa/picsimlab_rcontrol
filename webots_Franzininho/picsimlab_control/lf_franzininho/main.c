
#include <avr/io.h>
#include <util/delay.h> 

#define F_CPU 16500000L

 	
#define setBit(valor,bit) (valor |= (1<<bit))
#define clearBit(valor,bit) (valor &= ~(1<<bit))
#define toogleBit(valor,bit) (valor ^= (1<<bit))
#define testBit(valor,bit)    (valor & (1<<bit))


#define Speed      120        // MAX 255
#define mspeed     PB0
#define motor1     PB1
#define motor2     PB3
#define sesquerda  PB2
#define sdireita   PB4


/**
 * @brief Função main
 * 
 * @return int 
 */
int main(void) 
{
 //seguidor linha
  unsigned char left_value;
  unsigned char right_value;

  setBit(DDRB,mspeed); 	// Configura Pino como saída ()
  setBit(DDRB,motor1); 	// Configura Pino como saída ()
  setBit(DDRB,motor2); 	// Configura Pino como saída ()

  clearBit(DDRB,sesquerda); //configura pino como entrada ()
  clearBit(DDRB,sdireita); //configura pino como entrada ()

 
  //Configuração do timer
  TCCR0A=0x83;                //Modo Fast PWM em mode não invertido do OC0A
  TCCR0B=0x03;                // prescaler 64 e com TOP em 0xFF

  OCR0A= Speed;

  /**
   * @brief loop infinito
   * 
   */
  while (1)
  {
 
  left_value = testBit(PINB,sesquerda);
  right_value = testBit(PINB,sdireita);

  if (right_value && left_value)
  {
    //em frente
    setBit(PORTB,motor1);
    setBit(PORTB,motor2);
  }
  else if (right_value  && !left_value)
  {
    //vira direita
    clearBit(PORTB,motor1);
    setBit(PORTB,motor2);
  }
  else //if (!right_value  && left_value)
  {
    //vira esquerda
    setBit(PORTB,motor1);
    clearBit(PORTB,motor2);
  }

  _delay_ms(10);
  }                                                
  return (0);                           
}
