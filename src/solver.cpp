#pragma once

#include "solver.h"

namespace IBN5100 {
    int Solver::negamax(Position const &pos, int alpha, int beta) {
        assert(alpha < beta);
        assert(!pos.canWinNext());

        ++nodeCount;
    };

    int Solver::solve(Position const &pos, bool weak) {
        if (pos.canWinNext()) { return (43 - pos.getMoves())/2; }

        int min = -(42 - pos.getMoves())/2;
        int max = (43 - pos.getMoves())/2 - 1; // subtract 1 as we cannot win this turn

        if (weak) {
            min = -1;
            max = 1;
        }

        // iteratively narrow the search window
        while (min < max) {
            int med = min + (min + max)/2;

            if (med <= 0 && min/2 < med) { med = min/2; }
            else if (med >= 0 && max/2 > med) { med = max/2; }

            // Use a search window of depth 1 to see if the actual score is less than or greater than med.
            // From this result, we can then modify the min or max accordingly.
            int temp = negamax(pos, med, med + 1);

            if (temp <= med) { max = temp; }
            else { min = temp; }
        }

        return min;
    };
}
