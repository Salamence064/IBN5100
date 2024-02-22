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
            uint64_t pos; // bitmap storing a 1 to represent every cell the current player has a piece in
            uint64_t mask; // bitmask storing a 1 to represent every cell containing a piece
            uint8_t moves; // moves played since the start of the game

            static constexpr uint64_t bottomMask = bottom(7, 6);
            static constexpr uint64_t boardMask = bottomMask * ((1LL << 6) - 1);

            uint64_t winPos() const;

            uint64_t oppWinPos() const;

            uint64_t possibleMoves() const;

            static constexpr uint64_t computeWinPos(uint64_t pos, uint64_t mask);

            // Count the number of 1s in a bitmap
            static constexpr uint8_t numOnes(uint64_t);

            static constexpr uint64_t topMaskCol(uint8_t c);

            static constexpr uint64_t bottomMaskCol(uint8_t c);

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
            inline uint8_t init(std::string seq) {
                uint8_t c;

                for (uint8_t i = 0; i < seq.size(); ++i) {
                    c = seq[i] - '1';
                    if (c < 0 || c >= 7 || !canPlay(c) || isWin(c)) { return i; }
                    play(c);
                }

                return seq.size();
            };

            inline bool canPlay(uint8_t c) const { return !(mask & topMaskCol(c)); };
            inline void play(uint8_t c) { play((mask | bottomMaskCol(c)) & columnMask(c)); };

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
