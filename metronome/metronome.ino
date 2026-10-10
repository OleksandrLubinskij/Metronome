int tempo_potentiometr_pin = A0;
int led_c_1 = 12; //Молодший біт контроллера для 7-сегментних led
int led_c_0 = 13; //Старший біт контроллера для 7-сегментних led

//Регістр
int DS = 9; //Пін вводу інформації
int STC = 10; //Перехід з 0 на 1 віддає нові дані
int SHC = 11; //При подачі високого сигалу відбувається запис одного біту інформації

int piezo = 6; //Пін керування п'єзо елементом
int led = 7; //Пін керування led

unsigned long previous_millis_display = 0;
unsigned long previous_millis_metronome = 0;

unsigned int current_digit = 0;

byte digit_patterns[10] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11110110  // 9
};

void setup() {
  pinMode(led_c_1, OUTPUT);
  pinMode(led_c_0, OUTPUT);
  pinMode(DS, OUTPUT);
  pinMode(STC, OUTPUT);
  pinMode(SHC, OUTPUT);
  pinMode(piezo, OUTPUT); 
  pinMode(led, OUTPUT);
  Serial.begin(9600);
} 
void register_write(byte symbol) {
  shiftOut(DS, SHC, LSBFIRST, symbol);

  digitalWrite(STC, HIGH);
  digitalWrite(STC, LOW);
}
void displaySymb(int num) {
  unsigned long current_millis = millis();
  if(current_millis - previous_millis_display >= 1) {
    previous_millis_display = current_millis;
    int digit_to_show = 0;
    switch(current_digit) {
      case 0:
        digit_to_show = num % 10;
        break;
      case 1:
        digit_to_show = (num / 10) % 10;
        break;
      case 2:
        digit_to_show = (num / 100) % 10;
        break;
    }
    byte pattern = digit_patterns[digit_to_show];
    if(current_digit == 2 && num < 100) {
      pattern = 0b00000000;
    }
    register_write(pattern);
    digitalWrite(led_c_0, bitRead(current_digit, 1));
    digitalWrite(led_c_1, bitRead(current_digit, 0));
    current_digit++;
    if(current_digit > 2) {
      current_digit = 0;
    }
  }
}
void loop() {
  unsigned long current_millis = millis();
  int tempo;
  int tempo_potentiometr_state;

  tempo_potentiometr_state = analogRead(tempo_potentiometr_pin);
  tempo = map(tempo_potentiometr_state, 0, 1023, 30, 301);
  Serial.println(tempo);
  displaySymb(tempo);
  unsigned int beat_interval = 60000 / tempo;
  if(current_millis - previous_millis_metronome >= beat_interval) {
    previous_millis_metronome = current_millis;
    tone(piezo, 600, 30);
  }
  if(current_millis - previous_millis_metronome >= 30) {
    digitalWrite(led, LOW);
  } else {
  digitalWrite(led, HIGH);
  }
}
