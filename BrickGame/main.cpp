#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <conio.h>
#include <stdio.h>
#include <vector>

int TAM_ARREGLO_X = 11;
int TAM_ARREGLO_Y = 20;
int vidas = 3;//Cantidad de errores posibles
int nivel = 1;//contado de niveles durante el juego
int tiempo1 = 0;//contador del tiempo durante el juego ara referencia de los niveles
int tiempo2 = 0;//Contador mostrado en la pantalla
int puntaje = 0;//contador de puntaje durante el juego
std::vector<int> historial;//vector usado para guardar los datos de los juegos
int tiempoCiclo = 500;//Tiempo por cada ciclo, usado en el sleep
bool carroEnemigo;//Comprueba si hay un carro enemigo en la pantalla
char cuadros = 178;//Código ASCII para cuadros
char mapa[11][20];//Declaración del arreglo qu es usado para generar el mapa

//Función utilizada para mover el cursor de el cmd a un lugar específico
void gotoXY(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//Inicializa el arreglo previo a el inicio del juego
void iniMapa()
{
	for (int i = 0; i < TAM_ARREGLO_Y - 1; i++)
	{
		mapa[0][i] = cuadros;
		mapa[10][i] = cuadros;
	}

	for (int i = 1; i < TAM_ARREGLO_X - 1; i++)
	{
		for (int j = 0; j < TAM_ARREGLO_Y; j++)
		{
			mapa[i][j] = 040;
		}
	}


}

//Reinicia la pantalla de juego (Borra los carros de la pantalla)
void reiniciarMapa()
{
	for (int j = 0; j < 20; j++)
		for (int i = 1; i < 10; i++)
			mapa[i][j] = 040;
		
}

//Funcion usada para imprimir en pantalla con coordenadas
void imprimirEnPantalla(int x, int y)
{
	if ((y >= 0) && (y < 20))
		mapa[x][y] = cuadros;
}

//Clase del carro enemigo y sus funciones respectivas
class CarroEnemigo
{
public:
	CarroEnemigo()
	{
		XCarroEnem = 5;
		YCarroEnem = 1;
	}
	int XCarroEnem;
	int YCarroEnem;
	void generarEnemigo()
	{
		if (YCarroEnem == 25)
		{
			int numRand = rand() % 3;
			if (numRand == 0)
				XCarroEnem = 2;
			else if (numRand == 1)
				XCarroEnem = 5;
			else if (numRand == 2)
				XCarroEnem = 8;
		YCarroEnem = 1;
		}
	}
	void imprimirEnemigo()
	{
		//Última Fila del carro
		imprimirEnPantalla(XCarroEnem - 1, YCarroEnem);
		imprimirEnPantalla(XCarroEnem, YCarroEnem);
		imprimirEnPantalla(XCarroEnem + 1, YCarroEnem);
		//Fila 4 del carro
		imprimirEnPantalla(XCarroEnem, YCarroEnem - 1);
		//Fila 3 del carro
		imprimirEnPantalla(XCarroEnem, YCarroEnem - 2);
		//Fila 2 del carro
		imprimirEnPantalla(XCarroEnem - 1, YCarroEnem - 3);
		imprimirEnPantalla(XCarroEnem, YCarroEnem - 3);
		imprimirEnPantalla(XCarroEnem + 1, YCarroEnem - 3);
		//Fila 1 del carro
		imprimirEnPantalla(XCarroEnem, YCarroEnem - 4);
	}
	void movimiento()
	{
		YCarroEnem++;
	}
};

//Clase del carro utilizado por el jugador
class CarroJugador
{
public:
	int XCarro;
	int YCarro;
	CarroJugador()
	{
		XCarro = 5;
		YCarro = 18;
	}
	void imprimirCarro()
	{
		//Última Fila del carro
		imprimirEnPantalla(XCarro - 1, YCarro);
		imprimirEnPantalla(XCarro, YCarro);
		imprimirEnPantalla(XCarro + 1, YCarro);
		//Fila 4 del carro
		imprimirEnPantalla(XCarro, YCarro - 1);
		//Fila 3 del carro
		imprimirEnPantalla(XCarro, YCarro - 2);
		//Fila 2 del carro
		imprimirEnPantalla(XCarro - 1, YCarro -3);
		imprimirEnPantalla(XCarro, YCarro - 3);
		imprimirEnPantalla(XCarro + 1, YCarro - 3);
		//Fila 1 del carro
		imprimirEnPantalla(XCarro, YCarro - 4);
	}
	void movIzq()
	{
		if (XCarro != 2)
			XCarro = XCarro - 3;
	}
	void movDer()
	{
		if (XCarro != 8)
			XCarro = XCarro + 3;
	}
	bool comprobarColision(CarroEnemigo *CE, int &vidas)
	{
		if ((CE->XCarroEnem == XCarro) && (CE->YCarroEnem > 13))
		{
			CE->YCarroEnem = 25;
			vidas--;
			return true;
		}
		return false;
	}
};


//std::mutex m;
//std::condition_variable cv;
//bool ready = false;
//bool processed = false;

//void pauseFunction()
//{
//	std::unique_lock<std::mutex> lk(m);
//	cv.wait(lk, [] {return ready; });
//
//	gotoXY(30, 9);
//	std::cout << "PAUSA";
//
//	gotoXY(30, 11);
//	std::cout << "Presione una tecla para continuar....";
//	std::cin.get();
//
//	processed = true;
//
//	lk.unlock();
//	cv.notify_one();
//}

bool tecladoRun;
//Detecta el estado del teclado y realiza los movimientos
//Incluye pausa y aceleración, además de movimientos a los lados
void inputs(CarroJugador *carro)
{
	tecladoRun = true;
	while(tecladoRun){
		if (GetAsyncKeyState(VK_LEFT) & (0x8000 != 0))
			carro->movIzq();
		if (GetAsyncKeyState(VK_RIGHT) & (0x8000 != 0))
			carro->movDer();
		if (GetAsyncKeyState(VK_UP) & (0x8000 != 0))
		{	
			tiempoCiclo = tiempoCiclo * 0.7;
		}
		//if (GetAsyncKeyState(0x50) & (0x8000 != 0))//Usa la tecla 'P' para pausar
		//{
		//	std::thread pause(pauseFunction);
		//	{
		//		std::lock_guard<std::mutex> lk(m);
		//		ready = true;
		//		cv.notify_one();
		//	}

		//	{
		//		std::unique_lock<std::mutex> lk(m);
		//		cv.wait(lk, [] {return processed; });
		//	}
		//	pause.join();
		//	system("cls");
		//}
		if (GetAsyncKeyState(0x1B) & (0x8000 != 0))//Presionar Esc cierra el juego
			exit(1);
	}
}

void puntajeActualizar(CarroEnemigo *CE, CarroJugador *CJ)//Maneja el puntaje según la colisión
{
	if (CE->YCarroEnem == 25)
	{
		if (nivel == 1)
			puntaje += 20;
		if (nivel == 2)
			puntaje += 40;
		if (nivel == 3)
			puntaje += 60;
		if (nivel == 4)
			puntaje += 80;
		if (nivel == 5)
			puntaje += 100;
		if (nivel == 6)
			puntaje += 130;
		if (nivel == 7)
			puntaje += 170;
		if (nivel == 8)
			puntaje += 220;
		if (nivel == 9)
			puntaje += 280;
		if (nivel == 10)
			puntaje += 350;
	}

	if (CJ->comprobarColision(CE, vidas) == true)
	{
		if (nivel == 1)
			puntaje -= 20;
		if (nivel == 2)
			puntaje -= 40;
		if (nivel == 3)
			puntaje -= 60;
		if (nivel == 4)
			puntaje -= 80;
		if (nivel == 5)
			puntaje -= 100;
		if (nivel == 6)
			puntaje -= 130;
		if (nivel == 7)
			puntaje -= 170;
		if (nivel == 8)
			puntaje -= 220;
		if (nivel == 9)
			puntaje -= 280;
		if (nivel == 10)
			puntaje -= 350;
	}
}

bool tiempoContRun;
void tiempoCont()//Maneja el tiempo durante la partida
{
	tiempoContRun = true;
	while (tiempoContRun)
	{
		tiempo1++;
		tiempo2++;

		if ((tiempo1 == 21) && (nivel != 10))
		{
			nivel++;
			tiempoCiclo = tiempoCiclo * 0.7;
			tiempo1 = 0;
			system("cls");
		}
		Sleep(1000);
	}
}

void mostrarDatos()//Muestra los datos durante la partida
{
	gotoXY(15, 5);
	std::cout << "VIDAS: " << vidas;

	gotoXY(15, 7);
	std::cout << "NIVEL: " << nivel;

	gotoXY(15, 9);
	std::cout << "TIEMPO: " << tiempo2;

	gotoXY(15, 11);
	std::cout << "PUNTAJE: " << puntaje;
}


void pauseFunction()//Pausa el juego dentro del while loop con la tecla 'P'
{
	if (GetAsyncKeyState(0x50) & (0x8000 != 0))
	{
		gotoXY(30, 9);
		std::cout << "PAUSA";
		
		gotoXY(30, 11);
		system("pause");
		system("cls");
	}
}

void borrarCursor()//Borra el cursor durante el juego
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 100;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &cursor);
}

//Imprime en Pantalla el arreglo que conforma el mapa y comienza el ciclo
void comenzarJuego()
{
	tiempoContRun = true;
	
	system("cls");
	nivel = 1;
	tiempoCiclo = 500;
	tiempo1 = 0;
	tiempo2 = 0;
	vidas = 3;
	CarroJugador CJ = CarroJugador();
	CarroEnemigo CE = CarroEnemigo();

	std::thread teclado(inputs, &CJ);//Hilo que detecta el estado del teclado
	std::thread tiempo3(tiempoCont);
	
	bool running = true;//Variable que mantiene el juego corriendo
	while (running)
	{
		borrarCursor();
		reiniciarMapa();
		mostrarDatos();
		CE.generarEnemigo();
		CE.imprimirEnemigo();
		CE.movimiento();
		CJ.imprimirCarro();
		CJ.comprobarColision(&CE, vidas);
		if (vidas == 0)
			running = false;
		pauseFunction();

		for (int j = 0; j < 20; j++)
		{
			for (int i = 0; i < 11; i++)
			{
				if (i == 0 || i == 10)
				{
					gotoXY(i, j);
					std::cout << cuadros;
				}
				else if (mapa[i][j] == cuadros)
				{
					gotoXY(i, j);
					std::cout << cuadros;
				}
				else
				{
					gotoXY(i, j);
					std::cout << " ";
				}
			}
		}
		puntajeActualizar(&CE, &CJ);
		Sleep(tiempoCiclo);
	}
	teclado.detach();
	tiempo3.detach();
	tecladoRun = false;
	tiempoContRun = false;
	auto tmp = historial.begin();
	tmp = historial.insert(tmp, puntaje);
	puntaje = 0;
	system("cls");
}


void imprimirHistorial()
{
	system("cls");
	if (historial.empty())
	{
		std::cout << "No hay registros..." << std::endl;
		system("pause");
		system("cls");
		return;
	}
	std::cout << "HISTORIAL" << std::endl;
	for (int i = 0; i < historial.size(); i++)
	{
		std::cout << "Juego " << (i + 1) << ":" << historial.at(i) << std::endl;
	}
	std::cout << std::endl;
	system("pause");
	system("cls");
	return;
}

void menu()
{
	gotoXY(18, 3);
	std::cout << "BRICK GAME";

	gotoXY(16, 6);
	std::cout << "1 - Jugar";
	gotoXY(16, 7);
	std::cout << "2 - Ver historial";
	gotoXY(16, 8);
	std::cout << "3 - Salir";

	gotoXY(21, 10);
	int opcion;
	std::cin >> opcion;

	if (opcion == 1)
		comenzarJuego();
	if (opcion == 2)
		imprimirHistorial();
	if (opcion == 3)
		exit(1);
	opcion = 0;
	menu();
}


int main()
{
	menu();

	system("pause");

	return 0;
}