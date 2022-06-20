#include <cstdio>
#include <cstdint>
#include <string>

#include<hip_runtime.h>
#include <hip/hip_fp16.h>

using half = __half;


enum class print_table_option
{
    PRINT_NUMBER,
    PRINT_EPSILON,
};

// f16 (1, 5, 10), 
template<unsigned int ebit, unsigned int mbit>
uint16_t genFp(bool sign, uint16_t exp, uint16_t mnt)
{
    
    return ( (sign ? (1<<(ebit+mbit)) : 0) | (exp<<mbit) | mnt); 
}


void printNumberHeader(FILE *fpout, uint16_t mmin, uint16_t mmax)
{
    fprintf(fpout, "Mantissa     :");
    for(uint16_t i = mmin; i <= mmax; i++)
    {
        fprintf(fpout, "%14x ", i);
    }
    fprintf(fpout, "\n");
}

/*
 *  Print number table
 */
template<typename T, bool is2DPrint, int expBits, int mntBits>
void printNumber(FILE* fpout, bool sign, bool skip_non_normal)
{
    const unsigned int mmin = 0;
    const unsigned int mmax = (1<<mntBits) - 1;
    
    unsigned int emin, emax; 

    if (skip_non_normal)
    {
        emin = 1;                    // 0 -> 0.0, subnormal or denorm  
        emax = (1<<expBits) - 2;     // (1<<expBits)-1 --> -0.0, NaN, INF 
    }
    else
    {
        emin = 0;
        emax = (1<<expBits) - 1; 
    }
        

    // printing the values 
    for (uint16_t e=emin; e <= emax; e++)
    {
        if (is2DPrint)
        {
            fprintf(fpout, "SIGN=%d EXP=0x%x : ", sign?1:0, e);  
        }
        for (uint16_t m=mmin; m <= mmax; m++)
        {
            uint16_t ival = genFp<expBits, mntBits>(sign, e, m);

            //T hval(ival);                           // using constructor 
            
            half hval = reinterpret_cast<half &> (ival);
            
            
            float fval = float(hval);
            if (is2DPrint)
            {
                fprintf(fpout, "%16.10f ", fval);  
            }
            else 
            {
                fprintf(fpout, "%16.10f ", fval);  
            }
        }
        if (is2DPrint)
        {
            fprintf(fpout, "\n");
        }
    }
    if (!is2DPrint)
    {
        fprintf(fpout, "\n");
    }

}


#if 0
template<typename T>
T Epsilon()
{
    int i=0;
    const int maxiter = 100;
    
    T half_value = (half)0.5f;
    T one_value = (half)1.0f;
    T mval = (half)0.0f, f = (half)0.5f; 
    T eps;
    
    do 
    {
        eps = f;
        f = f * half_value;
        //f *= half_value;
        mval = one_value + f;
        i++;
    } while(mval != one_value && i < maxiter);

    return(eps);
}
#endif


template <typename T>
void printEpsilon(FILE *fpout)
{
   // if (std::is_same<T, half>::value)
   //    fprintf(fpout, "Epsilon (FP16) = %.8f\n", float(Epsilon<T>()));
   fprintf(fpout, "Not Implemented yet\n");
}

template <typename T>
void printNumberTable(FILE *fpout, bool skip_non_normal)
{
    
    // only considering the fp16 here 
    constexpr int expBits = 5; 
    constexpr int mntBits = 10;


    const int emin = 0;
    const int emax = (1<<expBits) - 1; 
    const int mmin = 0;
    const int mmax = (1<<mntBits) - 1;
    printNumberHeader(fpout, mmin, mmax);                             // printing header 
    
    // 
    printNumber<T, true, expBits, mntBits>(fpout, false, skip_non_normal);  // printing positive number
    printNumber<T, true, expBits, mntBits>(fpout, true, skip_non_normal);   // printing negative number
}


// print commandline argument 
void usage()
{
    FILE *fpout = stdout;
    fprintf(fpout, "\n");
    fprintf(fpout, "Usage : \n");
    fprintf(fpout, "--epsilon               : print epsilon \n");
    fprintf(fpout, "--print-number-table    : print all reprsentable numbers in table with header (default)  \n");
    fprintf(fpout, "--no-skip <1,0>         : 1-> print all numbers, 0-> skip denorm, nan, inf, 0.0/-0.0 \n");
    fprintf(fpout, "--outfile <filename>    : output filename\n");
    fprintf(fpout, "--help                  : print usage\n\n");
}

inline void ArgumentRangeCheck(int currentIndex, int argCount)
{
    if (currentIndex+1 >= argCount)
    {
        usage();
        exit(1);
    }
}

// parse commandline argument   
void getFlags(FILE **fpout, int argc, char **argv, enum print_table_option &printOption, bool &skip_non_normal)
{
   char *filename=nullptr;
   
   // default  values
   *fpout = stdout;
   skip_non_normal = true; 

   printOption = print_table_option::PRINT_NUMBER;  // default: show number table
    
   if (argc > 1)
   {
        for (int i=1; i < argc; i++)
        {
            if (!strcmp(argv[i], "--epsilon"))
            {
                printOption = print_table_option::PRINT_EPSILON;
            }
            else if (!strcmp(argv[i], "--print-number-table"))
            {
                printOption = print_table_option::PRINT_NUMBER;
            }
            else if (!strcmp(argv[i], "--outfile"))
            {
                ArgumentRangeCheck(i, argc);
                filename = argv[i+1];
                i++;
            }
            else if (!strcmp(argv[i], "--no-skip"))
            {
                ArgumentRangeCheck(i, argc);
                int ns = atoi(argv[i+1]);
                skip_non_normal = ns ? true : false;
                i++;
            }
            else if (!strcmp(argv[i], "--help"))
            {
                usage();
                exit(1);
            }
            else 
            {
                fprintf(stderr, "Unknown option !!!\n");
                usage();
                exit(1);
            }
        }
//      update output 
        if (filename)
        {
            *fpout = fopen(filename, "w"); 
            assert(*fpout);
        }
    }
}

int main(int argc, char **argv)
{
    FILE *fpout;
    enum print_table_option printOption;
    bool skip_non_normal;

    // get command line flags
    getFlags(&fpout, argc, argv, printOption, skip_non_normal);
    
/*
 *  print different tables as asked  
 */
    switch(printOption)
    {
        case print_table_option::PRINT_EPSILON:
            printEpsilon<half>(fpout);  
            break;
        case print_table_option::PRINT_NUMBER:
            printNumberTable<half>(fpout, skip_non_normal);  
            break;
    }

    return 0;
}
