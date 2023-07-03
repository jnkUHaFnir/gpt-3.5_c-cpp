/*
I have a problem with printing 2d array. After allocating memory for matrix and filling it with 0's it cannot be printed. Here is my code.


int _tmain(int argc, _TCHAR* argv[]) {
    int m, n, num;
    float **tab1 = NULL;
    for (;;)
    {
        printf("\n1. Nowy budynek");
        printf("\n2. Wyswietl moc pobierana w pomieszczeniach");
        printf("\n3. Wlacz swiatlo");
        printf("\n4. Wylacz swiatlo");
        printf("\n0. Exit\n");
        scanf_s("%d", &num);

        if (num == 1)
        {
            do{
                printf("Podaj liczbe kondygnacji: ");
                scanf_s("%d", &m);
            } while (m < 0);
            do{
                printf("Podaj liczbe pomieszczen: ");
                scanf_s("%d", &n);
            } while (n < 0);
            tworzenie(m, n);
        }
        for (int i = m - 1; i >= 0; i--)
        {
            for (int j = 0; j < n; j++)
            {
                printf("%10.2f", tab1[i][j]);
            }
            printf("\n");
        }
        if (num == 2)
        {
            if (tab1 == NULL)
            {
                printf("\n Brak budynku! Stworz nowy.\n");
            }
            else
            wyswietlanie(tab1, m, n); <- it crashes here.
        }
        if (num == 3)
        {
            if (tab1 == NULL)
            {
                printf("\n Brak budynku! Stworz nowy.\n");
            }
            else
                wlaczanie(tab1, m, n);
        }
        if (num == 4)
        {
            if (tab1 == NULL)
            {
                printf("\n Brak budynku! Stwórz nowy.");
            }
            //else
            //wylaczanie(tab1,m, n);
        }
        if (num == 0)
        {
            //      exit(tab1,m, n);
        }
    }
    return 0;
}


Here is creating the table:


float** tworzenie(int m, int n)
{
    float **tab1;

    tab1 = (float**)malloc(m * sizeof(float*));
    for (int i = 0; i < n; i++)
    {
        tab1[i] = (float*)malloc(n * sizeof(float));
    }


    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            tab1[i][j] = 0;
        }
    }
    return tab1;
}


and here, printing(upside down):


void wyswietlanie(float **tab1, int m, int n) {
    for (int i = m - 1; i >= 0; i--) {
        for (int j = 0; j < n; j++) {
            printf("%10.2f", tab1[i][j]);
        }
        printf("\n");
    }
}

*/

float** tworzenie(int m, int n) {
    float** tab1;

    tab1 = (float**)malloc(m * sizeof(float*));
    for (int i = 0; i < m; i++) {  // Corrected loop condition
        tab1[i] = (float*)malloc(n * sizeof(float));
    }

    // Rest of your code...

    return tab1;
}
