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
        Solution(const std::vector<Board> &);

        std::size_t moves() const;

        using const_iterator = std::vector<Board>::const_iterator;
        const_iterator begin() const;
        const_iterator end() const;

    private:
        std::vector<Board> m_moves;
    };

    struct Parameters
    {
        const Board * m_parent;
        unsigned m_depth;
        unsigned m_value;

        Parameters() = default;
        Parameters(const Board *, const unsigned, const Board &);

        static unsigned calc(const Board *, const unsigned, const Board &);
    };

public:
    static Solution solve(const Board &);
};
