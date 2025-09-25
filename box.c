#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define maxsizepassword 4
 
#define matrixout0 PD5   
#define matrixout1 PD6
#define matrixout2 PD7
#define matrixin0 PD1
#define matrixin1 PD2
#define matrixin2 PD3
#define matrixin3 PD4

#define resetpin  PB0
#define servopin  PB1
#define CLOCK_PIN  PB2  
#define LATCH_PIN  PB3  
#define DATA_PIN   PB4  

#define LEDB PC0
#define LEDG PC1
#define LEDR PC2

void write_password(int* input_password,int* indx,int val){
    if(* indx==maxsizepassword)
        return;
    input_password[(*indx)++]=val;
}
void reset_input(int* indx){
    *indx=0;
}

int is_password_true(int* password,int* input_password){
    for(int i=0;i<maxsizepassword;i++){
        if(password[i]!=input_password[i]){
            return 0;
        }
    }
    return 1;
}

void set_password(int* password,int* setindx, int val){
    if(* setindx==maxsizepassword)
        return;
    password[(* setindx)++]=val;
}


void shiftOut_LSBFIRST(uint8_t val){
    for (uint8_t i=0;i<8;i++){

        if(val&0x01){
            PORTB|=(1<<DATA_PIN);   
        } else {
            PORTB&=~(1<<DATA_PIN);  
        }

        PORTB|=(1<<CLOCK_PIN);
        PORTB&=~(1<<CLOCK_PIN);
        val>>=1;                     
    }
}

uint8_t segCode[11] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0x00};
void display(int num){
    if(num>=0&&num<11){
        PORTB&=~(1<<LATCH_PIN);                   
        shiftOut_LSBFIRST(segCode[num]);         
        PORTB|=(1<<LATCH_PIN);                    
    }
}


void servo_init() {
    ICR1=39999;                              
    TCCR1A =(1<<COM1A1)|(1<<WGM11); 
    TCCR1B =(1<<WGM13)|(1<<WGM12)|(1<<CS11); 
    DDRB |=(1<<servopin);                    
}

// 設定角度 0~180
void servo_write(uint8_t angle) {
    uint16_t pulse=1000+((angle*1000UL)/180); 
    OCR1A=pulse*2;                         
}

void My_RGB_display(char c){
    PORTC&=~(1<<PC0);
    PORTC&=~(1<<PC1);
    PORTC&=~(1<<PC2);
    switch (c){
        case 'R':
            PORTC|=(1<<PC2);
            break;
        case 'G':
            PORTC|=(1<<PC1);
            break;
        case 'B':
            PORTC|=(1<<PC0);
            break;
        case ' ':
            PORTC&=~(1<<PC0);
            PORTC&=~(1<<PC1);
            PORTC&=~(1<<PC2);
            break;
        default:
            break;
    }
}



int main(){
    int password[4]={0,0,0,0};
    int input_password[4];
    int indx=0,setindx=0;
    int setmode=0;
    int keymap[4][3]={{1,2,3},{4,5,6},{7,8,9},{10,0,11}};

    
    DDRD|=(1<<matrixout0);
    DDRD|=(1<<matrixout1);
    DDRD|=(1<<matrixout2);


    DDRD &= ~(1<<matrixin0);
    DDRD &= ~(1<<matrixin1);
    DDRD &= ~(1<<matrixin2);
    DDRD &= ~(1<<matrixin3);

    PORTD|=(1<<matrixout0)|(1<<matrixout1)|(1<<matrixout2);
    PORTD|=(1<<matrixin0)|(1<<matrixin1)|(1<<matrixin2)|(1<<matrixin3);
    
    DDRC|=(1<<PC0);
    DDRC|=(1<<PC1);
    DDRC|=(1<<PC2);

    PORTC&=~(1<<PC0);
    PORTC&=~(1<<PC1);
    PORTC&=~(1<<PC2);

    DDRB &= ~(1<<resetpin);
    PORTB|=(1<<resetpin);

    DDRB|=(1<<DATA_PIN)|(1<<CLOCK_PIN)|(1<<LATCH_PIN); 
    servo_init();

    while(1){

        if(!(PINB&(1<<resetpin))){
            setmode=1;
            My_RGB_display('B');
            reset_input(&setindx);
            _delay_ms(200);
        }


        for(int i=0;i<4;i++){
            for(int j=0;j<3;j++){
                int matrixoutpin=matrixout0+j;
                int matrixinpin=matrixin0+i;
                PORTD&=~(1<<matrixoutpin);

                if(!(PIND&(1<<matrixinpin))){
                    int val=keymap[i][j];
                    if(setmode){
                        if(val==10){
                            if(setindx==4){
                                setmode=0;
                                My_RGB_display(' ');
                                servo_write(180);
                                display(10);
                            }
                            reset_input(&setindx);
                        }
                        else if(val==11){
                            reset_input(&setindx);
                        }
                        else{
                            if(setindx<maxsizepassword){
                                set_password(password,&setindx,val);
                                display(val);
                            }

                        }
                    }
                    else{
                        if(val==10){
                            if(indx==4){
                                if(is_password_true(password,input_password)){
                                    My_RGB_display('G');
                                    servo_write(0);
                                }
                                else
                                    My_RGB_display('R');
                            }
                            reset_input(&indx);
                        }
                        else if(val==11){
                            reset_input(&indx);
                        }
                        else{
                            if(indx<maxsizepassword){
                                write_password(input_password,&indx,val);
                                display(val);
                            }

                        }    
                    }
                    _delay_ms(200);
                }
                PORTD|=(1<<matrixoutpin);
            }
        }

    }

    return 0;
}