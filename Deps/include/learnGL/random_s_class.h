#ifndef RANDOM_S_CLASS_H
#define RANDOM_S_CLASS_H
#include <random>
#include <chrono>
class Random
{
public:
    // static int get(int min, int max) // random number within [min, max]
    // {
    //     return std::uniform_int_distribution{min, max}(s_mt);
    // }
    template<typename T> // you need to specify what type R is
    static T get(T min,T max)
    {
        return std::uniform_int_distribution<T>{min,max}(s_mt);
    }
    
    template<typename R, typename S, typename T> // you need to specify what type R is
    static R get(S min,T max)
    {
        return std::uniform_int_distribution<R>{static_cast<R>(min),static_cast<R>(max)}(s_mt);
    }

    static std::size_t getRandomIndex(std::size_t min = 0, std::size_t max = 100000000)
    {
        return std::uniform_int_distribution{min,max}(s_mt);
    }

private:
    static std::mt19937 generator() // seeds
    {
        std::random_device rd{};
        // inintialize the seed_seq with 1 clock-value and 7 random_device values (8 seeds in total)
        std::seed_seq seq{
            static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
            rd(), rd(), rd(), rd(), rd(), rd(), rd()};

        return std::mt19937{seq}; // creates 624 random numbers with 8 seeds and return to initialize the static member
    }

    static inline std::mt19937 s_mt{generator()}; // main 
};

#endif
