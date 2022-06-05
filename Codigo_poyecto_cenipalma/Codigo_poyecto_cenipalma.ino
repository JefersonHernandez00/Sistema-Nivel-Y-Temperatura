
/**********************************************
   A4 = SDA y A5= SCL pines para protocolo I2C
   que usa el mod reloj o cualquier otro mod tambien
   los 2 ultimos al aldo de los pines de com por USB

   10 = CS   Pin para ctivar esclavo se puede usar
   cualquiera pero cuando hay un solo esclavo se usa ese
   11 = MOSI maestro sale esclavo entra
   12 = MISO maestro entra esclavo sale
   13 = SCK pulso de reloj serial

   ya estan definidos !
 *********************************************/

#include <LiquidCrystal.h>

#include <Ultrasonic.h>

#include <RTClib.h>
#include <Wire.h>

#include <DHT.h>
#include <DHT_U.h>

#include <SPI.h>
#include <SD.h>
Sd2Card card;

#define sensor 14 // pin del sensor DHT
#define SSpin 10
#define trig 9
#define echo 8
#define MAX_DISTANCE 400

int TIME_OUT = 30000;
long duracion, distancia;
int temperatura;
int humedad;
bool escritura = true;

RTC_DS3231 rtc;  //  creamos variable global del reloj
File archivo;
DHT dht (sensor, DHT11); // definimos el din del sensor y el tipo de senssor
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // definimos los pines del lcd (RS,E,D4,D5,D6,D7)
Ultrasonic ultrasonic(trig, echo, TIME_OUT);
DateTime fecha;

//////////////////////////////////////////////////////////barra
byte b1[8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte b2[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};
byte b3[8] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};
byte b4[8] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte b5[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

/////////////////////////////////////////////////////////////

byte ciculoarriba[8] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte nivel[8] = {
  B10001,
  B10001,
  B10001,
  B11011,
  B11111,
  B11111,
  B11111,
  B11111
};
byte Btemperatura[8] = {
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B10001,
  B10001,
  B01110
};
byte Bhumedad[8] = {
  B00000,
  B00100,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00000
};
byte reloj[8] = {
  B00000,
  B01110,
  B10101,
  B10101,
  B10111,
  B10001,
  B01110,
  B00000
};

byte nada[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte guardar[8] = {
  B00000,
  B11100,
  B10010,
  B10001,
  B11111,
  B10101,
  B11111,
  B00000
};
byte fuerarango[8] = {
  B10100,
  B11111,
  B11111,
  B10100,
  B10010,
  B11111,
  B11111,
  B10010
};

////////////no sd
byte nosd1[8] = {
  B01111,
  B01100,
  B01010,
  B01001,
  B01001,
  B01010,
  B01100,
  B01111
};
byte nosd2[8] = {
  B10000,
  B01000,
  B01100,
  B10010,
  B10010,
  B01010,
  B00110,
  B11110
};
////////////////////////

//////////////////////////marco
byte marco[8] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000
};

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(14, INPUT);

  Serial.begin(9600);
  lcd.begin(20, 4);

  ////////////////////////////
  lcd.createChar(1, b1);
  lcd.createChar(2, b2);
  lcd.createChar(3, b3);
  lcd.createChar(4, b4);
  lcd.createChar(5, b5);
  ///////////////////////////
  Serial.println(F(" "));
  Serial.println(F("        Firux S.A.S todos los derechos reservados 2019"));
  Serial.println(F(" "));

  /********* inicializacion del sensor de nivel **********/
  Serial.println(F("Inicializando Modulo Sensor de nivel..."));
  distancia = ultrasonic.read(CM);
  if (distancia) {
    Serial.println(F("Modulo sensor Inicializado"));
    if ((distancia == 535) || (distancia == 536 )) {
      Serial.println(F("|||||||Fuera de Rango|||||||"));
      Serial.println(F("CONECTAR SENSOR O VERIFICAR RANGO!!!"));
    }
    Serial.println(F(" "));
  } else {
    Serial.println(F("Modulo sensor no encontrado"));
    Serial.println(F(" "));
  }

  /********* inicializacion del mod temperatura **********/
  Serial.println(F("Inicializando Modulo Temperatura..."));
  dht.begin();
  if (dht.readTemperature(true)) {
    Serial.println(F("Modulo Temp Inicializado"));
    Serial.println(F(" "));
  } else {
    Serial.println(F("Modulo Temp no encontrado"));
    Serial.println(F(" "));
  }

  /********* inicializacion del mod reloj **********/
  Serial.println(F("Inicializando Modulo RTC..."));
  if (!rtc.begin()) {
    Serial.println(F("Modulo RTC no encontrado"));
    Serial.println(F(" "));
    while (1);
  } else {
    Serial.println(F("Modulo RTC Inicializado"));
    Serial.println(F(" "));
  }
  //  rtc.adjust(DateTime(__DATE__, __TIME__)); // obtenemos fecha y hora del pc solo 1 sola vez
  // rtc.adjust (DateTime (2019,9,30,16,13,0)); //año, mes, fecha, hora, min, segundo.

  /********* inicializacion del mod SD ***********/
  Serial.println(F("Inicializando Tarjeta SD..."));
  if (!SD.begin(SSpin)) {
    Serial.println(F("Fallo en Inicialización!!"));
    Serial.println(F("CONECTAR MICRO SD!!!"));
    Serial.println(F(" "));
  } else {
    Serial.println(F("Inicialización Correcta"));
    Serial.println(F(" "));
  }

  inicio();
}
void inicio() {
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print(F("LEYENDO"));
  lcd.setCursor(3, 1);
  lcd.print(F("--> MODULOS <--"));

  //  ////////////////// barra de progreso
  for (int f = 0; f < 20; f++) {
    for (int c = 1; c <= 5; c++) {
      lcd.setCursor(f, 3);
      lcd.write(byte(c));
      delay(60);
    }
  }
  lcd.clear();
  lcd.createChar(13, marco);
  lcd.setCursor(5, 1);
  lcd.print(F("CENIPALMA"));
  ///////////////////////marco
  lcd.setCursor(0, 0);
  lcd.write(byte(13));
  for (int m = 1; m < 19; m++)lcd.write(byte(13));
  lcd.write(byte(13));
  delay(20);
  lcd.setCursor(19, 2);
  lcd.write(byte(13));
  delay(20);
  lcd.setCursor(1, 2);
  for (int m = 1; m < 19; m++)lcd.write(byte(13));
  delay(20);
  lcd.setCursor(0, 2);
  lcd.write(byte(13));
  delay(20);
  //////////////////////////////
  lcd.setCursor(7, 3);
  lcd.print(F("FIRUX"));
  delay(3000);
  crearchart();
}

void crearchart() {
  lcd.createChar(6, ciculoarriba);
  lcd.createChar(7, nivel);
  lcd.createChar(8, Btemperatura);
  lcd.createChar(9, Bhumedad);
  lcd.createChar(10, reloj);
  lcd.createChar(11, guardar);
  lcd.createChar(12, fuerarango);
}

void DatosMonitorSerial() {
  Serial.print(F("Nivel "));
  if ((distancia == 535) || (distancia == 536 )) {
    Serial.print(F("0"));
  } else Serial.print(distancia);
  Serial.print(F(" cm / "));
  Serial.print(F("Temperatura "));
  Serial.print(temperatura);
  Serial.print(F(" °C / "));
  Serial.print(F("Humedad "));
  Serial.print(humedad);
  Serial.println(F(" % "));
  Serial.println(F(" "));
}

void loop() {
  fecha = rtc.now();
  // Leemos y registramos la distancia en centímetros que está detectando el sensor ultrasónico impermeable JSN-SR04T.
  distancia = ultrasonic.read(CM);
  temperatura = dht.readTemperature();
  temperatura = temperatura - 6; // ajuste de patron
  humedad = dht.readHumidity();

  if (distancia == 0) distancia = ultrasonic.read(CM);  // eliminamos el valor cero de los resultados
  DatosMonitorSerial();
  lcd.clear(); // lo uso para actualizar los carateres

  ///////////// NO SD //////////// verificamos si esta la micro SD insertada
  if (!card.init(SPI_HALF_SPEED, SSpin)) {
    lcd.setCursor(18, 0);
    lcd.print(F("E0"));
  } else {
    lcd.setCursor(18, 0);
    lcd.print(" ");
  }
  ////////////////////////////////
  lcd.setCursor(0, 0);
  lcd.write(byte(7));
  lcd.print(F(" Nivel F: "));
  if ((distancia == 535) || (distancia == 536 )) {
    lcd.write(byte(12));
  } else {
    lcd.print(distancia);
    lcd.print(F(" cm"));
  }
  ////////////////////////////////
  lcd.setCursor(0, 1);
  lcd.write(byte(8));
  lcd.print(F(" Temperatura: "));
  lcd.print(temperatura);
  lcd.print(F(" "));
  lcd.write(byte(6));
  lcd.print(F("C"));
  /////////////////////////////////
  lcd.setCursor(0, 2);
  lcd.write(9);
  lcd.print(F(" Humedad: "));
  lcd.print(humedad);
  lcd.print(F(" %"));
  ////////////////////////////////

  // mostramos la hora
  lcd.setCursor(0, 3);
  String horalcd = "Hora: " + String(fecha.hour()) + ":" + String(fecha.minute()) + ":" + String(fecha.second());
  lcd.write(byte(10));
  lcd.print(F("     "));
  lcd.print(horalcd);
  delay(1000);

  // activamos el almacanamiento de datos en la horas deseadas en formato militar
  datacenter( 0, 0,  0); //dato 1
  datacenter( 2,  0, 0); //dato 2
  datacenter( 4,  0, 0); //dato 3
  datacenter( 6,  0, 0); //dato 4
  datacenter( 8,  0, 0); //dato 5
  datacenter(10,  0, 0); //dato 6
  datacenter(12,  0, 0); //dato 7
  datacenter(14,  0, 0); //dato 8
  datacenter(16,  0, 0); //dato 9 
  datacenter(18,  0, 0); //dato 10
  datacenter(20,  0, 0); //dato 11
  datacenter(22,  0, 0); //dato 12
}

void datacenter(int hora, int minutos, int segundos) {
  // inica el ciclo de guradar los datos a esta hora especifica
  if ((fecha.hour() == hora) && (fecha.minute() == minutos) && (fecha.second() == segundos) && (escritura == true)) {

    archivo = SD.open("datos.txt", FILE_WRITE); // abre el archivo si no existe lo crea
    if (archivo) {
      Serial.println(F(" "));
      Serial.println(F("entro en la SD..."));
      Serial.println(F("Escribiendo en la SD..."));
      Serial.println(F(" "));

      archivo.println(F(" "));
      String fechaG = "Fecha de la grabación: " + String(fecha.day()) + "/" + String(fecha.month()) + "/" + String(fecha.year());
      archivo.println(fechaG);
      String horaG = "Hora de la grabación: " + String(fecha.hour()) + ":" + String(fecha.minute()) + ":" + String(fecha.second());
      archivo.println(horaG);
      archivo.println(F(""));

      // CSV formato separado por comas para pasar a cuadro de calculo
      archivo.print(fecha.day());
      archivo.print(F("/"));
      archivo.print(fecha.month());
      archivo.print(F("/"));
      archivo.print(fecha.year());
      archivo.print(F(","));
      archivo.print(fecha.hour());
      archivo.print(F(":"));
      archivo.print(fecha.minute());
      archivo.print(F(":"));
      archivo.print(fecha.second());
      archivo.print(F(","));
      if ((distancia == 535) || (distancia == 536 )) {
        archivo.print(F("0"));
      } else archivo.print(distancia);
      archivo.print(F(","));
      archivo.print(temperatura);
      archivo.print(F(","));
      archivo.println(humedad);
      archivo.println(F(""));
      archivo.println(F("------------------------------------"));
      archivo.println(F(" "));
      archivo.close();
      escritura =  false;
      Serial.println(F(" "));
      Serial.println(F("Escritura Corerecta, verificar en SD"));
      Serial.println(F(" "));
      // agregamos el icono guardar cada vez que guarda datos
      lcd.setCursor(19, 0);
      lcd.write(byte(11));
      delay(5000);
      lcd.print(" ");
    } else {
      Serial.println(F(" "));
      Serial.println(F("Error al abrir archivo."));
      Serial.println(F(" "));
    }
  }
  // se seleciona la hora y minuto para el evento
  if ((fecha.hour() == hora) && (fecha.minute() == minutos + 1) && (fecha.second() == segundos) && (escritura == false)) {
    // es para volver activar la escrictura 1 minuto despues
    escritura = true;
    Serial.println(F(" "));
    Serial.println(F("Escritura Activada"));
    Serial.println(F(" "));
    delay(700);
  }
  if ((fecha.hour() == hora) && (fecha.minute() == minutos + 2) && (fecha.second() == segundos) && (escritura == false)) {
    // es para volver activar la escrictura 2 minuto despues
    escritura = true;
    Serial.println(F(" "));
    Serial.println(F("Escritura Activada"));
    Serial.println(F(" "));
    delay(700);
  }
}
