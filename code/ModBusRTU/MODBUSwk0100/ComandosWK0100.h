
//*******************definici�n del c�digo de los comandos Winkhel
enum {
       CMD_ACT_ELE= 0x20,  //ACTIVAR ELEMENTO
       CMD_DES_ELE,    //DESACTIVAR ELEMENTO
       CMD_ACT_GRP,    //ACTIVAR GRUPO
       CMD_DES_GRP,    //DESACTIVAR GRUPO
       CMD_EXE_ESC,    //EJECUTAR ESCENA
       CMD_PUL_ELE,    //PULSO EN ELEMENTO

       CMD_CPH_ELE,    //CONSULTAR PROGRAMAS HORARIOS DE UN ELEMENTO
       CMD_CPH_GRP,    //CONSULTAR PROGRAMAS HORARIOS DE UN GRUPO
       CMD_CPH_ESC,    //CONSULTAR PROGRAMAS HORARIOS DE UNA ESCENA
       CMD_BPH_ELE,    //BORRAR PROGRAMA HORARIO DE UN ELEMENTO
       CMD_APH_ELE,    //A�ADIR PROGRAMA HORARIO DE UN ELEMENTO
       CMD_MPH_ELE,    //MODIFICAR PROGRAMA HORARIO DE UN ELEMENTO

       CMD_CPUERTA,    //CONSULTAR PUERTAS
       CMD_BPUERTA,    //BLOQUEAR PUERTA
       CMD_DPUERTA,    //DESBLOQUEAR PUERTA
       CMD_ALT_TRJ,    //ALTA DE TARJETA EN PUERTA
       CMD_BAJ_TRJ,    //BAJA DE TARJETA EN PUERTA
       CMD_BAJ_SIS,    //BAJA DE TARJETA EN SISTEMA
       CMD_ALT_MTR,    //ALTA DE TARJETA MAESTRA
       CMD_ALT_SMT,    //ALTA DE TARJETA SUPERMAESTRA
       CMD_BRT_CRR,    //BORRADO TOTAL DE CERRADURA

       CMD_LEE_TMP,    //LEER TEMPERATURA
       CMD_MOD_CSG,    //MODIFICAR CONSIGNA ACTUAL
       CMD_MOD_MDD,    //MODIFICAR CONSIGNA DEL MODO DIA
       CMD_MOD_MDN,    //MODIFICAR CONSIGNA DEL MODO NOCHE
       CMD_MOD_CAG,    //MODIFICAR CONSIGNA ACTUAL GLOBALMENTE
       CMD_MOD_MDG,    //MODIFICAR CONSIGNA DEL MODO DIA GLOBALMENTE
       CMD_MOD_MNG,    //MODIFICAR CONSIGNA DEL MODO NOCHE GLOBALMENTE
       CMD_ACT_HVA,    //ACTIVAR HVAC 
       CMD_DES_HVA,    //DESACTIVAR HVAC 
       CMD_ACT_HVG,    //ACTIVAR HVAC GLOBALMENTE
       CMD_DES_HVG,    //MODIFICAR LIMITE TEMPERATURA SUPERIOR 
       CMD_MOD_LTS,    //MODIFICAR LIMITE TEMPERATURA SUPERIOR 
       CMD_MOD_LTI,    //MODIFICAR LIMITE TEMPERATURA INFERIOR 
       CMD_MOD_TSG,    //MODIFICAR LIMITE TEMPERATURA SUPERIOR GLOBALMENTE
       CMD_MOD_TIG     //MODIFICAR LIMITE TEMPERATURA INFERIOR GLOBALMENTE 
};

/*buscarElemento*****************************************************************************
*Busca en los HR de identificador de elemento a ver si est� el elemento cuyo id_elemento 
*nos ha pasado el master WK0500 si lo encuentra devuelve la posici�n en la que lo ha encontrado
*si no lo encuentra devuelve un 0
*/
byte buscarElemento(int id_elemento)
{
int p;  
for(p = HR_INFO ;  p <= HR_DIN3_ID ; p += 5) //la entrada anal�gica auxiliar la analizo por separado
   if(regs[p] == id_elemento)return p; // si lo encuentra devuelve posici�n

   return 0;//NO lo ha encontrado
}  

byte posicion; //registro HR donde est�n los datos para ese elemento
//--------------------------------------------------
void ComandoError(){
//Serial.println("CODIGO DE COMANDO NO VALIDO");
}
//--------------------------------------------------
void ActivarElemento(){
//Serial.print("ACTIVAR ELEMENTO: ");

posicion=buscarElemento(regs[HR_ARG1]);
//en regs[HR_ARG1] est�n los 2 bytes de Id_elemento
if(posicion==HR_RELE1_ID || posicion==HR_RELE2_ID || posicion==HR_RELE3_ID || posicion==HR_RELE4_ID)
    regs[posicion + OFFSET_VESC] |=1;//ponemos un 1 en la escena cero(dejando los otros valores de escena) que es la que se activar� al final
      
 
}
//--------------------------------------------------
void DesactivarElemento(){
//Serial.print("DESACTIVAR ELEMENTO: ");
posicion=buscarElemento(regs[HR_ARG1]);
//en regs[HR_ARG1] est�n los 2 bytes de Id_elemento
if(posicion==HR_RELE1_ID || posicion==HR_RELE2_ID || posicion==HR_RELE3_ID || posicion==HR_RELE4_ID)
      regs[posicion + OFFSET_VESC] &=0xFFFE;//ponemos un 0 en la escena cero(dejando los otros valores de escena) que es la que se activar� al final
}
//--------------------------------------------------
void ActivarGrupo(){
//Serial.print("ACTIVAR GRUPO");
}
//--------------------------------------------------
void DesactivarGrupo(){
//Serial.print("DESACTIVAR GRUPO");
}
//--------------------------------------------------
void EjecutarEscena(){
//Serial.print("EJECUTAR ESCENA");
//Pregunta: en los 16 bits del argumento s�lo puede haber un bit a 1
}
//--------------------------------------------------
void PulsoElemento(){
//Serial.print("PULSO EN ELEMENTO");
//Pregunta: quien determina la duraci�n del pulso
}
//--------------------------------------------------

void ConsultarProgramaHorariosElemento(){
//Serial.print("CONSULTAR PROGRAMAS HORARIOS DE UN ELEMENTO");
}
//--------------------------------------------------
void ConsultarProgramaHorariosGrupo(){
//Serial.print("CONSULTAR PROGRAMAS HORARIOS DE UN GRUPO");
}
//--------------------------------------------------
void ConsultarProgramaHorariosEscena(){
//Serial.print("CONSULTAR PROGRAMAS HORARIOS DE UNA ESCENA");
}
//--------------------------------------------------
void BorrarProgramaHorario(){
//Serial.print("BORRAR PROGRAMA HORARIO");
}
//--------------------------------------------------
void AnadirProgramaHorario(){
//Serial.print("ANADIR PROGRAMA HORARIO");
}
//--------------------------------------------------
void ModificarProgramaHorario(){
//Serial.print("MODIFICAR PROGRAMA HORARIO DE UN ELEMENTO");
}
//--------------------------------------------------
   
void ConsultarPuertas(){
//Serial.print("CONSULTAR PUERTAS");
//Pregunta: las puertas no van conectadas al ModBus directamente
//pregunta: las puertas son los m�dulos WK0200
//pregunta: vamos a guardar el historial de aperturas
//pregunta: diferencia entre tarjeta maestra y supermaestra

}
//--------------------------------------------------
void BloquearPuerta(){
//Serial.print("BLOQUEAR PUERTA");
}
//--------------------------------------------------
void DesbloquearPuerta(){
//Serial.print("DESBLOQUEAR PUERTA");
}
//--------------------------------------------------
void AltaTarjetaPuerta(){
//Serial.print("ALTA DE TARJETA EN PUERTA");
}
//--------------------------------------------------
void BajaTarjetaPuerta(){
//Serial.print("BAJA DE TARJETA EN PUERTA");
}
//--------------------------------------------------
void BajaTarjetaSistema(){
//Serial.print("BAJA DE TARJETA EN SISTEMA");
}
//--------------------------------------------------
void AltaTarjetaMaestra(){
//Serial.print("ALTA DE TARJETA MAESTRA");
}
//--------------------------------------------------
void AltaTarjetaSuperMaestra(){
//Serial.print("ALTA DE TARJETA SUPERMAESTRA");
}
//--------------------------------------------------
void BorradoTotalCerradura(){
//Serial.print("BORRADO TOTAL DE CERRADURA");
}
//--------------------------------------------------
//NO QUEDA CLARO EL N� DE BYTES PARA CODIFICAR TERMOSTATO Y CONSIGNA
void LeerTemperatura(){
//Serial.print("LEER TEMPERATURA");
}
//--------------------------------------------------
void ModificarConsignaActual(){
//Serial.print("MODIFICAR CONSIGNA ACTUAL");
}
//--------------------------------------------------
void ModificarConsignaModoDia(){
//Serial.print("MODIFICAR CONSIGNA DEL MODO DIA");
}
//--------------------------------------------------
void ModificarConsignaModoNoche(){
//Serial.print("MODIFICAR CONSIGNA DEL MODO NOCHE");
}
//--------------------------------------------------
void ModificarConsignaModoActualGlobalmente(){
//Serial.print("MODIFICAR CONSIGNA ACTUAL GLOBALMENTE");
}
//--------------------------------------------------
void ModificarConsignaModoDiaGlobalmente(){
//Serial.print("MODIFICAR CONSIGNA DEL MODO DIA GLOBALMENTE");
}
//--------------------------------------------------
void ModificarConsignaModoNocheGlobalmente(){
//Serial.print("MODIFICAR CONSIGNA DEL MODO NOCHE GLOBALMENTE");
}
//--------------------------------------------------
void ActivarHVAC(){
//Serial.print("ACTIVAR HVAC ");
}
//--------------------------------------------------
void DesactivarHVAC(){
//Serial.print("DESACTIVAR HVAC ");
}
//--------------------------------------------------
void ActivarHVACGlobalmente(){
//Serial.print("ACTIVAR HVAC GLOBALMENTE ");
}
//--------------------------------------------------
void DesactivarHVACGlobalmente(){
//Serial.print("DESACTIVAR HVAC GLOBALMENTE ");
}
//--------------------------------------------------
void ModificarLimiteTemperaturaSuperior(){
//Serial.print("MODIFICAR LIMITE TEMPERATURA SUPERIOR  ");
}
//--------------------------------------------------
void ModificarLimiteTemperaturaInferior(){
//Serial.print("MODIFICAR LIMITE TEMPERATURA INFERIOR  ");
}
//--------------------------------------------------
void ModificarLimiteTemperaturaSuperiorGlobalmente(){
//Serial.print("MODIFICAR LIMITE TEMPERATURA SUPERIOR GLOBALMENTE ");
}
//--------------------------------------------------
void ModificarLimiteTemperaturaInferiorGlobalmente(){
//Serial.print("MODIFICAR LIMITE TEMPERATURA INFERIOR GLOBALMENTE ");
}
