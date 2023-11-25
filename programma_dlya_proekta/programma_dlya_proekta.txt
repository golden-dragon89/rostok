#define exit_sensors_counter 1   //количество датчиков на выходах
#define enter_sensors_counter 2  //количество датчиков на входах

int16_t counter_exit, counter_enter;  //количество вышедших, вошедших
double counter=0;//количество находящихся в здании людей

//пины к которым подключены датчики {trig пин, echo пин}, стоящие 

short enter_sensors[enter_sensors_counter][2] = { { 9, 10 }, { 7, 6 } };  //на входах
short exit_sensors[exit_sensors_counter][2] = { { 11, 12 } };                //на выходах

double turnonTime = 0, turnonTimeP = 0;  //время, на которое необходимо включить рециркулятор в течении часа. Тип double, чтобы не возникало ошибок при работе с дробными числами
double p = 100000;         //сумма производительностей всех рециркуляторов в помещении м^3/час

uint32_t enterTime[enter_sensors_counter];
uint32_t exitTime[exit_sensors_counter];

bool flag_enter[enter_sensors_counter];  //создаём массивы для записи проходят ли люди через датчик или нет
bool flag_exit[exit_sensors_counter];

void setup() {
  pinMode(8, OUTPUT);                                //инициализируем порт, к которому подключено реле для вкл/вкл нагрузки (рециркуляторов)
  for (short i = 0; i < exit_sensors_counter; i++) {  //инициализируем порты для выходных датчиков
    pinMode(exit_sensors[i][1], INPUT);
    pinMode(exit_sensors[i][0], OUTPUT);
  }
  for (short i = 0; i < enter_sensors_counter; i++) {  //инициализируем порты для входных датчиков
    pinMode(enter_sensors[i][1], INPUT);
    pinMode(enter_sensors[i][0], OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  get_data(counter_exit, counter_enter);  //получаем количество вышедших и вошедших людей
  Serial.println(counter);                //выводим количество людей в помещении
  counter += counter_enter;  //обсчитываем количество людей, отнимаем вышедших прибавляем вошедших
  counter -= counter_exit;
  if (counter < 0) {//защита от ошибок (людей не может быть < 0)
    counter = 0;
    Serial.println("EROR");
  }
  counter_enter = 0;  //обнуляем счётчики вошедших и вышедших
  counter_exit = 0;
  if (millis() % 60000 == 0) {//очищаем воздух с началом каждого часа с момента запуска программы
    turnonTime+=counter/p*3600000;//считаем каждую минуту время включения рециркулятора
  }
  if (millis()%3600000 <= 0 && turnonTime != 0) {  //проверяем надо ли включить рециркулятор или выключить
    turnonTimeP=turnonTime;
    turnonTime=0;
  }
  if (millis()%3600000<=turnonTimeP) {
    digitalWrite(8,1);
  } else {
    digitalWrite(8, 0);
  }
  delay(10);  //защита от ошибок и шумов
}


void get_data(int16_t &counter1, int16_t &e_counter) {
  long int duration;  //время возврата сигнала
  // unsigned long int counter=0,e_counter=0;
  for (short i = 0; i < exit_sensors_counter; i++) {  //получаем информацию с датчиков на выходе
    digitalWrite(exit_sensors[i][0], 0);           
    delayMicroseconds(5);
    digitalWrite(exit_sensors[i][0], 1);
    delayMicroseconds(10);
    digitalWrite(exit_sensors[i][0], LOW);
    duration = pulseIn(exit_sensors[i][1], HIGH, 14550);

    if ((duration / 2 / 29.1 < 90) && (duration != 0) && (flag_exit[i] == false)) {  //если в проходе человек записываем это
      flag_exit[i] = true;
      exitTime[i] = millis();
      counter1++;
    }
    if ((millis() - exitTime[i] > 450) && (flag_exit[i] == true) && (duration / 2 / 29.1 > 90 || duration == 0)) {
      flag_exit[i] = false;
    }
  }
  for (short i = 0; i < enter_sensors_counter; i++) {  //аналогично но для входа
    digitalWrite(enter_sensors[i][0], LOW);
    delayMicroseconds(5);
    digitalWrite(enter_sensors[i][0], HIGH);
    delayMicroseconds(10);
    digitalWrite(enter_sensors[i][0], LOW);
    duration = pulseIn(enter_sensors[i][1], HIGH, 14550);
    if ((duration / 2 / 29.1 < 100) && (duration != 0) && (flag_enter[i] == false)) {
      e_counter++;
      enterTime[i] = millis();
      flag_enter[i] = true;
    }
    if ((millis() - enterTime[i] > 450) && (flag_enter[i] == true) && (duration / 2 / 29.1 > 100 || duration == 0)) {
      flag_enter[i] = false;
    }
  }
}
