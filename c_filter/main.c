// https://github.com/lawliet89/RTDSP/blob/master/lab5/RTDSP/direct-form-ii-transposed.c

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#define SIZE 5

double digitalFilter(double data);
double IIRFilter(double x);
//double *v;  // указатель буфера
double v[4];  // указатель буфера


// Переменные-характеристики фильтра, входные данные, кэфы, выходы y, необходимые для расчета
double x[] = {1241,1483,1716,1930,2118,2273,2387,2458,2482,2458,2387,2273,2118,1930,1716,1483,1241,999,766,551,363,209,94,24,0,24,94,209,363,551,766,999,1241,1483,1716,1930,2118,2273,2387,2458};
double b[] = {0.00319806247283222151, 0, -0.00639612494566444301, 0, 0.00319806247283222151};
double a[] = {1, -3.78125206442570505, 5.44829440482254412, -3.54414088511948755, 0.878834866871245235};

// размер массива кэфов
int N = 5;

int main()
{
    // вычисляем размер массива данных
    int x_size = sizeof(x) / sizeof(x[0]);

//    v = (double *) calloc(N-1, sizeof(double));

    uint16_t data_filtered = 0;
    double data_filtered_double = 0;

    for (int j = 0; j < x_size; j++) {

        data_filtered_double = IIRFilter(x[j]);

        data_filtered = round(data_filtered_double) + 2047;
        printf("y_%d = %u\n", j + 1, data_filtered);

    }

//    free(v);
    return 0;
}

double IIRFilter(double x){
    double y = 0;   // output
    int i = 0;  // loop index
    y = v[0] + b[0]*x;

    // обновляем буфер для следующей итерации
    for (; i < N-2; i++)
        v[i] = v[i+1] + b[i+1]*x - a[i+1]*y;

    v[N-2] = b[N-1]*x - a[N-1]*y;

    return y;
}

