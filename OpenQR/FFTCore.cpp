//
// Created by DxxK on 2017/11/21.
//
#include "FFTCore.h"

const std::complex<double> I(0, 1);
const double PI = 3.14159265358979323846;

int reverseBits(unsigned short digitsCount, int value)
{
    if (value >> digitsCount > 0) return -1;

    int res = 0;
    for (int d = 0; d < digitsCount; d++)
    {
        res = (res * 2 + (value % 2));
        value /= 2;
    }

    return res;
}

void DiscreteFourierFast(const std::complex<double>* f, int i_max, std::complex<double>* F, fourier_transform_direction ftd)
{
    if (i_max <= 0 || ((i_max & (i_max - 1)) != 0)) throw INCORRECT_SPECTRUM_SIZE_FOR_FFT;

    double norm, exp_dir;
    switch (ftd)
    {
        case ftdFunctionToSpectrum:
            norm = 1;
            exp_dir = -1;
            break;
        case ftdSpectrumToFunction:
            norm = 1.0 / i_max;
            exp_dir = 1;
            break;
        default:
            throw UNSUPPORTED_FTD;
    }

    int NN = i_max, digitsCount = 0;
    while (NN >>= 1) digitsCount++;

    // Allocating 2 buffers with n std::complex values
    std::complex<double>** buf = new std::complex<double>* [2];
    for (int i = 0; i < 2; i++)
    {
        buf[i] = new std::complex<double>[i_max];
    }

    // Grouping function values according to the binary-reversed index order
    int cur_buf = 0;
    for (int i = 0; i < i_max; i++)
    {
        buf[cur_buf][i] = f[reverseBits(digitsCount, i)];
    }

    int exp_divider = 1;
    int different_exps = 2;
    int values_in_row = i_max / 2;
    int next_buf = 1;

    for (int step = 0; step < digitsCount; step++)
    {
        for (int n = 0; n < different_exps; n++)
        {
            std::complex<double> xp = exp((double)(exp_dir * PI * n / exp_divider) * I);

            for (int k = 0; k < values_in_row; k++)
            {
                std::complex<double>* pf = &buf[cur_buf][2 * k + (n % (different_exps / 2)) * (values_in_row * 2)];
                buf[next_buf][n * values_in_row + k] = (*pf) + (*(pf + 1)) * xp;
            }
        }

        exp_divider *= 2;
        different_exps *= 2;
        values_in_row /= 2;
        cur_buf = next_buf;
        next_buf = (cur_buf + 1) % 2;
    }

    // Norming, saving the result
    for (int i = 0; i < i_max; i++)
    {
        F[i] = norm * buf[cur_buf][i];
    }

    // Freeing our temporary buffers
    for (int i = 0; i < 2; i++)
    {
        delete [] buf[i];
    }
    delete [] buf;

}

void DiscreteFourierFast2D(const std::complex<double>* f, int i_max, int j_max, std::complex<double>* F, fourier_transform_direction ftd)
{
    std::complex<double>* phi = new std::complex<double>[i_max * j_max];
    for (int m = 0; m < j_max; m++)
    {
        DiscreteFourierFast(&f[i_max * m], i_max, &phi[i_max * m], ftd);
    }

    std::complex<double>* phi_t = new std::complex<double>[j_max * i_max];

    for (int p = 0; p < i_max; p++)
        for (int q = 0; q < j_max; q++)
        {
            phi_t[p * j_max + q] = phi[q * i_max + p];
        }

    std::complex<double>* F_t = phi;

    for (int i = 0; i < i_max; i++)
    {
        DiscreteFourierFast(&phi_t[j_max * i], j_max, &F_t[j_max * i], ftd);
    }

    for (int q = 0; q < j_max; q++)
        for (int p = 0; p < i_max; p++)
        {
            F[q * i_max + p] = F_t[p * j_max + q];
        }

    delete [] F_t;
    delete [] phi_t;
}

void FastFourierTransform(const std::complex<double>* input, int inputNumber, std::complex<double>* output, fourier_transform_direction ftd)
{
    if(!(inputNumber & (inputNumber - 1)) != 0)
        DiscreteFourierFast(input,inputNumber,output,ftd);
    else
    {
        int iMax=log2(inputNumber)+1;
        iMax=1<<iMax;
        std::complex<double>* inputWithPow2=new std::complex<double>[iMax];
        std::complex<double>* outputWithPow2=new std::complex<double>[iMax];
        for(int i=0;i<inputNumber;++i)
            inputWithPow2[i]=input[i];
        for(int i=inputNumber;i<iMax;++i)
            inputWithPow2[i]=0;
        DiscreteFourierFast(inputWithPow2,iMax,outputWithPow2,ftd);
        for(int i=0;i<inputNumber;++i)
            output[i]=outputWithPow2[i];
        delete[] inputWithPow2;
        delete[] outputWithPow2;
    }

}
