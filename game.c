#include "procesos.h"
#define TRUE 1
#define FALSE 0
#define TAM 35

int operacion();

int main()
{
    srand(time(NULL));
    int res, band;
    int pos1 = 0, pos2 = 0, fin = TAM;

    do
    {
        //TODO INICIA JUGADOR 1
        system("cls");
        printf("\nJugador 1 \n");
        res = operacion();
        band=FALSE;
        if (pos1 + res > TAM)
        {
            for (int i = res; pos1 <= TAM && i != 0; i--)
            {
                if (pos1 + 1 <= TAM && band == FALSE)
                {
                    pos1 += 1;
                }
                else
                {
                    band = TRUE;
                    pos1 -= 1;
                }
            }
        }
        else
        {
            pos2 += res;
        }
        printf("Posicion %d", pos1);
        system("pause");

        //TODO INICIA JUGADOR 2
        system("cls");
        printf("\n Jugador 2 \n");
        res = operacion();
        band=FALSE;
        if (pos2 + res > TAM)
        {
            for (int i = res; pos2 <= TAM && i != 0; i--)
            {
                if (pos2 + 1 <= TAM && band == FALSE)
                {
                    pos2 += 1;
                }
                else
                {
                    band = TRUE;
                    pos2 -= 1;
                }
            }
        }
        else
        {
            pos2 += res;
        }
        printf("Posicion %d\n", pos2);
        system("pause");


    } while (pos1 != fin || pos2 != fin);

    return 0;
}

//* 1. Suma
//* 2. Resta
//* 3. Multiplicaion
//* 4. Division
int operacion()
{
    int n1, n2, oper, res;
    char op;
    int res2;
    n1 = rand() % 6 + 1;
    n2 = rand() % 6 + 1;
    oper = rand() % 4 + 1;

    if (n1 > n2 && oper == 4)
    {
        do
        {
            n1 = rand() % 6 + 1;
            n2 = rand() % 6 + 1;
        } while (n1 > n2);
    }

    switch (oper)
    {
    case 1:
        op = '+';
        res2 = n1 + n2;
        break;
    case 2:
        op = '-';
        res2 = n1 - n2;
        break;
    case 3:
        op = '*';
        res2 = n1 * n2;
        break;
    case 4:
        op = '/';
        res2 = n1 / n2;
        break;
    }


    printf("\n Resuelve la siguiente operacion: ");
    printf("%d %c %d", n1, op, n2); //! Creo que essto me da error el caracter
    res = validar(" ", 0, 100);

    if (res == res2)
    {
        return res;
    }
    else
    {
        return res2;
    }
}
