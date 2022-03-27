#define RDATA PORTA.F2
#define C1DATA PORTC.F0
#define C2DATA PORTC.F1
#define CLK PORTC.F2
#define LATCH PORTB.F4
#define STB PORTB.F6
int moji1[16] = {0x8000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
int moji2[16] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
char display_brightness = 255;

const short uart_data_len = 65;
char rxchar, uart_count = 0, flag = 0;
unsigned char rxarray[70];

void interrupt () {
 if (PIR1.RCIF) {
  PIR1.RCIF = 0;
  rxchar = UART1_Read();
  rxarray[uart_count] = rxchar;
  uart_count++;
  if (uart_count == uart_data_len) {
   flag = 1;
   uart_count=0;
  }
 }
}

void init(){
 ANSEL = 0x00;
 ANSELH = 0x00;
 C1ON_bit = 0;
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
 PWM1_Init(50000);
 PWM1_Set_Duty(display_brightness);
 PWM1_Start();
 INTCON.GIE = 1;
 INTCON.PEIE = 1;
 PIR1.RCIF = 0;
 PIE1.RCIE = 1;
}

void uart_apply(){
 char temp = 0;
 for(temp=0;temp < 16;temp++){
  moji1[temp] = (int)rxarray[2*temp] * 0x100;
  moji1[temp] += (int)rxarray[2*temp+1];
  moji2[temp] = (int)rxarray[2*temp+32] * 0x100;
  moji2[temp] += (int)rxarray[2*temp+33];
 }
 PWM1_Set_Duty(rxarray[64]);
}

void shiftout3(int data1,int data2,int data3){
 char i;

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
 delay_ms(10);
 uart_count = 0;
 while(1){
  char loop = 16;
  if(flag == 1){
   uart_apply();
   flag = 0;
  }
  while(loop--){
   shiftout3(0x01 << (loop),moji1[loop],moji2[loop]);
   LATCH = 1;
   LATCH = 0;
  }

 }

}