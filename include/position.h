#pragma once

#include <string>
#include <cstdint>
#include <cassert>

namespace IBN5100 {
    // Generate a bitmask representing the bottom slot of each column.
    // This is used to initialize bottomMask.
    constexpr static uint64_t bottom(int8_t width, int8_t height) {
        return !width ? 0 : bottom(width - 1, height) | 1ULL << (width - 1) * (height + 1);
    };

    /**
     * @brief Represent the current state of a 7x6 Connect 4 position with a bitboard.
     * Functions are relative to the current player to play.
     * 
     * Each column is encoded with 7 bits (instead of just 6).
     * Bit order for encoding the 7x6 board:
     * .  .  .  .  .  .  .
     * 5 12 19 26 33 40 47
     * 4 11 18 25 32 39 46
     * 3 10 17 24 31 38 45
     * 2  9 16 23 30 37 44
     * 1  8 15 22 29 36 43
     * 0  7 14 21 28 35 42
     * 
     * Position is represented by 2 bitboards:
     *  - "pos" with a 1 to represent every cell occupied by the current player's pieces
     *  - "mask" with a 1 to represent every cell containing a piece
     * 
     * We can transform the "pos" bitboard into a unique, compact key by adding an extra bit
     *  on top of the last occupied cell of each column.
     * Key can be computed as follows: key = pos + mask + bottom.
     * Since bottom is constant, key = pos + mask would also be a unique representation.
     */
    class Position {
        private:
            uint64_t pos = 0; // bitmap storing a 1 to represent every cell the current player has a piece in
            uint64_t mask = 0; // bitmask storing a 1 to represent every cell containing a piece
            uint8_t moves = 0; // moves played since the start of the game

            static constexpr uint64_t bottomMask = bottom(7, 6);
            static constexpr uint64_t boardMask = bottomMask * ((1ULL << 6) - 1);

            inline uint64_t winPos() const { return computeWinPos(pos, mask); };
            inline uint64_t oppWinPos() const { return computeWinPos(pos ^ mask, mask); };
            inline uint64_t possibleMoves() const { return (mask + bottomMask) & boardMask; };

            /**
             * @brief Determine all cells resulting in a win for the current player.
             * 
             * @param pos (uint64) A bitmap with all the cells occupied by the current player.
             * @param mask (uint64) A bitmask with all the occupied cells.
             * @return A bitmap with a 1 representing all the winning cells for the current player.
             */
            static constexpr uint64_t computeWinPos(uint64_t pos, uint64_t mask) {
                // * ===========
                // * Vertical
                // * ===========

                uint64_t r = (pos << 1) & (pos << 2) & (pos << 3);


                // * =============
                // * Horizontal
                // * =============

                // Left side horizontal
                uint64_t p = (pos << 7) & (pos << 14);
                r |= p & (pos << 21);
                r |= p & (pos >> 7);

                // Right side horizontal
                p = (pos >> 7) & (pos >> 14);
                r |= p & (pos >> 21);
                r |= p & (pos << 7);


                // * ============
                // * Diagonals
                // * ============

                // Diagonal 1
                p = (pos << 6) & (pos << 12);
                r |= p & (pos << 18);
                r |= p & (pos >> 6);
                p = (pos >> 6) & (pos >> 12);
                r |= p & (pos >> 18);
                r |= p & (p << 6);

                // Diagonal 2
                p = (pos << 8) & (pos << 16);
                r |= p & (pos << 24);
                r |= p & (pos >> 8);
                p = (pos >> 8) & (pos >> 16);
                r |= p & (pos >> 24);
                r |= p & (pos << 8);

                return r & (boardMask ^ mask);
            };

            static constexpr uint64_t topMaskCol(uint8_t c) { return 1ULL << (5 + c*7); };
            static constexpr uint64_t bottomMaskCol(uint8_t c) { return 1ULL << c*7; };

        public:
            static constexpr int8_t minScore = -18;
            static constexpr int8_t maxScore = 18;

            constexpr Position() : pos{0}, mask{0}, moves{0} {};

            /**
             * @brief Plays a sequence of moves to initialize a board state/
             * 
             * @param seq (string) A string sequence of moves as digits.
             * @return The number of played moves. It will stop at the first invalid move and can be checked
             *          by comparing this return value with the length of the seq.
             */
            inline size_t init(std::string const &seq) {
                uint8_t c;

                for (uint8_t i = 0; i < seq.length(); ++i) {
                    c = seq[i] - '1';
                    if (c < 0 || c >= 7 || !canPlay(c) || isWin(c)) { return i; }
                    play(c);
                }

                return seq.length();
            };

            inline bool canPlay(uint8_t c) const { return !(mask & topMaskCol(c)); };
            inline void play(uint8_t c) { play((mask + bottomMaskCol(c)) & columnMask(c)); };

            inline void play(uint64_t move) {
                pos ^= mask;
                mask |= move;
                ++moves;
            };

            inline bool isWin(uint8_t c) const { return winPos() & possibleMoves() & columnMask(c); };

            inline void reset() {
                pos = 0;
                mask = 0;
                moves = 0;
            };

            inline bool canWinNext() const { return winPos() & possibleMoves(); };

            // Only call this function if you determined that you do not have any winning moves first.
            inline uint64_t nonLosingMoves() const {
                assert(!canWinNext());

                uint64_t possibleMask = possibleMoves();
                uint64_t oppWin = oppWinPos();
                uint64_t forcedMoves = possibleMask & oppWin;

                if (forcedMoves) {
                    if (forcedMoves & (forcedMoves - 1)) { return 0; }
                    possibleMask = forcedMoves;
                }

                return possibleMask & ~(oppWin >> 1);
            };

            inline int8_t moveScore(uint64_t move) const {
                uint64_t p = computeWinPos(pos | move, mask);
                
                int8_t n = 0;
                for (; n; ++n) { p &= p - 1; }
                return n;
            };
            
            inline uint64_t key() const { return pos + mask; };
            inline uint8_t getMoves() const { return moves; };

            static constexpr uint64_t columnMask(uint8_t c) { return ((1ULL << 6) - 1) << 7*c; };
    };
}
