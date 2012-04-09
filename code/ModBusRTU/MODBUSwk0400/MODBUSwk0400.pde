/* 		
 Modbus over serial line - RTU Slave Arduino para módulo WK0400 
 Basado en los trabajos de:
 Juan Pablo Zometa : jpmzometa@gmail.com  http://sites.google.com/site/jpmzometa/
 Samuel Marco: sammarcoarmengol@gmail.com  and Andras Tucsni.
 P.Costigan email: phil@pcscada.com.au http://pcscada.com.au
 paul@pmcrae.freeserve.co.uk (http://www.pmcrae.freeserve.co.uk)
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 autor: pvkinn 
 fecha: abril 2012
 *************************************************************************/
#include <EEPROM.h>  //para las operaciones de lectura y escritura en la EEPROM
//al final tengo otro #include para las librerias ModBus Slave
#include "G:\PVKINN\MODBUSwk0400\HRWK0400.h"

#define DirDefectoMB 18 //uso define en lugar de const para ahorrar espacio en cualquier sitio
unsigned char DirMB =1;// EEPROM.read (EP_DirMB);//DE MOMENTO LA DIRECCIÓN VALE 1

int regs[MB_REGS];     //*los holding registers son de 16 bits

unsigned long wdog = 0;         //* watchdog */
unsigned long tprev ;           //* previous time*/
boolean primerCiclo;       //
int update_mb_slave(unsigned char slave, int *regs,unsigned int regs_size) ;//prototipo de función, es porque el #include lo tengo abajo
void configure_mb_slave(long baud, char parity, char txenpin);//prototipo de función
byte memoria=HIGH;//quitar
void setup() //------------------------------------------------------------------------OK
{
        int pin;
        configure_mb_slave(COMM_BPS, PARITY, 0);//*cuando ponga varios módulos poner un 3 EN LUGAR DE 0
        primerCiclo=true;  //para determinar cuando se reinicia, se usa en la función TiempoCiclo()
        for( pin=DIN1; pin <= AINAUX; pin++) pinMode(pin, pinIO[pin]); //inicializa modo de cada pin definido en HRWK0400.h
        if (DirMB >246 || DirMB <1 ){//LO DEL 246 HAY QUE VERIFICARLO
            DirMB=DirDefectoMB; //asigno valor si no era correcto
            //EEPROM.write (EP_DirMB,DirMB); //DE MOMENTO NO ESCRIBIR EN LA EEPROM
            }
        regs[HR_CMDWK]=0xFFFF;
        regs[HR_VERSION]=0412;//abril 2012
        regs[HR_N_DIR_MB]=DirMB; //guardo en holding register la direccion de MB
        regs[HR_tcicloMax]=0; //para que realice correctamente la detrminación del ciclo máximo
        regs[HR_TMP_WD]=666; //lo he puesto a voleo, nº de la bestia del perro guardian
        InicializaElementos(); //esta función habrá que modificarla para cada configuración
        
             
}

void InicializaElementos()//----------------------------------------------------OK
{
  //estos datos debieran estar en la EEPROM y se grabarían
  //cuando se den de alta a los elementos con el comando correspondiente
  //Esta función simplemente copiaría los datos de la EEPROM en la zona de los HR
  //de los elementos
  
  //relación de tipos de elementos
  enum{ALUMBRADO,ENCHUFE, //esto se podría incluir en HRW0400.h 
       S_GAS=8, S_PRESENCIA,INTERRUPTOR,//la tontería del 8 es por poner unos de entrada y otros de salida y que estos sean del 8 al 15
       TIPO=16
      };  
  //relación de escenas
  enum{ ESCENA0=1,ESCENA1=2,  //también se puede incluir en  HRW0400.h 
        ESCENA2=4, INCENDIO=8, NOCHE=16 //esto es por ser creativo en los nombres de las escenas    
        //LOS VALORES ASIGNADOS SON PARA FACILITAR EL CODIGO POSTERIOR
      };
      
  //Ahora para poder simularlo escribo directamente en los HR unos cuantos elementos inventados       
        regs[HR_RELE1_ID]=1001; //identificador de elemento
        regs[HR_RELE1_ID+1]=ALUMBRADO*TIPO + RELE1; //contiene el tipo + la salida, es un entero pero sólo uso 1 byte, se tendrían que
                                                   //modificar las especificaciones y tener hasta 256 tipos de elementos y 256 I/0 de
                                                   //hecho en el WK0400 el RELE4 es la salida 16 y AINAUX la 17, no vale con 4 bits
                                                   //aunque creo que la información de la salida es redundante, sirve para facilitar 
                                                   //las consultas al maestro WK0500
        regs[HR_RELE1_ID+2]= ESCENA0+INCENDIO+NOCHE;          //escenas en las que participa RELÉ1 
        regs[HR_RELE1_ID+3]= ESCENA0*HIGH + INCENDIO*HIGH + NOCHE*LOW;    //Valores que toma en esas escenas

        regs[HR_RELE2_ID]=1025; 
        regs[HR_RELE2_ID+1]=ENCHUFE*TIPO + RELE2; 
        regs[HR_RELE2_ID+2]=ESCENA0+INCENDIO;
        regs[HR_RELE2_ID+3]=ESCENA0*HIGH + INCENDIO*LOW  ;   
  
        regs[HR_DIN6_ID]=2012; 
        regs[HR_DIN6_ID+1]=INTERRUPTOR*TIPO + DIN6; 
        regs[HR_DIN6_ID+2]=ESCENA0+NOCHE; 
        regs[HR_DIN6_ID+3]=ESCENA0*HIGH + NOCHE*HIGH;  
       regs[HR_RELE2_ID+3]=memoria;//QUITAR 
}


void loop() //--------------------------------------------------------------------------OK
{
regs[HR_CMDWK]=0xFFFF;
TiempoCiclo();   //Determinamos tiempos y lee el ModBus
Actualizar_Entradas();       //actualiza entradas y las guarda en HR y operará con los valores leídos   
//lo de lógica negativa poner aquí, para el tema de las ENTRADAS
if(regs[HR_CMDWK] == 0xFFFF) ProcesarNormal(); //NO ha llegado ningún comando del maestro
     else ProcesarComando();//Ha llegado un comando desde el WK0500 
//lo de lógica negativa poner aquí, para el tema de las SALIDAS
Actualizar_Salidas();       //actualiza salidas con los valores de los HR, de acuerdo con el resulatdo procesado en loop()
} 

/******************************Aquí termina loop y empiezan las subrutinas******************/
/*TiempoCiclo******************************************************OK, a veces se cuelga la comunicación, A VECES SOY TONTO, HABÍA PUESTO 2 BITS DE STOP AL MAESTRO Y 1 BIT AL ESCLAVO
*Esta función, determina la duración de la ejecución de la función loop()
*se determina también cual ha sido su valor máximo.
*/
void TiempoCiclo()
{
if(update_mb_slave(DirMB, regs, MB_REGS)) //*SI nos ha mandado algo el WK0500
                wdog = millis();                     //*leemos el tiempo
if((millis()-wdog) > regs[HR_TMP_WD])
      regs[HR_tcicloMB]++; //para que tarde más tiempo en desbordar cuento los ciclos de loop(), SE TIENE QUE BORRAR DESDE EL WK0500
      //if (regs[HR_tcicloMB] > lo que sea)   setup();  //podemos hacer que se reinicie el sitema si lleva mucho tiempo sin comunicarse 
if (primerCiclo) {tprev=millis(); primerCiclo= false;}
regs[HR_tciclo]=(int)(millis()-tprev); //determino tiempo transcurrido por loop() y lo guardo en HR
if (regs[HR_tciclo] > regs[HR_tcicloMax]) {regs[HR_tcicloMax] = regs[HR_tciclo];}
tprev=millis();
}

/*Actualizar_Entradas*****************************************************************************OK
*Esta función, al inicio de loop() actualiza todas las entradas con los valores que corresponda
*para poder procesarlas en el bucle loop(), guarda los datos leídos en el bit menos significativo
*del correspondiente registro de valor de escena del elemento leído
*/
void Actualizar_Entradas()
{
  //Esto habrá que modificarlo para cada módulo
  //He considerado que la escena 0 (LSB de valor de escena) es la que leo o escribo
   int pin;
   int HR_valorEscena;
  for (pin=DIN1; pin <= AINAUX; pin++){
    HR_valorEscena=HR_INFO+(pin-2)*5+3;
    //en la fórmula pone (pin-2)porque DIN1=2, el 5 (cada elemento digital usa (5*2)=10 bytes)
    //y es la posición tercera la del valor, considero la escena 0 la que hay que sacar
    //CON SWITCH NO ME FUNCIONA COMO QUIERO    
    if (pin==DIN1 || pin==DIN2 || pin==DIN3 || pin==DIN4 || pin==DIN5 || pin==DIN6 
       || pin==SWIN0 || pin==SWIN1 || pin==SWIN2 || pin==SWIN3 ) //entradas digitales
       regs[HR_valorEscena]=(regs[HR_valorEscena]&0xFFFE)|digitalRead(pin);
    
    if(pin==POTIN)  regs[HR_POTIN_ID+3]=  analogRead(pin);//entradas analógicas
    if(pin==AINAUX) regs[HR_AINAUX_ID+3]= analogRead(pin);//entradas analógicas
  }
}

/*ProcesarNormal*****************************************************************************OK
*Esta función, es la que se repite continuamente en el bucle loop() y realiza los procesos que
*se deseen con las entradas y las salidas
*/
void ProcesarNormal()
{
  //este es un sencillo ejemplo para probar el funcionamiento de lo escrito hasta ahora
  //sólo utilizo RELÉ1, RELÉ2 Y DIN6
  regs[HR_RELE1_ID+3]=(regs[HR_RELE1_ID+3]&0xFFFE) | (regs[HR_DIN6_ID+3]&0x1); //depende de la entrada DIN6
  //regs[HR_RELE2_ID+3]=memoria; // la enciendo siempre esto es para las pruebas
  
  
 
  digitalWrite(RELE2,regs[HR_RELE2_ID+3]);
  
   
}

/*Actualizar_Salidas*****************************************************************************OK
*Esta función, antes de terminar loop() actualiza todas las salidas con los valores que corresponda
*según las actualizaciones realizadas en loop().
*/
void Actualizar_Salidas()
{
  //Esto habrá que modificarlo para cada módulo
  //He considerado que la escena 0 (LSB de valor de escena) es la que leo o escribo
  int HR_valorEscena;
  byte pin;
  for (pin=RELE1; pin <= RELE4; pin++){
    HR_valorEscena=HR_INFO+(pin-2)*5+3;
    //en la fórmula pone (pin-2)porque DIN1=2, el 5 (cada elemento digital usa (5*2)=10 bytes)
    //y es la posición tercera la del valor, considero la escena 0 la que hay que sacar
    //para RELE1 sería 10+(8-2)*5+3 =43 que es la dirección del valor de escena del RELE1
    if (pin==RELE1 || pin==RELE2 || pin==RELE3 || pin==RELE4)
       digitalWrite(pin,regs[HR_valorEscena]&0x0001); //No consigo que me funcione como quiero con un switch
    }
}

#include "G:\PVKINN\MODBUSwk0400\ComandosWK0400.h"

/*ProcesarComando*****************************************************************************OK
*Si desde el inicio de la ejecución del loop() se ha recibido alguna orden desde el maestro WK0500
*esta función analiza el código recibido y los datos y verifica si existe el objeto y realiza la
*función indicada sobre él, devolviendo al master el mensaje adecuado.
*/
void ProcesarComando()
{
   if(regs[HR_CMDWK]==CMD_ACT_ELE) ActivarElemento(); 
   if(regs[HR_CMDWK]==CMD_DES_ELE) DesactivarElemento();
  //no me gusta lo que hace con el switch, además en lenguaje de alto nivel quedará más bonito
  //pero el código máquina generado es casi el mismo
  
 /* switch(regs[HR_CMDWK])
  {
   
         case CMD_ACT_ELE:ActivarElemento(); break;
         case CMD_DES_ELE:DesactivarElemento(); break;
         ------------------------------------*/
         
      /*   case CMD_ACT_GRP: ActivarGrupo(); break;
         case CMD_DES_GRP: DesactivarGrupo(); break;
         case CMD_EXE_ESC: EjecutarEscena(); break;
         case CMD_PUL_ELE: PulsoElemento();break;

         case CMD_CPH_ELE: ConsultarProgramaHorariosElemento();break;
         case CMD_CPH_GRP: ConsultarProgramaHorariosGrupo();break;
         case CMD_CPH_ESC: ConsultarProgramaHorariosEscena();break;
         case CMD_BPH_ELE: BorrarProgramaHorario();break;
         case CMD_APH_ELE: AnadirProgramaHorario();break;
         case CMD_MPH_ELE: ModificarProgramaHorario();break;*/
    
        // case CMD_CPUERTA: ConsultarPuertas();break;
        // case CMD_BPUERTA: BloquearPuerta();break;
        // case CMD_DPUERTA: DesbloquearPuerta();break;
        // case CMD_ALT_TRJ: AltaTarjetaPuerta();break;
        // case CMD_BAJ_TRJ: BajaTarjetaPuerta();break;
        // case CMD_BAJ_SIS: BajaTarjetaSistema();break;
        // case CMD_ALT_MTR: AltaTarjetaMaestra();break;
        // case CMD_ALT_SMT: AltaTarjetaSuperMaestra();break;
        // case CMD_BRT_CRR: BorradoTotalCerradura();break;

        // case CMD_LEE_TMP: LeerTemperatura();break;
        // case CMD_MOD_CSG: ModificarConsignaActual();break;
        // case CMD_MOD_MDD: ModificarConsignaModoDia();break; 
        // case CMD_MOD_MDN: ModificarConsignaModoNoche();break; 
        // case CMD_MOD_CAG: ModificarConsignaModoActualGlobalmente();break;
        // case CMD_MOD_MDG: ModificarConsignaModoDiaGlobalmente();break;
        // case CMD_MOD_MNG: ModificarConsignaModoNocheGlobalmente();break;
        // case CMD_ACT_HVA: ActivarHVAC();break;  
        // case CMD_DES_HVA: DesactivarHVAC();break;  
        // case CMD_ACT_HVG: ActivarHVACGlobalmente();break;
        // case CMD_DES_HVG: DesactivarHVACGlobalmente();break;
        // case CMD_MOD_LTS: ModificarLimiteTemperaturaSuperior();break;  
        // case CMD_MOD_LTI: ModificarLimiteTemperaturaInferior();break;  
        // case CMD_MOD_TSG: ModificarLimiteTemperaturaSuperiorGlobalmente();break;  
        // case CMD_MOD_TIG: ModificarLimiteTemperaturaInferiorGlobalmente();break;  

     //  default: ComandoError(); break;
    
  
}



/*Para disminuir el tamaño del fichero fuente he optado por colocar este include
*LO DE ESPECIFICAR EL SUBDIRECTORIO ES PORQUE LO LLEVO EN LA MEMORIA USB 
*Y COMO TRABAJO EN VARIOS SITIOS, ASI VA SIEMPRE CONMIGO     */
#include "G:\PVKINN\MODBUSwk0400\ModBusSlave.h"
