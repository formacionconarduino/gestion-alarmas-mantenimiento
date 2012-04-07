enum { //definici�n de pines en el WK0400,WK0401,WK0402,WK0403, asigna pines del 2 al 17 seg�n el orden
      DIN1=2,DIN2,DIN3,DIN4,DIN5,DIN6,RELE1,RELE2,RELE3=15,RELE4,SWIN0=10,SWIN1,SWIN2,SWIN3,POTIN,AINAUX=17
     };
int pinIO[]={0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0};  //0=INPUT, 1=OUTPUT, las dos primeras son para pin 0 y 1 que no existen 

enum {//direcciones asignadas de la EEPROM
      EP_DirMB, //direcci�n 0
      EP_AjustesMB,
      EP_LogicaNeg, //ESTAS Y LA SIGUIENTE NO LAS MANDO ESCRIBIR EN NINGUN SITIO TADAVIA
      EP_tiempo_WD
     };  
     
enum {// valores usados para la configuraci�n del ModBus
        COMM_BPS = 115200, //* baud rate */
        MB_SLAVE = 1,	   //* modbus slave id *///QUITAR uso variable DirMB
        PARITY = 'e'       //* even parity */
     };

/*Nombres para los Holding Registers para el WK0400*/
enum {        
        HR_INICIO,
        HR_CMDWK,       /* comando Winkhel */
        HR_ARG1,             //aqu� tengo que dejar hueco para los par�metros de los comados
        HR_ESTADO=5, //Esto esta copiado de los de winkhel
        HR_TMP_WD,   //Tiempo de WatchDog
        HR_VERSION,  //versi�n del programa
        HR_N_DIR_MB, //nueva direcci�n del remoto en ModBus
        HR_N_AJU_MB, //puede que haya que quitarlo
        HR_INFO=10,     //direcci�n de inicio de los HR con datos de elementos
        HR_DIN1_ID=10,//esto sera la direcci�n del HR donde empiezan los datos del elemento digital
        HR_DIN2_ID=15,//utilizo 10 bytes para cada elemento aunque creo que me vale con 8
        HR_DIN3_ID=20,//ESTOY POR QUITAR TANTO IDENTIFICADOR QUE LUEGO NO USO(O TODAV�A NO USO)
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
        HR_POTIN_ID=80,//El potenci�metro es un elemento anal�gico, utiliza 22 bytes
        HR_POTIN_VS0=83,//Valor de escena 0
        HR_AINAUX_ID=102,//La entrada anal�gica auxiliar tambi�n usa 22 bytes
        HR_AINAUX_VS0=105,//Valor de escena 0
        //HR para los valores de escena de AINAUX que por ser analógico son 8 Bytes
        HR_tciclo=113,        //tiempo de ejecuci�n de loop() HR106 no
        HR_tcicloMax,     //tiempo m�ximo de loop()
        HR_tcicloMB,      //n�mero de loop() desde la última orden del master, este registro va a toda pastilla
        HR_Log_Neg,
        MB_REGS	 	//* n�mero total de registros en el esclavo */
};

 
 enum{
        OFFSET_IDELE=0,//offset del identificador de elemento
        OFFSET_IDTPS,//offset identificador de tipo y n�salida en este caso la 2, lo que no s� que hacer con el otro byte
        OFFSET_ESCN, //offset escenas a las que pertenece
        OFFSET_VESC//offset valor de escena
 };        
