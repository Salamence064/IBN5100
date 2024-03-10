#pragma once

#include "position.h"

namespace IBN5100 {
    /**
     * @brief Store data about the current priority of the moves. Element at the end = greatest score.
     * This is implemented as a minimum priority queue and is used to explore the moves in an optimized order.
     * 
     */
    class MoveSorter {
        private:
            uint8_t size;
            struct { uint64_t move; int8_t score; } moves[7];

        public:
            inline MoveSorter() : size{0} {};

            /**
             * @brief Add a move with its score. Do not try to add more than 7 moves.
             * 
             * @param move (uint64) The move to add.
             * @param score (int8) The score of the move.
             */
            inline void add(uint64_t move, int8_t score) {
                uint8_t i = size++;
                for (; i && moves[i - 1].score > score; --i) { moves[i] = moves[i - 1]; }
                moves[i].move = move;
                moves[i].score = score;
            };

            /**
             * @brief Get the next move to explore and remove it from the queue.
             * 
             * @return (uint64) The move to explore next. 0 is returned when there are no moves available.
             */
            inline uint64_t getNext() {
                if (size) { return moves[--size].move; } 
                return 0;
            };

            inline void reset() { size = 0; };
    };
}
