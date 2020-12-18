#include <iostream>
#include <mmintrin.h>

//#define DEBUG_MODE
#define BYTE_SIZE 8
#define WORD_SIZE 4
#define DWORD_SIZE 2


void print(__m64 value, const std::string& type)
{
    if (type == "byte")
    {
        for (int i = 0; i < BYTE_SIZE; ++i)
        {
            std::cout << int(value.m64_i8[i]) << " ";
        }
        std::cout << std::endl;
    }
    else if (type == "word")
    {
        for (int i = 0; i < WORD_SIZE; ++i)
        {
            std::cout << int(value.m64_i16[i]) << " ";
        }
        std::cout << std::endl;
    }
    else if (type == "dword")
    {
        for (int i = 0; i < DWORD_SIZE; ++i)
        {
            std::cout << int(value.m64_i32[i]) << " ";
        }
        std::cout << std::endl;
    }
}

void SolutionWithAsm() // todo add func arguments, comparing funcs before unpacking funcs to proper work with negative nums
{
    std::cout << "F[i] = (A[i] + B[i]) * (C[i] + D[i]), i = 1...8\n";
    int size = 8;
    auto *A = new __int8[size] {1, 1, 1, 1, 2, 2, 2, 2};
    auto *B = new __int8[size] {2, 2, 2, 2, 3, 3, 3, 3};
    auto *C = new __int8[size] {3, 3, 3, 3, 4, 4, 4, 4};
    auto *D = new __int16[size] {4, 4, 4, 4, 5, 5, 5, 5};

    __m64 vectorA, vectorB, vectorC, ABSum;
    for (int i = 0; i < size; ++i)
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

#ifdef DEBUG_MODE
    std::cout << "A + B\n";
    print(ABSum, "byte");
#endif

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

#ifdef DEBUG_MODE
    std::cout << "The high half of C\n";
    print(highHalfC, "word");
    std::cout << "The low half of C\n";
    print(lowHalfC, "word");
#endif

    __m64 vectorD1, vectorD2, CDSum1, CDSum2;
    for (int i = 0; i < size / 2; ++i)
        vectorD1.m64_i16[i] = D[i];
    for (int i = size / 2, j = 0; i < size; ++i, ++j)
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

#ifdef DEBUG_MODE
    std::cout << "The first half of (C + D)\n";
    print(CDSum1, "word");
    std::cout << "The second half of (C + D)\n";
    print(CDSum2, "word");
#endif

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

#ifdef DEBUG_MODE
    std::cout << "The first half of (A + B)\n";
    print(ABHighHalf, "word");
    std::cout << "The second half of (A + B)\n";
    print(ABLowHalf, "word");
#endif

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

__m64* SolutionWithIntrinsicsWithAutomation(const __int8 *A, const __int8 *B, const __int8 *C, __int16 *D)
{
    __m64 zeroVector = _mm_setzero_si64();
    std::cout << "F[i] = (A[i] + B[i]) * (C[i] + D[i]), i = 1...8\n";

    __m64 AB_sum = _m_paddsb(*((__m64*)A), *((__m64*)B)); // A[i] + B[i]
#ifdef DEBUG_MODE
    std::cout << "A + B\n";
    print(AB_sum, "byte");
#endif

    __m64 check = _m_pcmpgtb(zeroVector, *((__m64*)C)); // comparing with zero

    __m64 CWord[2] = { // 0 - low, 1 - high
            _m_punpcklbw (*((__m64*)C), check), // C in _int8 to C2 in _int16
            _m_punpckhbw(*((__m64*)C), check), // C in _int8 to C1 in _int16
    };

#ifdef DEBUG_MODE
    std::cout << "The first half of C\n";
    print(CWord[0], "word");
    std::cout << "The second half of C\n";
    print(CWord[1], "word");
#endif

    __m64 CDSum[2];
    for (int i = 0; i < 2; ++i)
    {
        CDSum[i] = _m_paddsw(CWord[i], ((__m64*)D)[i]);
    }

#ifdef DEBUG_MODE
    std::cout << "The first half of (C + D)\n";
    print(CDSum[0], "word");
    std::cout << "The second half of (C + D)\n";
    print(CDSum[1], "word");
#endif

    check = _m_pcmpgtb(zeroVector, AB_sum); // comparing with zero

    __m64 AB[2] = { // 0 - low, 1 - high
            _m_punpcklbw(AB_sum, check), // (A + B) in _int8 to (A + B)2 in _int16
            _m_punpckhbw(AB_sum, check), // (A + B) in _int8 to (A + B)1 in _int16
    };

#ifdef DEBUG_MODE
    std::cout << "The first half of (A + B)\n";
    print(AB[0], "word");
    std::cout << "The second half of (A + B)\n";
    print(AB[1], "word");
#endif

    // multiplication
    __m64 mulRes[4];
    for (int i = 0, j = 0; i < 2; ++i)
    {
        mulRes[j++] = _m_pmulhw(AB[i], CDSum[i]); // 0, 2 - high
        mulRes[j++] = _m_pmullw (AB[i], CDSum[i]); // 1, 3 - low
    }

    // unpacking results
    auto *F = new __m64[4];
    for (int i = 0, j = 0, l = 1, h = 0; i < 2; ++i, l += 2, h += 2)
    {
        F[j++] = _m_punpckhwd(mulRes[l], mulRes[h]);
        F[j++] = _m_punpcklwd(mulRes[l], mulRes[h]);
    }

    return F;
}

__m64* SolutionWithIntrinsics(const __int8 *A, const __int8 *B, const __int8 *C, __int16 *D)
{
    std::cout << "F[i] = (A[i] + B[i]) * (C[i] + D[i]), i = 1...8\n";

    __m64 zeroVector = _mm_setzero_si64();
    __m64 AB_sum = _m_paddsb(*((__m64*)A), *((__m64*)B)); // A[i] + B[i]
#ifdef DEBUG_MODE
    std::cout << "A + B\n";
    print(AB_sum, "byte");
#endif

    __m64 check = _m_pcmpgtb(zeroVector, *((__m64*)C)); // comparing with zero

    __m64 CHighHalf = _m_punpckhbw(*((__m64*)C), check); // C in _int8 to C1 in _int16
    __m64 CLowHalf = _m_punpcklbw (*((__m64*)C), check); // C in _int8 to C2 in _int16

#ifdef DEBUG_MODE
    std::cout << "The first half of C\n";
    print(CHighHalf, "word");
    std::cout << "The second half of C\n";
    print(CLowHalf, "word");
#endif

    __m64 CDSum1 = _m_paddsw(CLowHalf, ((__m64*)D)[0]); // C1 + D1
    __m64 CDSum2 = _m_paddsw(CHighHalf, ((__m64*)D)[1]); // C2 + D2

#ifdef DEBUG_MODE
    std::cout << "The first half of (C + D)\n";
    print(CDSum1, "word");
    std::cout << "The second half of (C + D)\n";
    print(CDSum2, "word");
#endif

    check = _m_pcmpgtb(zeroVector, AB_sum); // comparing with zero

    __m64 ABHighHalf = _m_punpckhbw(AB_sum, check); // (A + B) in _int8 to (A + B)1 in _int16
    __m64 ABLowHalf = _m_punpcklbw (AB_sum, check); // (A + B) in _int8 to (A + B)2 in _int16

#ifdef DEBUG_MODE
    std::cout << "The first half of (A + B)\n";
    print(ABHighHalf, "word");
    std::cout << "The second half of (A + B)\n";
    print(ABLowHalf, "word");
#endif

    // multiplication
    __m64 highHalfRes1 = _m_pmulhw(ABLowHalf, CDSum1); // the first half of (ABSum1 * CDSum1)
    __m64 lowHalfRes1 = _m_pmullw (ABLowHalf, CDSum1); // the second half of (ABSum1 * CDSum1)
    __m64 highHalfRes2 = _m_pmulhw(ABHighHalf, CDSum2); // the first half of (ABSum2 * CDSum2)
    __m64 lowHalfRes2 = _m_pmullw (ABHighHalf, CDSum2); // the second half of (ABSum2 * CDSum2)

    // unpacking results
    auto *F = new __m64 [4];
    F[0] = _m_punpckhwd(lowHalfRes1, highHalfRes1);
    F[1] = _m_punpcklwd(lowHalfRes1, highHalfRes1);
    F[2] = _m_punpckhwd(lowHalfRes2, highHalfRes2);
    F[3] = _m_punpcklwd(lowHalfRes2, highHalfRes2);

    return F;
}

int main()
{
    auto *A = new __int8[BYTE_SIZE * 1] {-1, -1, -1, -1, 2, 2, 2, 2};
    auto *B = new __int8[BYTE_SIZE * 1] {-2, -2, 2, 2, 3, 3, 3, 3};
    auto *C = new __int8[BYTE_SIZE * 1] {3, 3, 3, 3, 4, 4, 4, 4};
    auto *D = new __int16[WORD_SIZE * 2] {4, 4, 4, 4, 5, -5, 5, 5};

    __m64 *F = SolutionWithIntrinsicsWithAutomation(A, B, C, D);
//    __m64 *F = SolutionWithIntrinsics(A, B, C, D);

    std::cout << "Answers (F[i]):\n";
    for (int i = 0; i < 4; ++i) { print(F[i], "dword"); }

//    SolutionWithAsm();

    return 0;
}
