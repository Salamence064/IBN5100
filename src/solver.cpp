#pragma once

#include "solver.h"

// todo if solver stuff stops working, try testing it with just the in data types to see if the int8_ts truncate information

namespace IBN5100 {
    int8_t Solver::negamax(Position const &pos, int8_t alpha, int8_t beta) {
        assert(alpha < beta);
        assert(!pos.canWinNext());

        ++nodeCount;
        uint8_t moves = pos.getMoves();

        uint64_t possible = pos.nonLosingMoves();
        if (!possible) { return -(42 - moves)/2; }

        // Check for a drawn game.
        // Since we have ruled out both us winning this turn and our opponent winning next move,
        //  we only need to see if the total number of moves made is 40 or more.
        if (moves >= 40) { return 0; }

        // Compute the new minimum possible score since our opponent cannot win on their next move.
        int8_t min = -(40 - moves)/2;
        
        // Check if we need to update our lower bound.
        if (alpha < min) {
            alpha = min;
            if (alpha >= beta) { return alpha; }
        }

        // Compute the maximum possible score as we cannot win on this move.
        int8_t max = (41 - moves)/2;

        // Check if we need to update our upper bound.
        if (beta > max) {
            beta = max;
            if (alpha >= beta) { return beta; }
        }

        // TODO: add transposition table stuff here

        // TODO: add MoveSorter stuff here (and update for loop to match)

        // Simulate each possible move.
        // The max score from all the possible moves is the score of the current position.
        for (uint8_t i = 0; i < 7; ++i) {
            Position pos2(pos);
            pos2.play(colOrder[i]);

            // The score of the move would be equal to the negative score of the move for the opponent. 
            int8_t score = -negamax(pos2, -beta, -alpha);

            // If the score is greater than or equal to the upper bound, we know we have found the best possible score.
            if (score >= beta) { return score; }

            // Update our lower bound if needed.
            if (alpha < score) { alpha = score; }
        }

        return alpha;
    };

    int8_t Solver::solve(Position const &pos, bool weak) {
        if (pos.canWinNext()) { return (43 - pos.getMoves())/2; }

        int8_t min = -(42 - pos.getMoves())/2;
        int8_t max = (43 - pos.getMoves())/2 - 1; // subtract 1 as we cannot win this turn

        if (weak) {
            min = -1;
            max = 1;
        }

        // iteratively narrow the search window
        while (min < max) {
            int8_t med = min + (min + max)/2;

            if (med <= 0 && min/2 < med) { med = min/2; }
            else if (med >= 0 && max/2 > med) { med = max/2; }

            // Use a search window of depth 1 to see if the actual score is less than or greater than med.
            // From this result, we can then modify the min or max accordingly.
            int8_t temp = negamax(pos, med, med + 1);

            if (temp <= med) { max = temp; }
            else { min = temp; }
        }

        return min;
    };
}
