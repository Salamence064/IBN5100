#pragma once

#include "position.h"

namespace IBN5100 {
    class Solver {
        private:
            uint64_t nodeCount; // track the number of explored nodes
            int colOrder[7] = {3, 4, 2, 5, 1, 6, 0}; // current priority for the columns
        
            int negamax();

        public:
            constexpr Solver() : nodeCount{0} {};

            int solve();

            inline uint64_t getNodeCount() { return nodeCount; };

            inline void reset() {
                nodeCount = 0;
                // ! reset the transposition table
            };
    };
}
