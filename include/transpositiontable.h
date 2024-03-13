#pragma once

#include <cstdint>
#include <cstring>
#include <cassert>
#include <type_traits>

namespace IBN5100 {
    // * ============================================================
    // * Util functions to compute the next prime at compile time
    // * ============================================================

    constexpr uint64_t med(uint64_t min, uint64_t max) { return (min+max)/2; };

    // Use a modified binary seach to determine if n has a factor between min (inclusive) and max (exclusive)
    constexpr bool hasFactor(uint64_t n, uint64_t min, uint64_t max) {
        return min*min > n ? false : // there cannot be factors above sqrt(n)
            min + 1 >= max ? !(n%min) : // if we are at the end of the search, check if min is a factor
            hasFactor(n, min, med(min,max)) || hasFactor(n, med(min,max), max);
    };

    // Return the next prime number greater than or equal to n (n >= 2)
    constexpr uint64_t nextPrime(uint64_t n) { return hasFactor(n, 2, n) ? nextPrime(n+1) : n; };

    constexpr unsigned int log2(unsigned int n) { return n <= 1 ? 0 : log2(n/2)+1; };


    /**
     * @brief A collection of positions previously explored by our solver. This is used to avoid re-exploring the same position.
     * It is stored as a HashMap with a fixed size. In case of collision, the last entry is kept and the previous one is overwritten.
     * 
     * @tparam keySize (uint) Number of bits of the key.
     * @tparam valueSize (uint) Number of bits of the value.
     * @tparam logSize (uint) Log2 of the size of the Transposition Table.
     */
    template<unsigned int keySize, unsigned int valueSize, unsigned int logSize>
    class TranspositionTable {
        private:
            static_assert(keySize <= 64, "keySize is too large");
            static_assert(valueSize <= 64, "valueSize is too large");
            static_assert(logSize <= 64, "logSize is too large");

            template<int S> using uint_t =
                    typename std::conditional<(S <= 8),  uint_least8_t,
                    typename std::conditional<(S <= 16), uint_least16_t, 
                    typename std::conditional<(S <= 32), uint_least32_t, 
                                                         uint_least64_t>::type>::type>::type;

            typedef uint_t<keySize - logSize> key_t;
            typedef uint_t<valueSize> value_t;

            static constexpr uint64_t size = nextPrime(1ULL << logSize);

            key_t* keys;
            value_t* values;

            static constexpr size_t index(key_t key) { return key % size; };

        public:
            constexpr TranspositionTable() {
                keys = new key_t[size];
                values = new value_t[size];
                reset();
            };

            inline TranspositionTable(TranspositionTable const &other) {
                keys = new key_t[other.size];
                values = new value_t[other.size];
                
                for (size_t i = 0; i < other.size; ++i) {
                    keys[i] = other.keys[i];
                    values[i] = other.values[i];
                }
            };

            inline TranspositionTable(TranspositionTable &&other) {
                keys = other.keys;
                values = other.values;
                other.keys = nullptr;
                other.values = nullptr;
            };

            inline TranspositionTable& operator=(TranspositionTable const &other) {
                if (this != &other) {
                    delete[] keys;
                    delete[] values;
                    keys = new key_t[other.size];
                    values = new value_t[other.size];
                    
                    for (size_t i = 0; i < other.size; ++i) {
                        keys[i] = other.keys[i];
                        values[i] = other.values[i];
                    }
                }

                return *this;
            };

            inline TranspositionTable& operator=(TranspositionTable &&other) {
                if (this != &other) {
                    delete[] keys;
                    delete[] values;
                    keys = other.keys;
                    values = other.values;
                    other.keys = nullptr;
                    other.values = nullptr;
                }

                return *this;
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
             * @param key (uint64) The position's unqique key. Must be less than keySize bits.
             * @param value (value_t) The position's score. Must be less than valueSize bits.
             */
            inline void add(uint64_t key, value_t value) {
                assert(key >> keySize == 0);
                assert(value >> valueSize == 0);

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
            inline value_t operator[](uint64_t key) const {
                assert(key >> keySize == 0);

                size_t pos = index(key);
                return keys[pos] == (key_t) key ? values[pos] : 0;
            };
    };
}
