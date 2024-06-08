#include <REG51.H>
#include <STDIO.H>
#include <intrins.h>

/* Define value to be loaded in timer for PWM period of 20 milli second */
#define PWM_Period 0xB7FE

sbit PWM_Out_Pin 			= P2^0;	/* PWM Out Pin for speed control */
sbit Speed_Inc 				= P1^0;	/* Speed Increment switch pin */
sbit Speed_Dec 				= P1^1;	/* Speed Decrement switch pin */
sbit Change_Dir				= P1^2;	/* Rotation direction change switch pin */
sbit CKT_ON           = P1^3;
sbit M1_Pin1					= P1^6;	/* Motor Pin 1 */
sbit M1_Pin2					= P1^7;	/* Motor Pin 2 */

sfr LCD_DATA = 0xB0 ;
sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;


void delay(unsigned int count)
{
    int i,j;
    for(i=0; i<count; i++)
			for(j=0; j<112; j++);
}

void lcd_cmd(unsigned char command)
{
	LCD_DATA = command;
	RS = 0;
	RW = 0;
	E = 1;
	delay(10);
	E = 0;
}

void lcd_data(unsigned char disp_data)
{
	LCD_DATA = disp_data;
	RS = 1;
	RW = 0 ;
	E = 1;
	delay(10);
	E = 0;
	
}

void lcd_init()
{
	
	lcd_cmd(0x38);
	delay(10);
	lcd_cmd(0x0c);
	delay(10);
	lcd_cmd(0x80);
	delay(10);
}

unsigned int ON_Period, OFF_Period, DutyCycle;
float Speed;

int rotation(int dir)
{
	if(dir==1)
		return 0;
	else
		return 1;
}
	

/* Function to provide delay of 1ms at 11.0592 MHz */


void Timer_init()
{
	TMOD = 0x01;						/* Timer0 mode1 */
	TH0 = (PWM_Period >> 8);/* 20ms timer value */
	TL0 = PWM_Period;      
	TR0 = 1;								/* Start timer0 */
}

/* Timer0 interrupt service routine (ISR) */
void Timer0_ISR() interrupt 1	
{
	PWM_Out_Pin = !PWM_Out_Pin;
	if(PWM_Out_Pin)
	{
		TH0 = (ON_Period >> 8);
		TL0 = ON_Period;
	}	
	else
	{
		TH0 = (OFF_Period >> 8);
		TL0 = OFF_Period;
	}	
			
}
void display(int tik)
{
	unsigned char a[20];
  unsigned int i = 0; 
	int key = rotation(tik);
	if(key == 0)
		{
			lcd_cmd(0xc0);
			lcd_data('a');
		}
		if(key==1)
		{
			lcd_cmd(0xc0);
			lcd_data('c');
		}
	sprintf(a,"DutyCycle=%0.1f",Speed);
				lcd_init();
					while( a[i] != '\0')
		{	
			lcd_data(a[i]);
			i++;
			delay(3);
		}
		
		
}
void Set_DutyCycle_To(float duty_cycle,int pin)
{
	float period = 65535 - PWM_Period;
	ON_Period = ((period/100.0) * duty_cycle);
	OFF_Period = (period - ON_Period);	
	ON_Period = 65535 - ON_Period;	
	OFF_Period = 65535 - OFF_Period;
	display(pin);
}

void Motor_Init()
{
	Speed = 0;
	M1_Pin1 = 1;
	M1_Pin2 = 0;
	Set_DutyCycle_To(Speed,M1_Pin1);
}

void main()
{
	int count=1;
unsigned char a[20];
unsigned int i = 0; 
Speed_Inc =0;			
Speed_Dec =0;			
Change_Dir =0;
CKT_ON = 0;
	
	EA  = 1;					/* Enable global _interrupt */
	ET0 = 1;   				/* Enable timer0 _interrupt */
	Timer_init();
	Motor_Init();
	
	while(CKT_ON==1)
		{
			while(count==1){
				sprintf(a,"PWM Generation");
				lcd_init();
				lcd_cmd(0x01);
					while( a[i] != '\0')
		{	
			
			lcd_data(a[i]);
			i++;
			delay(30);
		}
		count--;
	}
				
			/* Increment Duty cycle i.e. speed by 10% for Speed_Inc Switch */
			if(Speed_Inc == 1)
				{
					if(Speed < 100)
					Speed += 10;
					Set_DutyCycle_To(Speed,M1_Pin1);
					while(Speed_Inc == 1);
					delay(200);
				}
			/* Decrement Duty cycle i.e. speed by 10% for Speed_Dec Switch */
			if(Speed_Dec == 1)
				{
					if(Speed > 0)
					Speed -= 10;
					Set_DutyCycle_To(Speed,M1_Pin1);
					while(Speed_Dec == 1);
					delay(200);
				}
			/* Change rotation direction for Change_Dir Switch */
			if(Change_Dir == 1)
				{
					M1_Pin1 = !M1_Pin1;
					M1_Pin2 = !M1_Pin2;
					while(Change_Dir == 1);
					display(M1_Pin1);
					delay(200);
				}	
		}
			while(CKT_ON == 0){
	
	sprintf(a,"!!!");
				lcd_init();
				lcd_cmd(0x01);
				
					while( a[i] != '\0')
		{	
			
			lcd_data(a[i]);
			i++;
			delay(30);
		}
		count++;
		M1_Pin1 =0;
		
	}
}