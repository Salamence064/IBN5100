#pragma once

#include "position.h"

namespace IBN5100 {
    class Solver {
        private:
            uint64_t nodeCount; // track the number of explored nodes
            int colOrder[7] = {3, 4, 2, 5, 1, 6, 0}; // current priority for the columns
        
            /**
             * @brief Recursively solve a Connect 4 position using a negamax alpha-beta pruning algorithm.
             * 
             * @param pos (Position) The position to evaluate. It is assumed that no one has already won and that
             *              the current player cannot win this move.
             * @param alpha (int) The lower bound score of the position.
             * @param beta (int) The upper bound score of the position.
             * @return The exact score of the position.
             */
            int negamax(Position const &pos, int alpha, int beta);

        public:
            constexpr Solver() : nodeCount{0} {};

            int solve(Position const &pos, bool weak = 0);

            inline uint64_t getNodeCount() const { return nodeCount; };

            inline void reset() {
                nodeCount = 0;
                // ! reset the transposition table
            };
    };
}
