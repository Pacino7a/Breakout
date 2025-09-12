// RANDOM_MT_H
#ifndef RANDOM_MT_H
#define RANDOM_MT_H

#include <random>
#include <chrono>

// Return a seeded Mersenne Twister
namespace Random
{
    // seeds generator
    inline std::mt19937 generator()
    {
        std::random_device rd{};
        // inintialize the seed_seq with 1 clock-value and 7 random_device values
        std::seed_seq seq{
            static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
            rd(), rd(), rd(), rd(), rd(), rd(), rd()};

        return std::mt19937{seq};// creates 624 random numbers with 8 seeds (4 Bytes for each seed)
    }
    
    // global std::mt19937 object
    inline std::mt19937 mt{generator()}; // instantiated by copying generator()'s return 

    // control the range of random values
    inline int get(int min, int max)
    {
        return std::uniform_int_distribution{min, max}(mt); // from the mt19937's instantiation to contain the range what we want
    }

    // generates a random value between [min,max]
    // min and max are same type
    // return value is the same type with them too
    // int and unsigned int or their variants are supported
    // sample： Random::get(1L,6L)
    template<typename T>
    T get(T min,T max)
    {
        return std::uniform_int_distribution<T>{min, max}(mt);
    }

    // or the type of three of them are different
    // convert them
    template<typename R, typename S, typename T>
    R get(S min,T max)
    {
        return std::uniform_int_distribution<R>{static_cast<R>(min),static_cast<R>(max)}(mt);
    }

} // namespace Random


#endif
