#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

#include "../include/openingbook.h"

namespace IBN5100 {
    void OpeningBook::writeToFile(std::string const &filename, char* buffer, size_t bufferSize, uint16_t numPositions) {
        // save the number of positions
        size_t temp = 0; // just to allow us to serialize
        serializePrimitive<uint16_t>(buffer, temp, numPositions);

        // write the buffer to the file
        std::fstream f(filename, std::fstream::binary | std::fstream::out | std::fstream::trunc);
        f.write(buffer, bufferSize);
        f.close();
    };

    void OpeningBook::save(Position const &pos, int score) {
        if (numPos == UINT16_MAX) { // ensure we do not add more positions than we can store
            std::cout << "Max number of positions stored.\n"; // todo for debugging
            return;
        }

        serializePrimitive<uint64_t>(buffer, bufferSize, pos.key());
        serializePrimitive<int>(buffer, bufferSize, score);

        ++numPos;
    };

    void OpeningBook::load(TransposeTable* t) {
        std::fstream f(filename, std::fstream::binary | std::fstream::in);

        if (!f.is_open()) { // ensure the file exists
            std::cout << "No opening book detected.\n"; // todo this is just for debugging, remove later
            return;
        }

        // Load the data from the file into a buffer
        std::vector<char> buf(std::istreambuf_iterator<char>(f), {});
        bufferSize = 0;
        f.close();

        // read in the number of stored positions
        numPos = deserializePrimitive<uint16_t>(buf, bufferSize);

        // add each position to our transposition table
        for (uint16_t i = 0; i < numPos; ++i) {
            uint64_t key = deserializePrimitive<uint64_t>(buf, bufferSize);
            int score = deserializePrimitive<int>(buf, bufferSize);

            // store the score as an absolute bound
            t->add(key, score + 2*Position::maxScore - 3*Position::minScore + 3);
        }

        // save the data to the this instance of the opening book
        std::memcpy(buffer + 2, buf.data(), (bufferSize - 2) * sizeof(char));

        std::cout << "Opening book loaded into transposition table.\n"; // todo also just for debugging
    };
}
