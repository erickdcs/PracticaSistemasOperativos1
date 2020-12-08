// Alvaro Martinez 
// Erick Cardenas
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
typedef struct {
 short int ETQ;
 short int Datos[8];
} T_LINEA_CACHE;

void startUp(T_LINEA_CACHE* cache, char* ram, FILE* fRAM, FILE* fAccesos);
void startRAM(FILE* f, char* ram);
short int accesoMemoria(FILE* f);
short int numLinea(short int direccion);
short int etiqueta(short int direccion);
short int palabra(short int direccion);
short int numBloque(short int direccion);
int cargadoEnCache(short int direccion, T_LINEA_CACHE cache[4]);
void falloCache(short int direccion, T_LINEA_CACHE* cache, char* ram);
char aciertoCache(short int direccion, T_LINEA_CACHE* cache, char* ram);

unsigned int tiempoGlobal = 0;
unsigned int numFallos = 0;

int main(int argc, char** argv){
	
	T_LINEA_CACHE cache[4];
	char ram[1024];
	short int siguienteAcceso = 0;
	FILE* fRAM;
	FILE* fAccesos;
	char texto[100];
	int cantidad = 0;
	
	startUp(cache, ram, fRAM, fAccesos);
	printf("\nMemoria iniciada %s y esto es la ram", ram);
	fAccesos = fopen("accesos_memoria.txt", "r");
	printf("%c", ram[3]);
	
	while(siguienteAcceso!=-1){
		siguienteAcceso = accesoMemoria(fAccesos);
		
		if(siguienteAcceso == -1){
			printf("\nSe ha acabado de leer las direcciones de memoria");
			printf("\nTexto: %s", texto);
		}else{
			if (cargadoEnCache(siguienteAcceso, cache)==0){
				falloCache(siguienteAcceso,cache,ram);
			}else{
				texto[cantidad] = aciertoCache(siguienteAcceso, cache, ram);
				cantidad++;
			}
		}
	}
	return 0;
}

void startUp(T_LINEA_CACHE* cache, char* ram, FILE* fRAM, FILE* fAccesos){
	int i;
	int j;
	
	//Abrimos los ficheros en formato lectura
	fRAM = fopen("RAM.bin", "r");
	printf("\nAbriendo RAM");
	fAccesos = fopen("accesos_memoria.txt", "r");
	printf("\nAbriendo accesos");
	
	//Comprobamos que son accesibles
	if(fRAM == NULL){
		printf("\nNo se ha podido abrir el fichero RAM.bin");
		exit(-1);
	}
	if(fAccesos == NULL){
		printf("\nNo se ha podido abrir el fichero accesos_memoria.txt");
		exit(-1);
	}
	
	//Inicializamos la RAM
	startRAM(fRAM, ram);
	
	//Inicializamos la cache
	for(i=0; i < 4; i++){
		printf("\nEtiqueta cambiada");
		cache[i].ETQ = 0xFF;
		for(j = 0; j < 8; j++){
			printf("\nDato cambiado");
			cache[i].Datos[j] = 0;
		}
	}
	printf("\nAccesos iniciados");
	for(i=0; i < 4 ; i++){
		printf("\n ETIQ: %x", cache[i].ETQ);
		for(j=0; j < 8; j++){
			printf(" Dato: %d",cache[i].Datos[j]);  
		}
	}
	
}

short int accesoMemoria(FILE* fAccesos){
	short int direccion = 0;
	char aux[4] = "\n";
	
	if(feof(fAccesos)) return(-1);
	fread(aux,4,1,fAccesos); //Lee los 4 primeros bytes que encuentra en el fichero y los almacena en formato string	
	printf("\n Direccion leida: %s", aux);
	direccion = (short int)strtol(aux,NULL,16); //Convierte el string a formato numérico (teniendo en cuenta que estaban en base 16)
	aux[0] = getc(fAccesos); //Desecha el siguiente caracter "\n" contiguo a cada direccion 
	
	printf("\nSe quiere acceder a la direccion: %x\n", direccion);
	return direccion;
}

short int numLinea(short int direccion){ //Mantiene exclusivamente los bits de la linea, después los desplaza para que ocupen los bits de menor peso
	return ((direccion & 0b0001100000) >> 5);
}
short int etiqueta(short int direccion){ //Mantiene exclusivamente los bits de la etiqueta, después los desplaza para que ocupen los bits de menor peso
	return ((direccion & 0b1110000000) >> 7);
}
short int palabra(short int direccion){ //Mantiene exclusivamente los bits de la palabra, después los desplaza para que ocupen los bits de menor peso
	return (direccion & 0b0000011111);
}
short int numBloque(short int direccion){
	return ((direccion & 0b1111100000) >> 5);
}
int cargadoEnCache(short int direccion, T_LINEA_CACHE* cache){
		tiempoGlobal++;
	if(cache[numLinea(direccion)].ETQ == etiqueta(direccion)){ //Comprueba si la etiqueta de la direccion se corresponde con la etiqueta cargada en 
		return 1;											  //la linea correspondiente de la cache
	}else{
		return 0;
	}
}
void startRAM(FILE *fichero, char* ram){
	fichero = fopen("RAM.bin", "r");	// abrimos el fichero
	int cont = 0;	// contador para avanzar la posicion del array
	char c = 0;	//char auxiliar
	c = getc(fichero);// cargamos el primer caracter en c

	/// este bucle lee caracter por caracter hasta que se llegue al final del fichero
	while(!feof(fichero))
	{
		
		if(c != '\r' && c != '\0')
		{
			ram[cont] = c;
			cont++;
		}
		c =getc(fichero);
	}
	
	fclose(fichero);
}
void falloCache(short int direccion, T_LINEA_CACHE* cache, char* ram){
	numFallos++;
	short int linea = numLinea(direccion);
	short int bloque = numBloque(direccion);
	int i;
	sleep(2);
	printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X", tiempoGlobal, numFallos, direccion,etiqueta(direccion));
	printf(" linea %02X palabra %02X bloque %02X", linea, palabra(direccion), bloque);
	tiempoGlobal+=10;
	cache[linea].ETQ = etiqueta(direccion);
	for (i = 0; i < 8; i++){
		cache[linea].Datos[i] = ram[bloque+i];
	}
}
char aciertoCache(short int direccion, T_LINEA_CACHE* cache, char* ram){
	sleep(2);
	printf("T: %d, Acierto de CACHE, ADDR %04X ETQ %X", tiempoGlobal, direccion, etiqueta(direccion));
	printf(" linea %02X palabra %02X DATO %02X", numLinea(direccion), palabra(direccion), ram[direccion]);
	printf("%c", ram[direccion]);
	return ram[direccion];
}