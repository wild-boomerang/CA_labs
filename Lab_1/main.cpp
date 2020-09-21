#include <iostream>
#include <mmintrin.h>
#include <xmmintrin.h>

float inner2(float* x, float* y, int n)
{
    __m128 *xx, *yy;
    __m128 p, s;
    xx = (__m128*)x;
    yy = (__m128*)y;
    s = _mm_setzero_ps();
    for (int i=0; i<n/4; ++i)
    {
        p = _mm_mul_ps(xx[i],yy[i]);
        s = _mm_add_ps(s,p);
    }
    p = _mm_movehl_ps(p,s);
    s = _mm_add_ps(s,p);
    p = _mm_shuffle_ps(s,s,1);
    s = _mm_add_ss(s,p);
    float sum;
    _mm_store_ss(&sum,s);
    return sum;
}

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

//    __asm
//    {
//        movq mm0, AB_sum
//        PUNPCKHBW mm0, CHighHalf  // high half (right half)
//        movq CHighHalf, mm0
//        movq mm0, AB_sum
//        PUNPCKLBW mm0, CLowHalf // low half (left half)
//        movq CLowHalf, mm0
//    }

// F[i] = (A[i] + B[i]) * (C[i] + D[i]), i = 1...8
void Solution()
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
//    std::cout << sizeof(_int16);

    Solution();

//    float arr[4] = {5.5, 5.5, 5.5, 5.5};
//    std::cout << inner2(arr, arr, 4);

    return 0;
}
