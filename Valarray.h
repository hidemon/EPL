// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class 
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h
#include <complex>
#include <functional>
//#include <vector>
#include "Vector.h"

using std::complex;
//using std::vector; // during development and testing
using epl::vector; // after submission
using namespace std::rel_ops;

namespace epl {

    /***** Types *****/
    /* Simple Type */
    template <typename T> struct SRank { static constexpr int value = 0; };
    template <> struct SRank <int> { static constexpr int value = 1; };
    template <> struct SRank <float> { static constexpr int value = 2; };
    template <> struct SRank <double> { static constexpr int value = 3; };
    template <typename T> struct SRank <std::complex<T>> { static constexpr int value = SRank<T>::value; };

    template <int T> struct SType;
    template <> struct SType<1> { using type = int; };
    template <> struct SType<2> { using type = float; };
    template <> struct SType<3> { using type = double; };
    /* Complex Type */
    template <typename T> struct is_complex : public std::false_type {};
    template <typename T> struct is_complex <std::complex<T>> : public std::true_type {};

    template <bool, typename T> struct c_type;
    template <typename T> struct c_type<true, T> { using type = std::complex<T>; };
    template <typename T> struct c_type<false, T> { using type = T; };

    /* Choose the type */
    template <typename T1, typename T2>
    struct choose_type {
        static constexpr int rank1 = SRank<T1>::value;
        static constexpr int rank2 = SRank<T2>::value;
        static constexpr int max_rank = rank1 > rank2 ? rank1 : rank2;
        using s_type = typename SType<max_rank>::type;
        
        static constexpr bool t1_is_complex = is_complex<T1>::value;
        static constexpr bool t2_is_complex = is_complex<T2>::value;
        static constexpr bool res_is_complex = t1_is_complex || t2_is_complex;
        using type = typename c_type<res_is_complex, s_type>::type;
        
    };
//    template <typename T1, typename T2>
//    using ChooseType = typename choose_type<T1, T2>::type;
   
//    /* Check if it is a scalar */
//    template <typename T> struct isScalar { static constexpr bool val = 0; };
//    template<> struct isScalar<int> { static constexpr bool val = 1; using type = int; };
//    template<> struct isScalar<float> { static constexpr bool val = 1; using type = float; };
//    template<> struct isScalar<double> { static constexpr bool val = 1; using type = double; };
//    template<typename T> struct isScalar<std::complex<T>> { static constexpr bool val = 1; using type = std::complex<T>; };
    
    template <typename T> struct get_value_type{ using type = typename T::valType; };
    template <typename T> struct get_value_type<vector<T>>{ using type = T; };
    
    template <bool p, typename T> using EnableIf = typename std::enable_if<p, T>::type;
    template <typename T> using EnableIfScalar = EnableIf<SRank<T>::value != 0, T>;

    
    template <typename T> struct is_vector :public std::false_type {};
    template <typename T> struct is_vector<vector<T>> :public std::true_type {};
    template <typename T> struct choose_ref{ using type = T; };
    template <typename T> struct choose_ref<vector<T>> { using type = vector<T> const&; };
    template <typename T> using ChooseRef = typename choose_ref<T>::type;
    
    //   handle complex<int>
    template <typename T, typename ReturnType>
    ReturnType cast_type(T const& operand) {
        return operand;
    }
    
    template <typename T, typename ReturnType>
    ReturnType cast_type(std::complex<int> const& operand) {
        return ReturnType(operand.real(), operand.imag());
    }
    
    /***** Function Objects for proxies *****/
    /* Sqrt */
    template <typename T>
    struct mySqrt{
        using ReturnType = typename choose_type<double, T>::type;
        ReturnType operator()(T const& val) const { return std::sqrt(cast_type<T, ReturnType>(val)); }
    };
    
    //valarray
    template <typename T> class Wrapper;
    template <typename T> using valarray = Wrapper<vector<T>>;
    
    /********* iterator *********/
    template <typename T>
    class const_iterator: public std::iterator<std::random_access_iterator_tag, typename get_value_type<T>::type> {
    private:
        const T* p_vector;
        uint64_t index;
        using valType = typename T::valType;
        using Same = const_iterator<T>;
    public:
        const_iterator(T const& p_vector, uint64_t const index) {
            this->p_vector = &p_vector;
            this->index = index;
        }
        
        const valType operator*() const {
            return (*p_vector)[index];
        }
        
        Same& operator= (Same const& rhs) {
            p_vector = rhs.p_vector;
            index = rhs.index;
            return *this;
        }
        
        Same& operator+ (const uint64_t k) const {
            Same res {*this};
            res.index += k;
            return res;
        }
        
        friend Same operator+(const uint64_t k, Same const& it) {
            return it + k;
        }
        
        int64_t operator- (Same const& rhs) const { return ((int64_t)index) - ((int64_t)rhs.index); }
        Same& operator-(const uint64_t k) const { return operator+(-k); }
        Same& operator+=(const uint64_t k)  { index += k; return *this; }
        Same& operator-=(const uint64_t k)  { index -= k; return *this; }
        Same& operator++(void)  { return operator+=(1); }
        Same operator++(int) { Same res{*this}; operator++(); return res; }
        Same& operator--(void)  { return operator-=(1); }
        Same operator--(int) { Same res{*this}; operator--(); return res; }
        
        bool operator==(Same const& rhs) const {
            return p_vector == rhs.p_vector && index == rhs.index;
        }
        bool operator<(Same const& rhs) const {
            return p_vector == rhs.p_vector && index < rhs.index;
        }
        valType& operator[] (const uint64_t k) const { return *(*this + k); }
        valType* operator->() const { return &(p_vector[index]); }
        
    };

    
    /* Scalar type */
    template <typename T>
    class scalar{
    private:
        T val;
    public:
        using valType = T;
        scalar(void): val(T()) {}
        scalar(T const& value): val(value) {}
        T operator[] (uint64_t const k) const { return val; }
        uint64_t size(void) const { return UINT64_MAX; }
        const_iterator<scalar<T>> begin(void) const { return const_iterator<scalar<T>>(*this, 0); }
        const_iterator<scalar<T>> end(void) const { return const_iterator<scalar<T>>(*this, 1); }
    };
    

    
    /********Proxy**********/
    template <typename T1, typename T2, typename OP>
    class Proxy {
    private:
        using LeftType = ChooseRef<T1>;
        using RightType = ChooseRef<T2>;
        using LeftValType = typename get_value_type<T1>::type;
        using RightValType = typename get_value_type<T2>::type;
        
        LeftType left;
        RightType right;
        OP op;
        
    public:
        Proxy(T1 const& lhs, T2 const& rhs, OP binaryOp) : left(lhs), right(rhs), op(binaryOp) {}
        using valType = typename choose_type<LeftValType, RightValType>::type;
        uint64_t size() const { return std::min(left.size(), right.size()); }
        valType operator[] (uint64_t const k) const {
            valType l = cast_type<LeftValType, valType>(left[k]);
            valType r = cast_type<RightValType, valType>(right[k]);
            return op(l, r);
        }
        
        const_iterator<Proxy<T1, T2, OP>> begin() const { return const_iterator<Proxy<T1, T2, OP>>(*this, 0); }
        const_iterator<Proxy<T1, T2, OP>> end() const { return const_iterator<Proxy<T1, T2, OP>>(*this, size()); }
    };
    
    
    template <typename Type, typename OP>
    struct UnaryOP{
    private:
        OP op;
    public:
        UnaryOP(const OP op_t) : op(op_t) {}
        Type operator()(Type const& val, Type const&) const {
            return std::plus<Type>{}(op(val), 0);
        }
    };
    
    template <typename T, typename OP, typename ValType = typename get_value_type<T>::type>
    struct UnaryProxy: public Proxy<T, scalar<int>, UnaryOP<ValType, OP>>{
        UnaryProxy(T const& inner, OP const op): Proxy<T, scalar<int>, UnaryOP<ValType, OP>>(inner, scalar<int>(0), UnaryOP<ValType, OP>(op)) {}
    };
    
    template <typename T>
    class Wrapper : public T {
    public:
        using T::T;
        using valType = typename get_value_type<T>::type;
        using Same = Wrapper<T>;
        Wrapper() : T() {}
        Wrapper(T const& rhs) : T(rhs) {}
        
        template <typename V>
        Wrapper(Wrapper<V> const& rhs) : T() {
            uint64_t size = rhs.end() - rhs.begin();
            for (uint64_t i = 0; i < size; i++)
                this->push_back(cast_type<typename get_value_type<V>::type, valType>(rhs[i]));
        }
        
        template <typename V>
        Same& operator=(Wrapper<V> const& rhs) {
            if ((void*)this != (void*)&rhs) {
                uint64_t size = rhs.end() - rhs.begin();
                for (uint64_t i = 0; i < size; i++)
                    (*this)[i] = cast_type<typename get_value_type<V>::type, valType>(rhs[i]);
            }
            return *this;
        }
        
        Same& operator=(Same const& rhs) {
            if ((void*)this != (void*)&rhs) {
                uint64_t size = rhs.end() - rhs.begin();
                for (uint64_t i = 0; i < size; i++)
                    (*this)[i] = cast_type<typename get_value_type<T>::type, valType>(rhs[i]);
            }
            return *this;
        }
        
        template <typename S, typename E = EnableIfScalar<S>>
        Same& operator=(S const& rhs) {
            return operator=(Wrapper<scalar<S>>(scalar<S>(rhs)));
        }
        
        template <typename OP>
        Wrapper<UnaryProxy<T, OP>> apply(OP const op) const {
            return Wrapper<UnaryProxy<T, OP>>( UnaryProxy<T, OP>(*this, op) );
        }
        template <typename OP>
        Wrapper<UnaryProxy<T, OP>> apply(OP const op) {
            return Wrapper<UnaryProxy<T, OP>>( UnaryProxy<T, OP>(*this, op) );
        }
        Wrapper<UnaryProxy<T, std::negate<valType>>> operator-(void) const {
            return apply(std::negate<valType>());
        }
        Wrapper<UnaryProxy<T, mySqrt<valType>>> sqrt() const {
            return apply(mySqrt<valType>());
        }
        
        
        template <typename Fun>
        valType accumulate(Fun const fun) const{
            if(this->size() == 0) return valType();
            else if(this->size() == 1) return (*this)[0];
            else{
                valType res = fun((*this)[0], (*this)[1]);
                for(uint64_t i=2; i<this->size(); i++)
                    res = fun(res, (*this)[i]);
                return res;
            }
        }
        
        template <typename Fun>
        valType accumulate(Fun const fun) {
            if(this->size() == 0) return valType();
            else if(this->size() == 1) return (*this)[0];
            else{
                valType res = fun((*this)[0], (*this)[1]);
                for(uint64_t i=2; i<this->size(); i++)
                    res = fun(res, (*this)[i]);
                return res;
            }
        }
        
        valType sum(void) const{ return accumulate(std::plus<valType>()); }
    
    };
    


    
    /******** + *********/
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrapper<T1>::valType, typename Wrapper<T2>::valType>::type>
    Wrapper<Proxy<T1, T2, std::plus<T3>>> operator+(Wrapper<T1> const& lhs, Wrapper<T2> const& rhs) {
        T1 const& left(lhs);
        T2 const& right(rhs);
        Proxy<T1, T2, std::plus<T3>> proxy(left, right, std::plus<T3>());
        return Wrapper<Proxy<T1, T2, std::plus<T3>>> (proxy);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrapper<T1>::valType, EnableIfScalar<T2>>::type>
    Wrapper<Proxy<T1, scalar<T2>, std::plus<T3>>> operator+(Wrapper<T1> const& lhs, T2 const& rhs) {
        scalar<T2> right(rhs);
        return lhs + Wrapper<scalar<T2>>(right);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<EnableIfScalar<T1>, typename Wrapper<T2>::valType>::type>
    Wrapper<Proxy<scalar<T1>, T2, std::plus<T3>>> operator+(T1 const& lhs, Wrapper<T2> const& rhs) {
        scalar<T1> left(lhs);
        return Wrapper<scalar<T1>>(left) + rhs;
    }
    
    /********** - **********/
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrapper<T1>::valType, typename Wrapper<T2>::valType>::type>
    Wrapper<Proxy<T1, T2, std::minus<T3>>> operator-(Wrapper<T1> const& lhs, Wrapper<T2> const& rhs) {
        T1 const& left(lhs);
        T2 const& right(rhs);
        Proxy<T1, T2, std::minus<T3>> proxy(left, right, std::minus<T3>());
        return Wrapper<Proxy<T1, T2, std::minus<T3>>> (proxy);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrapper<T1>::valType, EnableIfScalar<T2>>::type>
    Wrapper<Proxy<T1, scalar<T2>, std::minus<T3>>> operator-(Wrapper<T1> const& lhs, T2 const& rhs) {
        scalar<T2> right(rhs);
        return lhs - Wrapper<scalar<T2>>(right);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<EnableIfScalar<T1>, typename Wrapper<T2>::valType>::type>
    Wrapper<Proxy<scalar<T1>, T2, std::minus<T3>>> operator-(T1 const& lhs, Wrapper<T2> const& rhs) {
        scalar<T1> left(lhs);
        return Wrapper<scalar<T1>>(left) - rhs;
    }
    
    /********* * ************/
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrapper<T1>::valType, typename Wrapper<T2>::valType>::type>
    Wrapper<Proxy<T1, T2, std::multiplies<T3>>> operator*(Wrapper<T1> const& lhs, Wrapper<T2> const& rhs) {
        T1 const& left(lhs);
        T2 const& right(rhs);
        Proxy<T1, T2, std::multiplies<T3>> proxy(left, right,std::multiplies<T3>());
        return Wrapper<Proxy<T1, T2, std::multiplies<T3>>> (proxy);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrapper<T1>::valType, EnableIfScalar<T2>>::type>
    Wrapper<Proxy<T1, scalar<T2>, std::multiplies<T3>>> operator*(Wrapper<T1> const& lhs, T2 const& rhs) {
        scalar<T2> right(rhs);
        return lhs * Wrapper<scalar<T2>>(right);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<EnableIfScalar<T1>, typename Wrapper<T2>::valType>::type>
    Wrapper<Proxy<scalar<T1>, T2, std::multiplies<T3>>> operator*(T1 const& lhs, Wrapper<T2> const& rhs) {
        scalar<T1> left(lhs);
        return Wrapper<scalar<T1>>(left) * rhs;
    }
    
    /********* / ************/
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrapper<T1>::valType, typename Wrapper<T2>::valType>::type>
    Wrapper<Proxy<T1, T2, std::divides<T3>>> operator/(Wrapper<T1> const& lhs, Wrapper<T2> const& rhs) {
        T1 const& left(lhs);
        T2 const& right(rhs);
        Proxy<T1, T2, std::divides<T3>> proxy(left, right, std::divides<T3>());
        return Wrapper<Proxy<T1, T2, std::divides<T3>>> (proxy);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<typename Wrapper<T1>::valType, EnableIfScalar<T2>>::type>
    Wrapper<Proxy<T1, scalar<T2>, std::divides<T3>>> operator/(Wrapper<T1> const& lhs, T2 const& rhs) {
        scalar<T2> right(rhs);
        return lhs / Wrapper<scalar<T2>>(right);
    }
    
    template <typename T1, typename T2, typename T3 = typename choose_type<EnableIfScalar<T1>, typename Wrapper<T2>::valType>::type>
    Wrapper<Proxy<scalar<T1>, T2, std::divides<T3>>> operator/(T1 const& lhs, Wrapper<T2> const& rhs) {
        scalar<T1> left(lhs);
        return Wrapper<scalar<T1>>(left) / rhs;
    }
    
    template<typename T>
    std::ostream& operator<<(std::ostream& out, const Wrapper<T>& arr){
        const char* gap = "";
        for(auto& elem: arr){
            std::cout<< gap << elem;
            gap = ", ";
        }
        return out;
    }
   
    
}
#endif /* _Valarray_h */

