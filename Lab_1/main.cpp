#include <iostream>
#include <mmintrin.h>
//#include <xmmintrin.h>

//float inner2(float* x, float* y, int n)
//{
//    __m128 *xx, *yy;
//    __m128 p, s;
//    xx = (__m128*)x;
//    yy = (__m128*)y;
//    s = _mm_setzero_ps();
//    for (int i=0; i<n/4; ++i)
//    {
//        p = _mm_mul_ps(xx[i],yy[i]);
//        s = _mm_add_ps(s,p);
//    }
//    p = _mm_movehl_ps(p,s);
//    s = _mm_add_ps(s,p);
//    p = _mm_shuffle_ps(s,s,1);
//    s = _mm_add_ss(s,p);
//    float sum;
//    _mm_store_ss(&sum,s);
//    return sum;
//}

// F[i] = (A[i] + B[i]) * (C[i] + D[i]), i = 1...8

void print(__m64 value, const std::string& type)
{
    if (type == "byte")
    {
        for (int i = 0; i < 8; ++i)
            std::cout << int(value.m64_i8[i]) << " ";
        std::cout << std::endl;
    }
    else if (type == "word")
    {
        for (int i = 0; i < 4; ++i)
            std::cout << int(value.m64_i16[i]) << " ";
        std::cout << std::endl;
    }
    else if (type == "dword")
    {
        for (int i = 0; i < 2; ++i)
            std::cout << int(value.m64_i32[i]) << " ";
        std::cout << std::endl;
    }
}

void SolutionWithAsm()
{
    std::cout << "F[i] = (A[i] + B[i]) * (C[i] + D[i]), i = 1...8\n";
    int size = 8;
    auto *A = new __int8[size] {1, 1, 1, 1, 2, 2, 2, 2};
    auto *B = new __int8[size] {2, 2, 2, 2, 3, 3, 3, 3};
    auto *C = new __int8[size] {3, 3, 3, 3, 4, 4, 4, 4};
    auto *D = new __int16[size] {4, 4, 4, 4, 5, 5, 5, 5};

    __m64 vectorA, vectorB, vectorC, ABSum;
    for (int i = 0; i < 8; ++i)
    {
        vectorA.m64_i8[i] = A[i];
        vectorB.m64_i8[i] = B[i];
        vectorC.m64_i8[i] = C[i];
    }
    __asm
    {
        movq mm0, vectorA
        paddsb mm0, vectorB // A + B
        movq ABSum, mm0
    }

    std::cout << "A + B\n";
    print(ABSum, "byte");

    __m64 zeroVector, highHalfC, lowHalfC;
    zeroVector.m64_i64 = 0;
    __asm
    {
        movq mm0, vectorC
        punpckhbw mm0, zeroVector // highHalfC
        movq highHalfC, mm0
        movq mm0, vectorC
        punpcklbw mm0, zeroVector // lowHalfC
        movq lowHalfC, mm0
    }

    std::cout << "The high half of C\n";
    print(highHalfC, "word");
    std::cout << "The low half of C\n";
    print(lowHalfC, "word");

    __m64 vectorD1, vectorD2, CDSum1, CDSum2;
    for (int i = 0; i < 4; ++i)
        vectorD1.m64_i16[i] = D[i];
    for (int i = 4, j = 0; i < 8; ++i, ++j)
        vectorD2.m64_i16[j] = D[i];
    __asm
    {
        movq mm0, lowHalfC
        paddsw mm0, vectorD1 // C1 + D1
        movq CDSum1, mm0

        movq mm0, highHalfC
        paddsw mm0, vectorD2 // C2 + D2
        movq CDSum2, mm0
    }

    std::cout << "The first half of (C + D)\n";
    print(CDSum1, "word");
    std::cout << "The second half of (C + D)\n";
    print(CDSum2, "word");

    __m64 ABHighHalf, ABLowHalf;
    __asm
    {
        movq mm0, ABSum
        punpckhbw mm0, zeroVector // ABHighHalf
        movq ABHighHalf, mm0
        movq mm0, ABSum
        punpcklbw mm0, zeroVector // ABLowHalf
        movq ABLowHalf, mm0
    }

    std::cout << "The first half of (A + B)\n";
    print(ABHighHalf, "word");
    std::cout << "The second half of (A + B)\n";
    print(ABLowHalf, "word");

    __m64 highHalfRes1, lowHalfRes1, highHalfRes2, lowHalfRes2;
    __asm
    {
        movq mm0, ABLowHalf
        pmulhw mm0, CDSum1
        movq highHalfRes1, mm0

        movq mm0, ABLowHalf
        pmullw mm0, CDSum1
        movq lowHalfRes1, mm0

        movq mm0, ABHighHalf
        pmulhw mm0, CDSum2
        movq highHalfRes2, mm0

        movq mm0, ABHighHalf
        pmullw mm0, CDSum2
        movq lowHalfRes2, mm0
    }

    __m64 FPart1, FPart2, FPart3, FPart4;
    __asm
    {
        movq mm0, lowHalfRes1
        punpckhwd mm0, highHalfRes1 // FPart1
        movq FPart1, mm0

        movq mm0, lowHalfRes1
        punpcklwd mm0, highHalfRes1 // FPart2
        movq FPart2, mm0

        movq mm0, lowHalfRes2
        punpckhwd mm0, highHalfRes2 // FPart3
        movq FPart3, mm0

        movq mm0, lowHalfRes2
        punpcklwd mm0, highHalfRes2 // FPart4
        movq FPart4, mm0
    }

    std::cout << "Answers (F[i]):\n";
    print(FPart1, "dword");
    print(FPart2, "dword");
    print(FPart3, "dword");
    print(FPart4, "dword");
}

void SolutionWithIntrinsics()
{
    std::cout << "F[i] = (A[i] + B[i]) * (C[i] + D[i]), i = 1...8\n";
    int size = 8;
    auto *A = new __int8[size] {1, 1, 1, 1, 2, 2, 2, 2};
    auto *B = new __int8[size] {2, 2, 2, 2, 3, 3, 3, 3};
    auto *C = new __int8[size] {3, 3, 3, 3, 4, 4, 4, 4};
    auto *D = new __int16[size] {4, 4, 4, 4, 5, 5, 5, 5};

    __m64 AB_sum = _m_paddsb(*((__m64*)A), *((__m64*)B)); // A[i] + B[i]
    std::cout << "A + B\n";
    print(AB_sum, "byte");

    __m64 CLowHalf, CHighHalf;
    CLowHalf.m64_i64 = 0;
    CHighHalf.m64_i64 = 0;

    CHighHalf = _m_punpckhbw(*((__m64*)C), CHighHalf); // C in _int8 to C1 in _int16
    CLowHalf = _m_punpcklbw (*((__m64*)C), CLowHalf); // C in _int8 to C2 in _int16
    std::cout << "The first half of C\n";
    print(CHighHalf, "word");
    std::cout << "The second half of C\n";
    print(CLowHalf, "word");

    __m64 CDSum1 = _m_paddsw(CLowHalf, ((__m64*)D)[0]); // C1 + D1
    __m64 CDSum2 = _m_paddsw(CHighHalf, ((__m64*)D)[1]); // C2 + D2
    std::cout << "The first half of (C + D)\n";
    print(CDSum1, "word");
    std::cout << "The second half of (C + D)\n";
    print(CDSum2, "word");

    __m64 ABLowHalf, ABHighHalf;
    ABLowHalf.m64_i64 = 0;
    ABHighHalf.m64_i64 = 0;
    ABHighHalf = _m_punpckhbw(AB_sum, ABHighHalf); // (A + B) in _int8 to (A + B)1 in _int16
    ABLowHalf = _m_punpcklbw (AB_sum, ABLowHalf); // (A + B) in _int8 to (A + B)2 in _int16
    std::cout << "The first half of (A + B)\n";
    print(ABHighHalf, "word");
    std::cout << "The second half of (A + B)\n";
    print(ABLowHalf, "word");

    // multiplication
    __m64 highHalfRes1 = _m_pmulhw(ABLowHalf, CDSum1); // the first half of (ABSum1 * CDSum1)
    __m64 lowHalfRes1 = _m_pmullw (ABLowHalf, CDSum1); // the second half of (ABSum1 * CDSum1)
    __m64 highHalfRes2 = _m_pmulhw(ABHighHalf, CDSum2); // the first half of (ABSum2 * CDSum2)
    __m64 lowHalfRes2 = _m_pmullw (ABHighHalf, CDSum2); // the second half of (ABSum2 * CDSum2)

    // unpacking results
    __m64 FPart1 = _m_punpckhwd(lowHalfRes1, highHalfRes1);
    __m64 FPart2 = _m_punpcklwd(lowHalfRes1, highHalfRes1);
    __m64 FPart3 = _m_punpckhwd(lowHalfRes2, highHalfRes2);
    __m64 FPart4 = _m_punpcklwd(lowHalfRes2, highHalfRes2);
    std::cout << "Answers (F[i]):\n";
    print(FPart1, "dword");
    print(FPart2, "dword");
    print(FPart3, "dword");
    print(FPart4, "dword");
}

int main()
{
    SolutionWithIntrinsics();
//    SolutionWithAsm();

//    float arr[4] = {5.5, 5.5, 5.5, 5.5};
//    std::cout << inner2(arr, arr, 4);

    return 0;
}
