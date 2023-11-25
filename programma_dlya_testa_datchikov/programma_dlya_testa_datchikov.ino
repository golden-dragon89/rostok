
#define pin_trig_mid 7 //указываем порты, к которым подключены датчики
#define pin_echo_mid 6

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_trig_mid, OUTPUT);//настраиваем порты
  pinMode(pin_echo_mid, INPUT);
}

void loop() {
  Serial.println(get_lenght_mid());//выодим растояние, полученое с датчика
  delay(10);
}

int get_lenght_mid() {
  long int duration;
  digitalWrite(pin_trig_mid, LOW);//защита
  delayMicroseconds(5);
  digitalWrite(pin_trig_mid, HIGH);//подаём сигнал
  delayMicroseconds(10);
  digitalWrite(pin_trig_mid, LOW);//заканчиваем подачу сигнала
  duration = pulseIn(pin_echo_mid, HIGH,12250);//записываем время за которое сигнал вернулся
  return (duration / 2) / 29.1;//считаем растояние и возвращаем его
}