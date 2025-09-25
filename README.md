
1.編譯指令
    tmega328p -DF_CPU=16000000UL -Os -o box.elf box.c
    avr-objcopy -O ihex -R .eeprom box.elf box.hex

2.AVRC 語法，採用4X3矩陣輸入和而外1個重製密碼輸入鍵組成的密碼置物箱

3.shiftOut_LSBFIRST():
    //8個bit一個一個丟進74HC595N
    // 若當下的bit=1 設定DATA_PIN=1 否則 設定DATA_PIN=0
    //CLOCK_PIN從1到0時賣變化代表將前一個bit推入74HC595N
    //LATCH_PIN=0 代表先不輸出 LATCH_PIN=1 將8個bit同步輸出

4.servo_init():
    使用 Timer1, Fast PWM, 20ms 週期
    頻率=F_CPU / (預分頻 * TOP+1)
    detail:
    // Fast PWM, Prescale=8 (預分頻8)
    //F_CPU=16MHz, 預分頻=8 => 2MHz =>0.5 miusec (1 tick/0.5 msec)
    // TOP: 20ms 設定週期長度ICR1+1=40000 tick => 40000*0.5=20 msec (50Hz)
    // PB1 = OC1A 設為輸出 (COM1A1=1)
    
5.servo_write():
    // 0°=1ms,180°=2ms
    // 1ms=1000us,2ms=2000us
    //pulse 單位us
    //OCR1A 單位tick (輸入第幾個tick變低電位，總長度40000 tick)
