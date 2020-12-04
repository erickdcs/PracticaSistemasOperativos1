// Alvaro Martinez 
// Erick Cardenas

typedef struct {
 short int ETQ;
 short int Datos[8];
} T_LINEA_CACHE;

void startUp();
unsigned char[1024] startRAM();
void accessMemory(FILE* f);













int main(int argc, char** argv){
	int tiempoGlobal = 0;
	int numFallos = 0;
	T_LINEA_CACHE chache[4];
	unsigned char[1024] ram;
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}

void startUp(T_LINEA_CACHE cache[4], unsigned char ram[1024]){
	ram = startRam();
	int i;
	int j;
	for(i=0; i < 4; i++){
		cache[i].ETQ = 0xFF;
		for(j = 0; j < 8; j++){
			cache[i].Datos[j] = 0;
		}
	}
}