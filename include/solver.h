#pragma once

#include "transpositiontable.h"
#include "movesorter.h"

namespace IBN5100 {
    class Solver {
        private:
            uint64_t nodeCount; // track the number of explored nodes
            uint8_t colOrder[7] = {3, 4, 2, 5, 1, 6, 0}; // current priority for the columns
            TranspositionTable<49, log2(Position::maxScore - Position::minScore + 1) + 2, 23> transTable;
        
            /**
             * @brief Recursively solve a Connect 4 position using a negamax alpha-beta pruning algorithm.
             * 
             * @param pos (Position) The position to evaluate. It is assumed that no one has already won and that
             *              the current player cannot win this move.
             * @param alpha (int8) The lower bound score of the position.
             * @param beta (int8) The upper bound score of the position.
             * @return The exact score of the position.
             */
            int negamax(Position const &pos, int alpha, int beta);

        public:
            constexpr Solver() : nodeCount{0} {};

            /**
             * @brief Solve a Connect 4 position either weakly or strongly.
             * 
             * @param pos (Position) The position to solve. It is assumed that no one has already won.
             * @param weak (bool) Determines if the position will be weakly or strongly solved. If true,
             *  the function will return a positive number, negative number, or 0 as score. If false,
             *  the function will return the exact score.
             * @return (int) The score of the position. This will be the exact score if weak is false,
             *  otherwise it will be a positive number, negative number, or 0 as score.
             */
            int solve(Position const &pos, bool weak = 0);

            inline uint64_t getNodeCount() const { return nodeCount; };

            inline void reset() {
                nodeCount = 0;
                transTable.reset();
            };
    };
}
