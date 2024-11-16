#pragma once

#include <vector>

#include "position.h"
#include "transpositiontable.h"

// todo add a way to remove positions that have been stored in the opening book

// The default buffer size for opening book. This is equivalent to 512KB + 256KB + 2 bytes.
// This way we can store up to uint16_max positions in our buffer at any time alongside the number of positions.
#define BUFFER_SIZE 524288 + 262144 + 2

namespace IBN5100 {
    /**
     * @brief 
     * 
     */
    class OpeningBook {
        private:
            std::string filename;
            char buffer[BUFFER_SIZE] = {'\0'};
            size_t bufferSize = 2; // Start at 2 to reserve 2 bytes for the number of positions.
            uint16_t numPos = 0;

            // Serialize an arbitrary primitive of type T.
            // Do NOT use this function on non-trivial datatypes.
            // It is not recommended to use on pointers as only the memory address will be serialized.
            template <typename T>
            static constexpr void serializePrimitive(char* buffer, size_t &bufferSize, T n) {
                std::memcpy(&buffer[bufferSize], &n, sizeof(T));
                bufferSize += sizeof(T);
            };

            // Deserialize an arbitrary primitive of type T.
            // Do NOT use this function on non-trivial datatypes.
            // It is not recommended to use on pointers as only the memory address will be serialized.
            template <typename T>
            static constexpr T deserializePrimitive(std::vector<char> const &buffer, size_t &currIndex) {
                T n;
                std::memcpy(&n, &buffer[currIndex], sizeof(T));
                currIndex += sizeof(T);
                return n;
            };

            static void writeToFile(std::string const &filename, char* buffer, size_t bufferSize, uint16_t numPositions);
        
        public:
            inline OpeningBook(std::string const &filename) : filename{filename} {};

            inline ~OpeningBook() { writeToFile(filename, buffer, bufferSize, numPos); };

            // Save the position to the opening book via its key.
            void save(uint64_t key, int score);

            // Load the opening book into a TranspositionTable of default configuration.
            void load(TransposeTable* t);
    };
}
