 //Timing Calculations:
  /*
  Horizontal timing:
  Visible area - 320 counts
  Front porch - 16 counts
  Sync pulse-51.2(51)
  Back porch - 35 counts
  Whole line 422 counts
  Vertical Timing(lines):
  Visible area- 600 lines
  Front porch 1 lines
  Sync pulse 4 lines
  Back porch 23 lines
  Whole frame 628 lines
  Info From Datasheet:
  Timer1A CLK i/p on P3.7
*/
#include "msp430.h"
//ISRs:
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_TAIFG(void)
{
 P1OUT|= BIT2;
 P1OUT&=~BIT3;
}
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer1(void)
{
  switch(TA1IV){
case TA1IV_TAIFG:
    P1OUT|=BIT3;
    break;
case TA1IV_TACCR1:
   P1OUT&=~BIT2;

 break;
 case TA1IV_TACCR2:
   P1OUT|=BIT2;
break;
 }
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TimerA1ISR(void)
{
    P1OUT|=BIT4;
}
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TimerBISR2(void)
{
    switch(TAIV){
    case TA0IV_TACCR1:
        P1OUT&=~BIT4;
        break;
    case TA0IV_TACCR2:
        P1OUT|=BIT4;
        break;
    }
}
int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

  //Caliberate DCO for 16MHz
  DCOCTL=CALDCO_16MHZ;
  BCSCTL1=CALBC1_16MHZ;
  //Setting up Timer A
  TACTL = TACLR + TASSEL_0 + MC_1 + ID_0+TAIE;
  //Setting up port P1.2 for HSync
  P1DIR|=BIT2;
  P1OUT|=BIT2;
  //Setting up the port P1.4 VSync
  P1DIR|=BIT4;
  P1OUT|=BIT4;
  //Setting p1.3 for clock for timer
  P1DIR|=BIT3;
  P1OUT&=~BIT3;
 // Setting p1.0 for TACLK
  P1DIR&=~BIT0;
  P1SEL|=BIT0;
  P1SEL2&=~BIT0;
  //Setting up compare units for each calculated counts
  CCR0 = 628;
  CCTL0 = CCIE;
  CCR1 = 601;
  CCTL1 = CCIE;
  CCR2 = 605;
  CCTL2=CCIE;
  //Configuring timer 2
  TA1CTL = TASSEL_2+MC_1+TACLR+ID_0+TAIE;
  TA1CCR0=422;
  TA1CCTL0=CCIE;
  TA1CCR1=336;
  TA1CCTL1=CCIE;
  TA1CCR2=387;
  TA1CCTL2=CCIE;
  P2DIR|=(BIT3+BIT4);
  __bis_SR_register(GIE);
  while(1){
      while(TA1R<=320&&TA0R<=600)
      {
          P2OUT|=BIT3;
          P2OUT|=BIT4;
      }
      P2OUT&=~BIT3;
      P2OUT&=~BIT4;
 }
 return 0;
}

