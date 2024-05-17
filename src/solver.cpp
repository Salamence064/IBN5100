#include <iostream>
#include "../include/solver.h"

namespace IBN5100 {
    int Solver::negamax(Position const &pos, int alpha, int beta) {
        assert(alpha < beta);
        assert(!pos.canWinNext());

        ++nodeCount;
        int moves = pos.getMoves();

        uint64_t possible = pos.nonLosingMoves();

        if (!possible) { return -(42 - moves)/2; }

        // Check for a drawn game.
        // Since we have ruled out both us winning this turn and our opponent winning next move,
        //  we only need to see if the total number of moves made is 40 or more.
        if (moves >= 40) { return 0; }

        // Compute the new minimum possible score since our opponent cannot win on their next move.
        int min = -(40 - moves)/2;
        
        // Check if we need to update our lower bound.
        if (alpha < min) {
            alpha = min;
            if (alpha >= beta) { return alpha; }
        }

        // Compute the maximum possible score as we cannot win on this move.
        int max = (41 - moves)/2;

        // Check if we need to update our upper bound.
        if (beta > max) {
            beta = max;
            if (alpha >= beta) { return beta; }
        }

        // Check if we have a position stored in our transposition table.
        // If we do, we will update the bounds accordingly.
        uint64_t key = pos.key();

        if (int val = transTable[key]) {
            if (val > Position::maxScore - Position::minScore + 1) { // we have a lower bound
                min = val - Position::maxScore + 2*Position::minScore - 2; // decode the stored lower bound

                // Check if we need to update our lower bound.
                if (alpha < min) {
                    alpha = min;
                    if (alpha >= beta) { return alpha; }
                }

            } else { // we have an upper bound
                max = val + Position::minScore - 1; // decode the stored upper bound

                // Check if we need to update our upper bound.
                if (beta > max) {
                    beta = max;
                    if (alpha >= beta) { return beta; }
                }
            }
        }

        MoveSorter movesOrder;

        for (int i = 0; i < 7; ++i) {
            if (uint64_t move = possible & Position::columnMask(colOrder[i])) {
                movesOrder.add(move, pos.moveScore(move));
            }
        }

        // Simulate each possible move.
        // The max score from all the possible moves is the score of the current position.
        while (uint64_t move = movesOrder.getNext()) {
            Position pos2(pos);
            pos2.play(move);

            // The score of the move would be equal to the negative score of the move for the opponent. 
            int score = -negamax(pos2, -beta, -alpha);

            // If the score is greater than or equal to the upper bound, we know we have found the best possible score.
            if (score >= beta) {
                // Save this as the lower bound of the position.
                transTable.add(key, score + Position::maxScore - 2*Position::minScore + 2);
                return score;
            }

            // Update our lower bound if needed.
            if (alpha < score) { alpha = score; }
        }

        // Save this as the upper bound of our position.
        transTable.add(key, alpha - Position::minScore + 1);
        return alpha;
    };

    int Solver::solve(Position const &pos, bool weak) {
        nodeCount = 0;

        if (pos.canWinNext()) { return (43 - pos.getMoves())/2; }

        int min = -(42 - pos.getMoves())/2;
        int max = (43 - pos.getMoves())/2 - 1; // subtract 1 as we cannot win this turn

        if (weak) {
            min = -1;
            max = 1;
        }

        // iteratively narrow the search window with a modified version of binary search
        while (min < max) {
            int med = min + (max - min)/2;

            if (med <= 0 && min/2 < med) { med = min/2; }
            else if (med >= 0 && max/2 > med) { med = max/2; }

            // Use a search window of depth 1 to see if the actual score is less than or greater than med.
            // From this result, we can then modify the min or max accordingly.
            int temp = negamax(pos, med, med + 1);

            // update the min and max accordingly
            if (temp <= med) { max = temp; }
            else { min = temp; }
        }

        return min;
    };
}
