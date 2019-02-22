/*
AUTOR: Castillo Alvarado, David Rolando 
PROYECTO: Test Básico del SIM800L con teclado alfanumerico y Monitor Serial
USO DEL TECLADO:::::::::::::::::::
    A= Realizar llamada
    B= reset - en llamadas
    C= Colgar Llamada
    D= Contestar Llamada
    #= Marcar para mandar mensajes de texto usando la interfaz del Monitor Serial
    *= Leer mensaje entrante
CONEXIÓN::::::::::::::::::::::::::
    SoftwareSerial RX||TX=Pin 3||Pin 2
    Teclado 4,5,6,7; 8,9,10,11 
    Volt_arduino = 5.00v
    Volt_SIM800  = 4.00v
    LED 13 = Como indicador de recepción de SMS
    LED 14(A0)= Intermitente, trabajo en "paralelo" con el programa del Arduino_SIM800
FUENTES:::::::::::
https://www.minitronica.com/enviando-sms-arduino-sim800l/
https://www.minitronica.com/recibir-sms-arduino-sim800l/
http://www.elcodigoascii.com.ar/
*/

#include <SIM800.h> // Libreria Libre Modificada
#include <Keypad.h> // Libreria para Controlar el teclado
#include <SoftwareSerial.h> // simula puertos serial en pines digitales
SoftwareSerial simComx(3,2);
unsigned long bauds = 19200;                     //Define los baudios para el serial y el SIM
int LED=13;                                       // Led se enciende cuando hay sms y se apaga cuando se lee el sms
char push;
char msg[200]=" ";                               // 200 como tamaño Maximo de Caracteres en un sms
String msgin;
int i=0;
int j=0;
//Actualizar el codigo de pais, +NN
char phonenumber[20]="+51          ";          // Formato para las llamadas telefonica
char phonesms[20]=" +51          ";            // Formato para enviar los sms

// Parametros para el teclado alfanumerico
const byte filas = 4;
const byte columnas = 4;
byte pinsFilas[filas] = {11, 10, 9, 8};
byte pinsColumnas[columnas] = {7, 6, 5, 4}; 
char teclas[filas][columnas] =  {
  {'1', '2', '3', 'A'},                       // Declaración del teclado
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
Keypad teclado = Keypad(makeKeymap(teclas), pinsFilas, pinsColumnas, filas, columnas);

void setup() {
  Serial.begin(bauds);  
  Serial.print("  :::Espere el reconocimiento de la SIM:::\n ");
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); 
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW); 
  SIM.begin(bauds);
  SIM.ini();                              // Pantalla de Presentación_ Libreria modificada por DAvidCA
  delay(100);
  
  SIM.setTimeout(3000);                   // Tiempo de respuesta del servidor
  
  SIM.ringerVolume  (SET, "100");             // Volumen de timbre
  SIM.loudSpeakerVol(SET, "100");             // Volumen del speaker
                                             //Configuramos el módulo para trabajar con los SMS en modo texto
  SIM.smsFormat(SET,"1");
  delay(1000);                               //Pausa de 1 segundo
                                             //Configuramos el módulo para que nos muestre los SMS recibidos por comunicacion serie
  SIM.smsNotifyNew(SET,"1,1,0,1,1");         // Revisar el Manual de Comandos AT, los msm se guardan en la memoria 
  delay(1000);
  SIM.delAllMsg(SET,"DEL ALL");              // Borra todos los sms
  delay(3000);                               // Tiempo de borrado
  Serial.println("...");
  Serial.println("... Listo");
  Serial.println("  :::Ingrese Nuumero de celular:::\n ");
  Serial.println("Use el Teclado Externo");
  msgout();                                 // Bloquea salidas indeseadas de datos al monitor  
}

void loop(){
    push = teclado.getKey();                         // Lee la entrada del teclado 
    if (push != NO_KEY){
    SIM.begin(bauds);                                // Renueva la dirección del Serial con la libreria del SIM
    SIM.setTimeout(3000);                            // Tiempo de espera para respuesta
    delay(50);
    switch (push) {                                  // Se configura la acción de cada tecla
      case 'A':
        Serial.println("\nllamando a:: ");
        phonenumber[3+i] = 59;                       //59 en codigo ASCII es (;) ......punto y coma
        i=0;
        Serial.print(phonenumber);
        SIM.originCall(phonenumber);
        delay(3000);   
        msgout();
        reset();
        break;      
      case 'B':
        msgout();
        reset();
        break;  
      case 'C':
        SIM.endCall();
        Serial.println("\n Colgaste la llamada:::: \n");
        msgout();
        reset();
        break;
      case 'D':
        Serial.println("Contestaste la llamada");
        SIM.answerCall();
        delay(300);
        msgout();
        reset();
        break;
      case '*':
        Serial.println("Ver ultimo Mensaje SMS");
        digitalWrite(LED, 0);
        Serial.println("...");
        SIM.smsRead(SET,"1");
        delay(50);
        SIM.smsDel(SET,"1");
        msgout();
        reset();
        break;
      case '#':
        Serial.println("\n Zona de Mensajes");
        Serial.println("\n Introdusca Numero Telefonico");
        Serial.println("\n Presione 'A' cuando acabe te colocar el numero ");
        i=0;
        while(push != 'A'){
          push = teclado.getKey();
          if((push!= NO_KEY) && ( push!='A')){
          phonesms[4+i] = push;
          i=i+1;
          Serial.print(push);}
          }
          phonesms[0]=34;                         // 34 en el Codigo ASCII es (") ..... comilla
          phonesms[4+i]=34;
        i=0; j=0;
        Serial.println(phonesms);
        Serial.println("\n Introdusca el mensaje");
        Serial.println("\n Introdusca por el Monitor serial");
        Serial.println("\n Presione Enter para mandar el mensaje");
        while(i==0){
            if (Serial.available()>0){            // Si la comunicacion serie normal tiene datos
             delay(100);                          // Tiempo de espera para que lleguen los datos
              while(Serial.available()>0) {       // y mientras tenga datos que mostrar 
                msg[j]=Serial.read();           // Colocamos los datos del monitor al mensaje
              j++;
              } 
              i=1;
            }
          }
          Serial.println(msg);
          SIM.smsFormat(SET, "1");             // Se configura para mandar mensajes
          delay(50);
          SIM.smsSend(phonesms, msg);       // Manda mensaje
          Serial.println("\n ");
        Serial.println("\n Mensaje Enviado");
        delay(50);
        msgout();
        reset();
        break;
      default: //Se presenta el numero en la pantalla y se guarda en un char
          if(1){
          phonenumber[3+i] = push;
          i=i+1;
          Serial.print(push);
          }
      }
    }

msgin_t();

// Led intermitente
  digitalWrite(14, 1);
  delay(50);
  digitalWrite(14, 0);
  delay(50);
}

void reset() {                           // Función para resetear las variables
  Serial.println("");
  int i;
  for(i =0;i<=16;i++){
  phonenumber[3+i]=NO_KEY;
  }
  for(i =0;i<=15;i++){
  phonesms[4+i]=NO_KEY;
  }
  i=0;
  j=0;
  for(int i =0;i<=200;i++){
  msg[i]=NO_KEY;
  }
}

void msgout(){                          //Función para eliminar información indeseada
  SoftwareSerial simComx(3,2);
  simComx.begin(bauds);
  delay(50);
  if(simComx.available()>0){           // ** Metodo UNO de lectura / si hay datos disponibles
  delay(50);
  while(simComx.available()>0) {       // y mientras tenga datos que mostrar 
                msg[j]=simComx.read();           // Colocamos los datos del monitor al mensaje
              j++;
              } 
  j=0;
  }
  for(int i =0;i<=200;i++){
  msg[i]=NO_KEY;
  }
  i=0;
}

void msgin_t(){                                            // Función para Leer la información entrante por el serial
  SoftwareSerial simComx(3,2);
  simComx.begin(bauds);
  delay(50);
  if (simComx.available()){                                //** Metodo DOS de lectura /Si hay datos disponibles
    msgin = simComx.readString();                        //Los almacenamos en la variable msgin
    if (msgin.indexOf("RING")>=0){                        // Identifica el termino RING
      Serial.println("Llamada Entrante: " + msgin);
      SIM.showCallerID(SET,"1");
      delay(100);
      }else if (msgin.indexOf("SM")>=0){                 // Identifica el Termino SM
      Serial.println("NUEVO SMS ENTRANTE \n Presiona * para ver sms: " + msgin); 
      digitalWrite(LED, 1);                                                      //Los sacamos por comunicacion serie   
        }else{ Serial.println("EXIT");
          }
  } 
}

