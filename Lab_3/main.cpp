#include <omp.h>
#include <iostream>
#include <ctime>
#include <vector>


[[maybe_unused]] void print_matrix(int **matrix, int strings, int columns)
{
    for (int i = 0; i < strings; ++i) {
        for (int j = 0; j < columns; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

[[maybe_unused]] void print_vector(int *vector, int size)
{
    for (int j = 0; j < size; ++j) {
        std::cout << vector[j] << "\n";
    }
}

void print_results(bool is_single, int strings, int columns, unsigned int duration)
{
    std::cout << ((is_single) ? "One thread" : "Multiple threads") << "\n"
              << "Num of strings: " << strings << "\n"
              << "Num of columns: " << columns << "\n"
              << "Duration: " << duration << "\n"
              << "\n";
}

int* multiply(int **matrix, const int *vector, int strings, int columns, bool is_single)
{
    int* res = new int[strings];

    for (int i = 0; i < strings; ++i) {
        res[i] = 0;
    }

    unsigned int start_time = clock();

    int i, j;

#pragma omp parallel for shared(matrix, vector, res, strings, columns) private(i, j) if(!is_single) default(none)
    for (i = 0; i < strings; ++i) {
        for (j = 0; j < columns; ++j) {
            res[i] += matrix[i][j] * vector[j];
        }
    }
    unsigned int end_time = clock();

    print_results(is_single, strings, columns, end_time - start_time);

    return res;
}

bool check(const int *res1, const int *res2, int n)
{
    for (int i = 0; i < n; ++i) {
        if (res1[i] != res2[i]) {
            return false;
        }
    }
    return true;
}

void test(int n)
{
    srand(time(nullptr));

    int strings = n, columns = n;
    int** matrix, *vector;

//    std::cin >> strings >> columns;

    matrix = new int* [strings];
    for (int i = 0; i < strings; ++i) {
        matrix[i] = new int[columns];
    }

    for (int i = 0; i < strings; ++i) {
        for (int j = 0; j < columns; ++j) {
            matrix[i][j] = rand();
//            std::cin >> matrix[i][j];
        }
    }

//    print_matrix(matrix, strings, columns);

    vector = new int[columns];
    for (int i = 0; i < columns; ++i) {
        vector[i] = rand();
//        std::cin >> vector[i];
    }

//    print_vector(vector, columns);

    int *single_res = multiply(matrix, vector, strings, columns, true);
//    print_vector(single_res, strings);
    int *multiple_res = multiply(matrix, vector, strings, columns, false);
//    print_vector(multiple_res, strings);

    std::cout << "n = " << n << ": ";
    if (check(single_res, multiple_res, strings)) {
        std::cout << "All is clear\n\n";
    } else {
        std::cerr << "\nError\n";
    }

    delete single_res;
    delete multiple_res;
    delete vector;
    for (int i = 0; i < strings; ++i) {
        delete [] matrix[i];
    }
}

int main()
{
    std::cout << "Available number of threads: " << omp_get_max_threads() << "\n\n";

    int multiplier = 100;
    std::vector<int> size { multiplier * 1, multiplier * 10, multiplier * 100 };
    for (auto i : size) {
        test(i);
    }

    return 0;
}