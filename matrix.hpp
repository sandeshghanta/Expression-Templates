#include <iostream>
#include <assert.h>
#include <complex>

#define ADDITION 1
#define SUBTRACTION -1
#define MULTIPLICATION 0

template<class T>
struct Precision {
    int rank = 0;
};

#define PRECISION(T,R)                        \
template<>                                    \
struct Precision< T > {                       \
    enum {                                    \
        rank = R                              \
    };                                        \
};

PRECISION(int, 100)
PRECISION(unsigned int, 200)
PRECISION(long, 300)
PRECISION(unsigned long, 400)
PRECISION(long long, 500)
PRECISION(unsigned long long, 600)
PRECISION(float, 700)
PRECISION(double, 800)
PRECISION(std::complex<int>, 900)
PRECISION(std::complex<unsigned int>, 1100)
PRECISION(std::complex<long>, 1200)
PRECISION(std::complex<unsigned long>, 1300)
PRECISION(std::complex<long long>, 1400)
PRECISION(std::complex<unsigned long long>, 1500)
PRECISION(std::complex<float>, 1600)
PRECISION(std::complex<double>, 1700)
PRECISION(std::complex<long double>, 1800)

template<class T>
struct Autopromote {
  typedef T datatype;
};

#define AUTOPROMOTE(T1,T2)                    \
template<>                                    \
struct Autopromote<T1> {                      \
  typedef T2 datatype;                        \
};

AUTOPROMOTE(bool, int)
AUTOPROMOTE(char, int)
AUTOPROMOTE(unsigned char, int)
AUTOPROMOTE(short int, int)
AUTOPROMOTE(short unsigned int, unsigned int)

template<class T1, class T2, int promoteToT1>
struct resolve_datatype {
    typedef T1 datatype;
};

template<class T1, class T2>
struct resolve_datatype<T1,T2,0> {
    typedef T2 datatype;
};

template<typename datatype, typename leftop, typename rightop>
inline datatype addition(leftop a, rightop b){
    return a + b;
}

template<typename datatype, typename leftop, typename rightop>
inline datatype subtraction(leftop a, rightop b){
    return a - b;
}

template < typename leftop, typename rightop>
    class matrix_expr {
        public:
            //resolving datatype of return expression based on datatype of left and right expressions
            typedef typename leftop::datatype left_datatype;
            typedef typename rightop::datatype right_datatype;
            typedef typename Autopromote <left_datatype>::datatype T1;
            typedef typename Autopromote <right_datatype>::datatype T2;
            enum {
                T1IsBetter = int(Precision<T1>::rank) > int(Precision<T2>::rank)
            };
            typedef typename resolve_datatype<T1,T2,T1IsBetter>::datatype datatype;
            datatype (*op) (T1,T2);
            mutable datatype* data;
            bool is_mult;
            mutable bool evaluated;
            const leftop lhs;
            const rightop rhs;
            std::size_t rows;
            std::size_t cols;
            matrix_expr(const leftop & lhs, int oper, const rightop & rhs): lhs(lhs), rhs(rhs) {
                evaluated = false;
                if (oper == 0){
                    assert(lhs.cols == rhs.rows);
                    is_mult = true;
                    rows = lhs.rows;
                    cols = rhs.cols;
                }
                else if (oper == -1){
                    assert(lhs.rows == rhs.rows && lhs.cols == rhs.cols);
                    op = subtraction<datatype,T1,T2>;
                    is_mult = false;
                    rows = lhs.rows;
                    cols = lhs.cols;
                }
                else if (oper == 1){
                    assert(lhs.rows == rhs.rows && lhs.cols == rhs.cols);
                    op = addition<datatype,T1,T2>;
                    is_mult = false;
                    rows = lhs.rows;
                    cols = lhs.cols;
                }
            }
            ~matrix_expr(){
                if (is_mult && evaluated){
                    delete [] data;
                }
            }
            inline datatype element_at(std::size_t i, std::size_t j){
                return get_element_at(i,j);
            }
            inline datatype element_at (std::size_t i, std::size_t j) const{
                return get_element_at(i,j);
            }
            datatype get_element_at(std::size_t row, std::size_t col) const{
                if (is_mult){
                    //storing is done only if the operation is multiplication
                    if (evaluated){
                        //If expr is already evaluated there is no need to reevaluate
                        return data[row*cols + col];
                    }
                    else{
                        /*If the expression is not evaluated we evaluate it and store it in temp*/
                        data = new datatype[rows*cols];     //creating the array
                        for (std::size_t i=0;i<lhs.rows;i++){
                            for (std::size_t j=0;j<rhs.cols;j++){
                                datatype val = 0;
                                for (std::size_t k=0;k<lhs.cols;k++){
                                    val += lhs.element_at(i,k) * rhs.element_at(k,j);
                                }
                                data[i*cols + j] = val;
                            }
                        }
                        evaluated = true;
                        return data[row*cols + col];
                    }
                }
                else{
                    return op(lhs.element_at(row,col),rhs.element_at(row,col));
                }
            }
    };

template < typename dtype >
    class matrix {
        public:
            typedef dtype datatype;
            datatype * matr_ptr;
            std::size_t rows;
            std::size_t cols;
            matrix(std::size_t x, std::size_t y) {
                rows = x;
                cols = y;
                matr_ptr = new datatype[rows * cols];
            };
            void print() const{
                std::cout << "[" << rows << "," << cols << "](";
                for (std::size_t i = 0; i < rows; i++) {
                    std::cout << "(";
                    for (std::size_t j = 0; j < cols; j++) {
                        if (j == cols - 1) {
                            std::cout << ( * this)(i, j);
                        } else {
                            std::cout << ( * this)(i, j) << ",";
                        }
                    }
                    if (i == rows - 1) {
                        std::cout << ")";
                    } else {
                        std::cout << "),";
                    }
                }
                std::cout << ")" << std::endl;
            }
            inline datatype & operator()(std::size_t i, std::size_t j) {
                assert(i >= 0 && i < rows && j >= 0 && j < cols);
                return matr_ptr[i * cols + j];
            }
            inline const datatype & operator()(std::size_t i, std::size_t j) const {
                assert(i >= 0 && i < rows && j >= 0 && j < cols);
                return matr_ptr[i * cols + j];
            }
            inline datatype element_at(std::size_t i, std::size_t j) {
               return (* this)(i,j);
            }
            inline const datatype element_at(std::size_t i, std::size_t j) const{
               return ( * this)(i,j);
            }

            template <typename datatype1 >
            matrix<datatype>& operator = (const matrix<datatype1>& rightop);

            template < typename leftdt, typename rightdt >
            matrix<datatype>& operator = (const matrix_expr<leftdt,rightdt>& rightop);

            template < typename datatype1 >
            matrix<datatype>& operator += (const matrix<datatype1>& rightop);

            template < typename leftdt, typename rightdt >
            matrix<datatype>& operator += (const matrix_expr<leftdt,rightdt>& rightop);

            template <typename datatype1 >
            matrix<datatype>& operator -= (const matrix<datatype1>& rightop);

            template < typename leftdt, typename rightdt >
            matrix<datatype>& operator -= (const matrix_expr<leftdt,rightdt>& rightop);

            template <typename datatype1 >
            matrix<datatype>& operator *= (const matrix<datatype1>& rightop);

            template < typename leftdt, typename rightdt >
            matrix<datatype>& operator *= (const matrix_expr<leftdt,rightdt>& rightop);
    };

template <typename datatype>
    template <typename datatype1>
    matrix<datatype> & matrix<datatype>::operator = (const matrix<datatype1> & rightop){
        assert((*this).rows == rightop.rows && (*this).cols == rightop.cols);
        for (std::size_t i = 0; i < ( * this).rows; i++) {
            for (std::size_t j = 0; j < ( * this).cols; j++) {
                    (* this) (i, j) = rightop.element_at(i,j);
                }
            }
        return *this;
    }

template < typename datatype >
    template < typename leftdt, typename rightdt >
    matrix < datatype > & matrix<datatype>::operator = (const matrix_expr < leftdt, rightdt > & rightop) {
        assert((*this).rows == rightop.rows && (*this).cols == rightop.cols);
        for (std::size_t i = 0; i < ( * this).rows; i++) {
            for (std::size_t j = 0; j < ( * this).cols; j++) {
                (* this) (i, j) = rightop.element_at(i,j);
            }
        }
        return *this;
    }

template < typename leftdt, typename rightdt >
    inline matrix_expr < leftdt, rightdt > operator + (const leftdt & leftop, const rightdt & rightop) {
        return matrix_expr < leftdt, rightdt > (leftop,ADDITION,rightop);
    }

template < typename leftdt, typename rightdt >
    inline matrix_expr < leftdt, rightdt > operator * (const leftdt & leftop, const rightdt & rightop) {
        return matrix_expr < leftdt, rightdt > (leftop,MULTIPLICATION,rightop);
    }

template < typename leftdt, typename rightdt >
    inline matrix_expr < leftdt, rightdt > operator - (const leftdt & leftop, const rightdt & rightop) {
        return matrix_expr < leftdt, rightdt > (leftop,SUBTRACTION,rightop);
    }

template < typename datatype >
    template < typename datatype1 >
        matrix < datatype > & matrix<datatype>::operator += (const matrix<datatype1> & rightop) {
            *this = *this + rightop;
            return *this;
        }

template < typename datatype >
    template < typename leftdt, typename rightdt >
        matrix < datatype > & matrix<datatype>::operator += (const matrix_expr<leftdt,rightdt> & rightop) {
            *this = *this + rightop;
            return *this;
        }

template < typename datatype >
    template < typename datatype1 >
        matrix < datatype > & matrix<datatype>::operator -= (const matrix<datatype1> & rightop) {
            *this = *this - rightop;
            return *this;
        }

template < typename datatype >
    template < typename leftdt, typename rightdt >
        matrix < datatype > & matrix<datatype>::operator -= (const matrix_expr<leftdt,rightdt> & rightop) {
            *this = *this - rightop;
            return *this;
        }

template < typename datatype >
    template < typename datatype1 >
        matrix < datatype > & matrix<datatype>::operator *= (const matrix<datatype1> & rightop) {
            *this = *this * rightop;
            return *this;
        }

template < typename datatype >
    template < typename leftdt, typename rightdt >
        matrix < datatype > & matrix<datatype>::operator *= (const matrix_expr<leftdt,rightdt> & rightop) {
            *this = *this * rightop;
            return *this;
        }