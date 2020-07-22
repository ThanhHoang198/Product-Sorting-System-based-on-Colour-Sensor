#include <color_sensor.h>
#define LCD_ENABLE_PIN PIN_D6
#define LCD_RS_PIN PIN_D7
#define LCD_RW_PIN PIN_C4
#define LCD_DATA4 PIN_D5
#define LCD_DATA5 PIN_D4
#define LCD_DATA6 PIN_D3
#define LCD_DATA7 PIN_D2

#define UNIDENTIFIED    0
#define COLOR_RED       1
#define COLOR_BLUE      2
#define COLOR_GREEN     3
#define COLOR_COUNT_MAX     4
#define LOW_   1
#define MEDIUM    2
#define HIGH_   3
#define ON_    1
#define OFF_   0
#include<lcd.c>
unsigned char has_product = 0, count_enable = 0, isFromButtonPressed = 0,done=0,cycle,data_received[5],c,has_red,has_blue,has_green;
int color_count = 0, red_count = 0, blue_count = 0, green_count = 0, blind_count = 0;
unsigned int red_product = 0, blue_product = 0, green_product = 0,sum; 
int color_flag,on_off,speed_mode,freq_mode,sp_stop,time_stop,is_running;
static  float  count, red_freq, blue_freq, green_freq;
int16 Ton, ChuKy=10000;
float Map(float x, float in_min, float in_max, float out_min, float out_max)
{
   float tam = (out_max - out_min)/(in_max - in_min);
   tam = (x - in_min) * tam;
   tam =  tam + out_min;
   return tam;
}

void Hien_thi(int16 value)
{
   lcd_putc(value/10000 + 0x30) ; // chuc nghin
   lcd_putc(value%10000/1000 + 0x30) ;  // nghin
   lcd_putc(value%10000%1000/100 + 0x30) ; // tram
   lcd_putc(value%10000%1000%100/10 + 0x30) ; // chuc
   lcd_putc(value%10 + 0x30) ; // don vi
   
}
void TCS_mode(int8 s2, int8 s3)
{
   count=0;
   color_flag++;
   OUTPUT_BIT(S_2,s2);
   OUTPUT_BIT(S_3,s3);
   set_timer1(3036);
   if(color_flag==4)
      {
       color_flag=0;
       setup_timer_1(T1_DISABLED);
       done=1;
      }
}
void get_data()
{
int i=0;
c=fgetc(com1);
if(c=='<')
   { do{ c=fgetc(com1);
         if(c!='-'&& c!='>') data_received[i++]=c;
       }
       while(c!='>');
   }
   
}

void Delay4us(int16 t)
{
   while(t>0) t--;
}

long Convert(float x, float in_min, float in_max, float out_min, float out_max)
{
   float tam = (out_max - out_min)/(in_max - in_min);
   tam = (x - in_min) * tam;
   tam = tam + out_min;
   return tam;
}

void Write(long Goc, signed long t,int servo)
{
   Ton = Convert(Goc,0,180, 247,1250);
   t=t/20;
   for(;t>0;t--)
   {
      // phat xung trong 20ms
    if(servo==1)
    {
      output_bit(PIN_D1,1);
      Delay4us(Ton);
      output_bit(PIN_D1,0);
      Delay4us(ChuKy-Ton);
    }
     if(servo==2)
    {
      output_bit(PIN_D0,1);
      Delay4us(Ton);
      output_bit(PIN_D0,0);
      Delay4us(ChuKy-Ton);
    }
   }
}

void WriteBack()
{ 
   output_bit(PIN_D1,0);
   output_bit(PIN_D0,0);
}

#INT_EXT
void  EXT_isr(void) 
{  
   count=count+1.0;
}

#INT_RB
void Stop()
{
   if(Input(Pin_B5)==0)
      {  
         isFromButtonPressed=0;     
      }
}

#INT_TIMER1
void  TIMER1_isr(void) 
{ 
   switch(color_flag)
      {  
      case 0: 
              TCS_mode(0,0);              
              break;
      case 1: red_freq=count;
              TCS_mode(1,1);            
              break;
      case 2: green_freq=count;
              TCS_mode(0,1);
              break;
      case 3: blue_freq=count;
              TCS_mode(1,0);
              break;
      }
}
#INT_RDA
void data_rec()
{

   memset(&data_received[0],0,sizeof(data_received));
   get_data();
   on_off=atoi(&data_received[0]);
   sp_stop=atoi(&data_received[3]);
   time_stop=atoi(&data_received[4]);
   switch(data_received[1])
   {  
      case 'L' :speed_mode=LOW_;break;
      case 'M' :speed_mode=MEDIUM;break;
      case 'H' :speed_mode=HIGH_;break;
   }
   switch(data_received[2])
   {  
      case 'l' :freq_mode=LOW_;break;
      case 'm' :freq_mode=MEDIUM;break;
      case 'h' :freq_mode=HIGH_;break;
   }

}
unsigned char TCS3200_getcolor()
{ done=0;
  setup_timer_1(T1_internal | T1_div_by_8);
  set_timer1(3036);
  
  while(!done){};
           /*red_freq= Map(red_freq,500,1650,0,255);
           blue_freq= Map(blue_freq,600,1600,0,255);
           green_freq= Map(green_freq,400,1000,0,255);*/
           cycle++;    
           sum=red_product+blue_product+green_product;
           printf("%d|%d|%d|%d\r\n",red_product,blue_product,green_product,sum);
           
            
  if (((red_freq < 150) && (blue_freq < 150)) || ((green_freq < 150) && (blue_freq < 150)) || ((red_freq < 150) && (green_freq < 150)))
        return UNIDENTIFIED;
  else
    {
        if ((red_freq > blue_freq) && (red_freq > green_freq))
            return COLOR_RED;
        else if ((blue_freq > red_freq) && (blue_freq > green_freq))
            return COLOR_BLUE;
        else if ((green_freq > red_freq) && (green_freq > blue_freq))
            return COLOR_GREEN;
    }    
}

void main()

{  port_b_pullups(true);
   lcd_init(); 
   enable_interrupts(INT_EXT);
   enable_interrupts(INT_rb);
   enable_interrupts(INT_RDA);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);
   set_tris_b(0x31);

while (1){
   
      if(Input(Pin_B4)==0)
      {      
         isFromButtonPressed=1;     
      }     
     
      if ((on_off==ON_) && !is_running)        
          { is_running=1;
             count_enable = 1;
             switch(freq_mode)
             {
             case LOW_: output_low(S_0);
                        output_high(S_1);
                        break;
             case MEDIUM: output_low(S_1);
                          output_high(S_0);
                        break;
             case HIGH_: output_high(S_0);
                        output_high(S_1);
                        break; 
             }         
          }
                    
      else if ((on_off==OFF_)&&is_running)
      {     is_running=0;
            count_enable = 0;
            color_count = red_count = blue_count = green_count = blind_count = 0;
            red_product = blue_product = green_product = 0;
            has_product = 0;
            { 
               lcd_putc("\f");
               lcd_gotoxy(5,1);
               printf(lcd_putc,"STOPP !!!");
               delay_ms(500);
            }
      }
      
      if (count_enable)
      {            
            switch(TCS3200_getColor())
            {
            case COLOR_RED :
                red_count++;
                break;
            case COLOR_BLUE :
                blue_count++;
                break;
            case COLOR_GREEN :
                green_count++;
                break;
            case UNIDENTIFIED :
                blind_count++;
                break;
            }
        
            color_count++;
//!                        lcd_putc("\f");
//!                        lcd_gotoxy(7,2);
//!                        lcd_putc(red_count+0x30);       
//!                        lcd_gotoxy(1,2);
//!                        lcd_putc(has_product+0x30);     
            if (color_count >= COLOR_COUNT_MAX)
            {
                color_count = 0;
                
                if (!has_product)
                {            
                    if ((red_count > blue_count) && (red_count > green_count) && (red_count > blind_count))
                     { 
                     
                        red_product++;
                        has_product = 1;
                        has_red=1;
                    }                 
                    else if ((blue_count > red_count) && (blue_count > green_count) && (blue_count > blind_count))
                    {

                        blue_product++;
                        has_product = 1;
                        has_blue=1;
                    }
                    else if ((green_count > red_count) && (green_count > blue_count) && (green_count > blind_count))
                    {

                        green_product++;
                        has_product = 1;
                        has_green=1;
                    }
                    else if ((blind_count > red_count) && (blind_count > green_count) && (blind_count > blue_count))
                    {

                    }
                }
                else
                {
                    if ((blind_count > red_count) && (blind_count > green_count) && (blind_count > blue_count))
                        has_product = 0;
                       
                }
                            
                red_count = blue_count = green_count = blind_count = 0;
               
            }
       
            lcd_putc("\f");
            lcd_putc("RD   BL  GR ");
            lcd_gotoxy(1,2);
            lcd_putc(red_product+0x30);
            lcd_gotoxy(5,2);
            lcd_putc(blue_product+0x30);
            lcd_gotoxy(9,2);
            lcd_putc(green_product+0x30); 
            lcd_gotoxy(4,1);
            lcd_putc(input_state(pin_C2)+0x30);
           if(has_red)
            {
              if(!input_state(pin_E0))
             {//servo1 
               Write(120,1000,1);
               Write(0,1000,1);
               WriteBack();                
               has_red=0;

             }
               
            }
            
            if(has_blue)
             { 
             if(!input_state(pin_C2)){has_blue=0;}
              //Oservo2
              {
               Write(120,1000,2);
               Write(0,1000,2);
               WriteBack();                
               has_blue=0;
              }
             }
             if(has_green)
             {
               has_green=0;
             }
                     
     //LCD
      }
      else
      {     
            lcd_gotoxy(1,1);
            lcd_putc("    MACH DEM SP");
            lcd_gotoxy(1,2);
            lcd_putc("  CAM BIEN MAU");
      }
      
}

}
