#include "solver.h"

#include <cmath>

namespace std {

template <>
struct hash<Board>
{
    unsigned operator()(const Board & board) const
    {
        return board.hash();
    }
};

} // namespace std

Solver::Solution::Solution(const std::vector<Board> & moves)
    : m_moves(moves)
{
}

std::size_t Solver::Solution::moves() const
{
    return m_moves.size() - (m_moves.empty() ? 0 : 1);
}

Solver::Solution::const_iterator Solver::Solution::begin() const
{
    return m_moves.cbegin();
}

Solver::Solution::const_iterator Solver::Solution::end() const
{
    return m_moves.cend();
}

Solver::Parameters::Parameters(const Board * p, const unsigned d, const Board & b)
    : parent{p}
    , depth{d}
    , value{calc(p, d, b)}
{
}

unsigned Solver::Parameters::calc(const Board * p, const unsigned d, const Board & b)
{
    if (p != nullptr && p->size() < 5) {
        return d + b.hamming() + b.manhattan() * 2;
    }
    return 2 * d / std::log(d) + b.hamming() + b.manhattan() * 2;
}

Solver::Solution Solver::solve(const Board & initial)
{
    if (!initial.is_solvable()) {
        return Solution();
    }

    std::unordered_map<Board, Parameters> states;
    auto comparator = [&states](const Board & lhs, const Board & rhs) {
        return states[lhs].value >= states[rhs].value;
    };
    std::priority_queue<Board, std::vector<Board>, decltype(comparator)> queue(comparator);

    states[initial] = Parameters(nullptr, 0, initial);
    queue.push(initial);

    while (!queue.empty()) {
        const Board & current = queue.top();
        if (current.is_goal()) {
            break;
        }

        unsigned depth = states[current].depth + 1;
        const Board * cur_ptr = &((*states.find(current)).first);

        auto available = current.next_moves();
        queue.pop();

        for (unsigned i = 0; i < available.size(); ++i) {
            if (states.count(available[i])) {
                continue;
            }
            states[available[i]] = Parameters(cur_ptr, depth, available[i]);
            queue.push(available[i]);
        }
    }

    std::vector<Board> m_moves(states[queue.top()].depth + 1);
    m_moves.back() = queue.top();
    for (std::size_t i = 1; i < m_moves.size(); ++i) {
        m_moves[m_moves.size() - i - 1] = *states[m_moves[m_moves.size() - i]].parent;
    }

    return Solution(m_moves);
}
