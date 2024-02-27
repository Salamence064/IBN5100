#pragma once

#include "position.h"

namespace IBN5100 {
    class Solver {
        private:
            uint64_t nodeCount; // track the number of explored nodes
            uint8_t colOrder[7] = {3, 4, 2, 5, 1, 6, 0}; // current priority for the columns
        
            /**
             * @brief Recursively solve a Connect 4 position using a negamax alpha-beta pruning algorithm.
             * 
             * @param pos (Position) The position to evaluate. It is assumed that no one has already won and that
             *              the current player cannot win this move.
             * @param alpha (int8) The lower bound score of the position.
             * @param beta (int8) The upper bound score of the position.
             * @return The exact score of the position.
             */
            int8_t negamax(Position const &pos, int8_t alpha, int8_t beta);

        public:
            constexpr Solver() : nodeCount{0} {};

            int8_t solve(Position const &pos, bool weak = 0);

            inline uint64_t getNodeCount() const { return nodeCount; };

            inline void reset() {
                nodeCount = 0;
                // ! reset the transposition table
            };
    };
}
