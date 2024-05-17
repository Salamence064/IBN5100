#pragma once

#include <cstdint>
#include <cstring>
#include <cassert>
#include <type_traits>

namespace IBN5100 {
    // * ============================================================
    // * Util functions to compute the next prime at compile time
    // * ============================================================

    static constexpr uint64_t med(uint64_t min, uint64_t max) { return (max + min)/2; };

    // Use a modified binary seach to determine if n has a factor between min (inclusive) and max (exclusive)
    static constexpr bool hasFactor(uint64_t n, uint64_t min, uint64_t max) {
        return min*min > n ? 0 : // there cannot be factors above sqrt(n)
            min + 1 >= max ? !(n%min) : // if we are at the end of the search, check if min is a factor
            hasFactor(n, min, med(min, max)) || hasFactor(n, med(min, max), max);
    };

    // Return the next prime number greater than or equal to n (n >= 2)
    static constexpr uint64_t nextPrime(uint64_t n) { return hasFactor(n, 2, n) ? nextPrime(n+1) : n; };

    static constexpr unsigned int log2(unsigned int n) { return n <= 1 ? 0 : log2(n/2) + 1; };


    /**
     * @brief A collection of positions previously explored by our solver. This is used to avoid re-exploring the same position.
     * It is stored as a HashMap with a fixed size. In case of collision, the last entry is kept and the previous one is overwritten.
     * When storing the score, we differentiate if it's a lower bound or upper bound by adding or subtracting a constant to the score.
     * We have 2*(maxScore - minScore + 1) possibilities, which for 7x6 is 74. 74 - 18 = 56, so we store lower bounds by adding 56.
     * This makes lower bound scores in [38, 74]. Further, 37 - 18 = 19, so we store upper bounds by adding 19. This makes upper bounds
     * scores in [1, 37]. Thus, we can differentiate between lower and upper bounds by checking if the score is above 37. Note, it is
     * important to not try to use rule of 5 operators (other than the destructor) for this class as they are not implemented.
     * 
     * @tparam keySize (uint) Number of bits of the key.
     * @tparam valueSize (uint) Number of bits of the value.
     * @tparam logSize (uint) Log2 of the size of the Transposition Table.
     */
    template<unsigned int keySize, unsigned int valueSize, unsigned int logSize>
    class TranspositionTable {
        private:
            static_assert(keySize   <= 64, "keySize is too large");
            static_assert(valueSize <= 64, "valueSize is too large");
            static_assert(logSize   <= 64, "logSize is too large");

            template<int S> using uint_t = 
                typename std::conditional<S <= 8, uint_least8_t, 
                typename std::conditional<S <= 16, uint_least16_t,
                typename std::conditional<S <= 32, uint_least32_t, 
                                                    uint_least64_t>::type >::type >::type;

            typedef uint_t<keySize - logSize> key_t;
            typedef uint_t<valueSize> value_t;

            static constexpr uint64_t size = nextPrime(1ULL << logSize);

            key_t* keys;
            value_t* values;

            static constexpr size_t index(uint64_t key) { return key%size; };

        public:
            inline TranspositionTable() {
                keys = new key_t[size];
                values = new value_t[size];
                reset();
            };

            inline ~TranspositionTable() {
                delete[] keys;
                delete[] values;
            };

            inline void reset() {
                std::memset(keys, 0, size*sizeof(key_t));
                std::memset(values, 0, size*sizeof(value_t));
            };

            /**
             * @brief Add a position with its score to the transposition table.
             * 
             * @param key (uint64) The position's unique key. Must be less than keySize bits.
             * @param value (value_t) The position's score. Must be less than valueSize bits.
             */
            inline void add(uint64_t key, value_t value) {
                assert(!(key >> keySize));
                assert(!(value >> valueSize));
                
                size_t pos = index(key);
                keys[pos] = key;
                values[pos] = value;
            };

            /**
             * @brief Get the score from a position's unique key.
             * 
             * @param key (uint64) The position's unique key. Must be less than keySize bits.
             * @return (value_t) The position's score if present, 0 otherwise.
             */
            inline value_t operator [](uint64_t key) const {
                assert(!(key >> keySize));
                
                size_t pos = index(key);
                return keys[pos] == (key_t) key ? values[pos] : 0; // cast to key_t as keys[pos] may have been truncated
            };
    };
}
