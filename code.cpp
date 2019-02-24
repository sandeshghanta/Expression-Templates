#include <matrix.hpp>
#include <iostream>
#include <chrono>
#include <complex>
using namespace std::chrono;

int main () {
    srand(52);
    int rows = 2;
    int cols = 2;
    matrix<int>m(rows,cols);
    matrix<double>n(rows,cols);
    matrix<double>s(rows,cols);
    matrix<std::complex<int> > cplex1(rows,cols);
    matrix<std::complex<int> > cplex2(rows,cols);
    matrix<std::complex<int> > cplex_res(rows,cols);
    matrix<int>t(10,10);
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            m(i,j) = rand()%100;
            n(i,j) = rand()%100 + 0.1 * (rand()%10) + 0.01 * (rand()%10);
            s(i,j) = rand()%100 + 0.1 * (rand()%10) + 0.01 * (rand()%10);
            std::complex<int> cp1(rand()%100,rand()%100);
            cplex1(i,j) = cp1;
            std::complex<int> cp2(rand()%100,rand()%100);
            cplex2(i,j) = cp2;
        }
    }
    m.print();
    n.print();
    s.print();
    //res = m<int>(100,100) + n<double>(100,100)
    auto start = high_resolution_clock::now();
    matrix<double>res(rows,cols);
    res = m + n;
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start); 
    std::cout << "res = m + n; time: " << duration.count() / 1000000.0 << std::endl;
    // res.print();  //to print the result uncomment the line
    
    //res = m<int>(100,100) + n<double>(100,100) * s<double>(100,100)
    start = high_resolution_clock::now();
    res = m + n*s;
    stop = high_resolution_clock::now(); 
    duration = duration_cast<microseconds>(stop - start); 
    std::cout << "res = m + n*s; time: " << duration.count() / 1000000.0 << std::endl;
    // res.print();   //to print the result uncomment the line

    //res = m<int>(100,100) + n<double>(100,100) * s<double>(100,100) + m<int>(100,100) * n<double>(100,100) + m<int>(100,100)
    start = high_resolution_clock::now();
    res = m + n*s + m*n + m;
    stop = high_resolution_clock::now(); 
    duration = duration_cast<microseconds>(stop - start); 
    std::cout << "res = m + n*s + m*n; time: " << duration.count() / 1000000.0 << std::endl;
    // res.print();   //to print the result uncomment the line

    //res += m<int>(100,100)*n(100,100)
    start = high_resolution_clock::now();
    res += m*n;
    stop = high_resolution_clock::now(); 
    duration = duration_cast<microseconds>(stop - start); 
    std::cout << "res += m*s; time: " << duration.count() / 1000000.0 << std::endl;
    // res.print();     //to print the result uncomment the line

    //res *= m<int>(100,100) + n(100,100)
    start = high_resolution_clock::now();
    res *= m*n;
    stop = high_resolution_clock::now(); 
    duration = duration_cast<microseconds>(stop - start); 
    std::cout << "res *= m+s; time: " << duration.count() / 1000000.0 << std::endl;
    // res.print();     //to print the result uncomment the line

    //cplex_res = cplex1<complex<int>>(100,100) + cplex2<complex<int>>(100,100)
    start = high_resolution_clock::now();
    cplex_res = cplex1 + cplex2;
    stop = high_resolution_clock::now(); 
    duration = duration_cast<microseconds>(stop - start); 
    std::cout << "cplex_res = cplex1 + cplex2; time: " << duration.count() / 1000000.0 << std::endl;
    // cplex_res.print();     //to print the result uncomment the line

    //cples_res = cplex1<complex<int>>(100,100) + m<int>(100,100)
    start = high_resolution_clock::now();
    cplex_res = cplex1 + m;
    stop = high_resolution_clock::now(); 
    duration = duration_cast<microseconds>(stop - start); 
    std::cout << "cplex_res = cplex1 + m; time: " << duration.count() / 1000000.0 << std::endl;
    // cplex_res.print();       //to print the result uncomment the line


    // t = m * res;          //uncomment this line to get error since the matrices are incompatible
    return 0;
}