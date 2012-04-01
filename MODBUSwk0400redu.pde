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
 *************************************************************************/
#include <EEPROM.h>
//al final tengo otro #include para las librerias ModBus Slave
enum { //definición de pines en el WK0400,WK0401,WK0402,WK0403, asigna pines del 2 al 17 según el orden
      DIN1=2,DIN2,DIN3,DIN4,DIN5,DIN6,RELE1,RELE2,RELE3=15,RELE4,SWIN0=10,SWIN1,SWIN2,SWIN3,POTIN,AINAUX=17
     };
enum {//direcciones asignadas de la EEPROM
      EP_DirMB, //dirección 0
      EP_AjustesMB,
      EP_LogicaNeg, //ESTAS Y LA SIGUIENTE NO LAS MANDO ESCRIBIR EN NINGUN SITIO TADAVIA
      EP_tiempo_WD
     };  
     
enum {// valores usados para la configuración del ModBus
        COMM_BPS = 115200, //* baud rate */
        MB_SLAVE = 1,	   //* modbus slave id *///QUITAR uso variable DirMB
        PARITY = 'e'       //* even parity */
     };

/*Nombres para los Holding Registers para el WK0400*/
enum {        
        HR_CMDWK,       /* comando Winkhel */
                     //aquí tengo que dejar hueco para los parámetros de los comados
        HR_ESTADO=5, //Esto esta copiado de los de winkhel
        HR_TMP_WD,   //Tiempo de WatchDog
        HR_VERSION,  //versión del programa
        HR_N_DIR_MB, //nueva dirección del remoto en ModBus
        HR_N_AJU_MB, //puede que haya que quitarlo
        HR_INFO,     //dirección de inicio de los HR con datos de elementos
        HR_DIN1_ID=10,//esto sera la dirección del HR donde empiezan los datos del elemento digital
        HR_DIN2_ID=15,//utilizo 10 bytes para cada elemento aunque creo que me vale con 8
        HR_DIN3_ID=20,//ESTOY POR QUITAR TANTO IDENTIFICADOR QUE LUEGO NO USO(O TODAVÍA NO USO)
        HR_DIN4_ID=25,//
        HR_DIN5_ID=30,//
        HR_DIN6_ID=35,//
        HR_RELE1_ID=40,
        HR_RELE2_ID=45,
        HR_SWIN0_ID=50,
        HR_SWIN1_ID=55,
        HR_SWIN2_ID=60,
        HR_SWIN3_ID=65,
        HR_RELE3_ID=70,
        HR_RELE4_ID=75,
        HR_POTIN_ID=80,//El potenciómetro es un elemento analógico, utiliza 22 bytes
        HR_POTIN_VS0=83,//Valor de escena 0
        HR_AINAUX_ID=102,//La entrada analógica auxiliar también usa 22 bytes
        HR_AINAUX_VS0=105,//Valor de escena 0
        HR_tciclo,        //tiempo de ejecución de loop()
        HR_tcicloMax,     //tiempo máximo de loop()
        HR_tcicloMB,      //número de loop() desde
        HR_Log_Neg,
        MB_REGS	 	//* número total de registros en el esclavo */
};

  //estos me sobran con respecto a la primera idea
 enum{
        //HR_DIN1_IDREM,//SE TIENE QUE PODER QUITAR ya que tiene que coincidir con la del módulo
        OFFSET_IDELE,
        OFFSET_IDTPS,//identificador de tipo y nºsalida en este caso la 2, lo que no sé que hacer con el otro byte
        OFFSET_ESCN, //escenas a las que pertenece
        OFFSET_VESC//valor de escena
 };        


#define DirDefectoMB 18 //uso define en lugar de const para ahorrar espacio en cualquier sitio
unsigned char DirMB =1;// EEPROM.read (EP_DirMB);//DE MOMENTO LA DIRECCIÓN VALE 1
#define AjustesMB_Defecto  0x0A;// ESTO DEBE SER LO DE 8 bits Y PARIDAD PAR
unsigned char AjustesMB = EEPROM.read (EP_AjustesMB);

int regs[MB_REGS];     //*los holding registers son de 16 bits
byte pinIO[]={0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0};//0=INPUT, 1=OUTPUT, las dos primeras son para pin 0 y 1 que no existen
unsigned long wdog = 0;         //* watchdog */
unsigned long tprev ;           //* previous time*/
boolean primerCiclo=true;
int update_mb_slave(unsigned char slave, int *regs,unsigned int regs_size) ;//prototipo de función
void configure_mb_slave(long baud, char parity, char txenpin);//prototipo de función

void setup() 
{
        int pin;
        configure_mb_slave(COMM_BPS, PARITY, 0);//*cuando ponga varios módulos poner un 3 EN LUGAR DE 0
        for( pin=DIN1; pin <= AINAUX; pin++) pinMode(pin, pinIO[pin]);
        if (DirMB >246 || DirMB <1 ){//LO DEL 246 HAY QUE VERIFICARLO
            DirMB=DirDefectoMB;
            //EEPROM.write (EP_DirMB,DirMB); //DE MOMENTO NO ESCRIBIR EN LA EEPROM
            AjustesMB = AjustesMB_Defecto; //creo que lo voy a quitar
            //EEPROM.write (EP_AjustesMB,AjustesMB_Defecto);//DE MOMENTO NO ESCRIBIR EN LA EEPROM
            }
        regs[HR_VERSION]=0312;//marzo 2012
        regs[HR_N_DIR_MB]=DirMB; //guardo en holding register la direccion de MB
        regs[HR_N_AJU_MB]=AjustesMB; //puede que haya que quitarlo
        regs[HR_tcicloMax]=0;
        regs[HR_TMP_WD]=666; //lo he puesto a voleo, nº de la bestia del perro guardian
        
}

void loop() 
{
regs[HR_CMDWK]=0xFFFF;
TiempoCiclo();   //Determinamos cuanto tiempo dura un ciclo y cual ha sido el máximo también se deja en unos HR
Actualizar_Entradas();       //actualiza entradas y las guarda en HR y operará con los valores leídos   
if(regs[HR_CMDWK]!=0xFFFF)
     ProcesarComando();//Ha llegado un comando desde el WK0500     
//lo de lógica negativa poner aquí
Actualizar_Salidas();       //actualiza salidas con los valores de los HR, de acuerdo con el resulatdo procesado en loop()
} 

/******************************Aquí termina loop y empiezan las subrutinas******************/
/*TiempoCiclo*****************************************************************************
*Esta función, determina la duración de la ejecución de la función loop()
*se determina también cual ha sido su valor máximo.
*/
void TiempoCiclo()
{
if(update_mb_slave(DirMB, regs, MB_REGS)) //*Nos ha mandado algo el WK0500
                wdog = millis();                     //*leemos el tiempo
if((millis()-wdog) > regs[HR_TMP_WD])
      regs[HR_tcicloMB]++; //para que tarde más tiempo en desbordar cuento los ciclos de loop(), SE TIENE QUE BORRAR DESDE EL WK0500
if (primerCiclo) {tprev=millis(); primerCiclo= false;}
regs[HR_tciclo]=(int)(millis()-tprev); //determino tiempo transcurrido por loop() y lo guardo en HR
if (regs[HR_tciclo] > regs[HR_tcicloMax]) {regs[HR_tcicloMax] = regs[HR_tciclo];}
tprev=millis();
}

/*Actualizar_Entradas*****************************************************************************
*Esta función, al inicio de loop() actualiza todas las entradas con los valores que corresponda
*para poder procesarlas en el bucle loop()
*/
void Actualizar_Entradas()
{
  //Esto habrá que modificarlo para cada módulo
  //He considerado que la escena 0 (LSB de valor de escena) es la que leo o escribo
  int HR_valorEscena;
  byte pin;
  for (pin=DIN1; pin <= AINAUX; pin++){
    HR_valorEscena=HR_INFO+(pin-2)*5+3;
    //en la fórmula pone (pin-2)porque DIN1=2, el 5 (cada elemento digital usa (5*2)=10 bytes)
    //y es la posición tercera la del valor, considero la escena 0 la que hay que sacar
    switch(pin)
    {
      case  DIN1: DIN2: DIN3: DIN4: DIN5: DIN6:SWIN0: SWIN1: SWIN2: SWIN3:
         regs[HR_valorEscena]=(regs[HR_valorEscena]&0xFFFE)|digitalRead(pin);
         break;
      case RELE1: RELE2: RELE3: RELE4:
         //como son salidas no hay quehacer nada
         break;
      case POTIN:
         regs[HR_POTIN_VS0]=analogRead(POTIN); //se guarda en HR valor de escena 0
         break;
      case AINAUX:
         regs[HR_AINAUX_VS0]=analogRead(AINAUX);      
         break;
   } 
  }
}

/*Actualizar_Salidas*****************************************************************************
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
    switch(pin)
    {
      case SWIN0: SWIN1: SWIN2: SWIN3://esto es porque los pines de los relés no van seguidos
           break;
      case RELE1: RELE2: RELE3: RELE4:
         digitalWrite(pin,regs[HR_valorEscena]&0x0001);
         break;
    } 
  }
}

#include "M:\PVKINN\MODBUSwk0400redu\ComandosWK0400.h"
/*ProcesarComando*****************************************************************************
*Si desde el inicio de la ejecución del loop() se ha recibido alguna orden desde el maestro WK0500
*esta función analiza el código recibido y los datos y verifica si existe el objeto y realiza la
*función indicada sobre él, devolviendo al master el mensaje adecuado.
*/
void ProcesarComando()
{
  //este código difiere del de winkhel
  switch(regs[HR_CMDWK])
  {
         case CMD_ACT_ELE: ActivarElemento(); break;
         case CMD_DES_ELE: DesactivarElemento(); break;
         case CMD_ACT_GRP: ActivarGrupo(); break;
         case CMD_DES_GRP: DesactivarGrupo(); break;
         case CMD_EXE_ESC: EjecutarEscena(); break;
         case CMD_PUL_ELE: PulsoElemento();break;

         case CMD_CPH_ELE: ConsultarProgramaHorariosElemento();break;
         case CMD_CPH_GRP: ConsultarProgramaHorariosGrupo();break;
         case CMD_CPH_ESC: ConsultarProgramaHorariosEscena();break;
         case CMD_BPH_ELE: BorrarProgramaHorario();break;
         case CMD_APH_ELE: AnadirProgramaHorario();break;
         case CMD_MPH_ELE: ModificarProgramaHorario();break;
    
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

         default: ComandoError(); break;
    
  }
}



/*Para disminuir el tamaño del fichero fuente he optado porcolocar este include
*LO DE ESPECIFICAR EL SUBDIRECTORIO ES PORQUE LO LLEVO EN LA MEMORIA USB 
*Y COMO TRABAJO EN VARIOS SITIOS, ASI VA SIEMPRE CONMIGO     */
#include "M:\PVKINN\MODBUSwk0400redu\ModBusSlave.h"
