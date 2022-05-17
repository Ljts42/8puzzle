#pragma once

#include "board.h"

#include <queue>
#include <unordered_map>

class Solver
{
    class Solution
    {
    public:
        Solution() = default;
        Solution(const std::vector<Board> & moves)
            : m_moves(moves)
        {
        }

        std::size_t moves() const
        {
            return m_moves.size() - (m_moves.empty() ? 0 : 1);
        }

        using const_iterator = std::vector<Board>::const_iterator;

        const_iterator begin() const { return m_moves.begin(); }

        const_iterator end() const { return m_moves.end(); }

    private:
        std::vector<Board> m_moves;
    };

    struct Parameters
    {
        unsigned depth;
        unsigned heuristic;
        const Board * parent;

        Parameters() = default;
        Parameters(const unsigned d, const unsigned h, const Board * p)
            : depth(d)
            , heuristic(h)
            , parent(p)
        {
        }
    };

public:
    static Solution solve(const Board &);
};
