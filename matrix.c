#define RDATA PORTA.F0
#define C1DATA PORTA.F1
#define C2DATA PORTA.F2
#define CLK PORTA.F3
#define LATCH PORTA.F4
#define STB PORTA.F5
int moji1[16] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
int moji2[16] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

const short n = 64;
char rxchar, uart_count = 0, flag = 0; // Variable for storing the data from UART and array counter
unsigned char rxarray[100];   // array to store the received charaters

void interrupt () {
 if (PIR1.RCIF) {          // test the interrupt for uart rx
  rxchar = UART1_Read();  //
  rxarray[uart_count] = rxchar;
  uart_count++;
  // ******************************************************
  // Only select one of the following statements
  if (uart_count == n) {         // select this if looking for a number of characters
   flag = 1;
   uart_count=0;
  } // end if (rxchar == "$")
 } // end  if (PIR1.RCIF)
} // end interrupt
  
void init(){
 //OSCCON = 0x70;
 //OSCCON = 0b00001000;
 ANSEL = 0x00;
 ANSELH = 0x00;
 C1ON_bit = 0;                       // Disable comparators
 C2ON_bit = 0;
 TRISA = 0x00;
 PORTA = 0x00;
 TRISB = 0x00;
 PORTB = 0x00;
 TRISC = 0x00;
 PORTC = 0x00;
 STB = 0;
 LATCH = 0;
 CLK = 0;
 UART1_Init(500000);
 INTCON.GIE = 1;
 INTCON.PEIE = 1;
 PIE1.RCIE = 1; //enable interrupt.
}

void uart_apply(){
 int temp = 0;
 for(temp=0;temp < 16;temp++){
  moji1[temp] = (int)rxarray[2*temp] * 0x100;
  moji1[temp] += (int)rxarray[2*temp+1];
  moji2[temp] = (int)rxarray[2*temp+32] * 0x100;
  moji2[temp] += (int)rxarray[2*temp+33];
 }
}

void shiftout3(int data1,int data2,int data3){
 int i;

 for(i=0;i<16;i++){
  RDATA = (data1 >> i) & 0x01;
  C1DATA = (data2 >> i) & 0x01;
  C2DATA = (data3 >> i) & 0x01;
  CLK = 1;
  CLK = 0;
 }
}

void main() {
 init();
 while(1){
  int loop = 16;
  if(flag == 1){
   uart_apply();
   flag = 0;
  }
  while(loop--){
   shiftout3(0x01 << (loop),moji1[15-loop],moji2[15-loop]);
   LATCH = 1;
   LATCH = 0;
  }

 }
 
}