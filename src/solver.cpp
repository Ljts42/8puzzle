#include "solver.h"

#include <algorithm>
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

Solver::Parameters::Parameters(const Board * parent, const unsigned depth, const Board & board)
    : m_parent{parent}
    , m_depth{depth}
    , m_value{calc(parent, depth, board)}
{
}

unsigned Solver::Parameters::calc(const Board * parent, const unsigned depth, const Board & board)
{
    double coef = std::log(depth);
    if ((parent != nullptr && parent->size() < 5) || coef == 0) {
        return depth + board.hamming() + board.manhattan() * 2;
    }
    return 2 * depth / coef + board.hamming() + board.manhattan() * 2;
}

Solver::Solution Solver::solve(const Board & initial)
{
    if (!initial.is_solvable()) {
        return Solution();
    }

    std::unordered_map<Board, Parameters> states;
    auto comparator = [&states](const Board & lhs, const Board & rhs) {
        return states[lhs].m_value >= states[rhs].m_value;
    };
    std::priority_queue<Board, std::vector<Board>, decltype(comparator)> queue(comparator);

    states[initial] = Parameters(nullptr, 0, initial);
    queue.push(initial);

    while (!queue.empty()) {
        const Board & current = queue.top();
        if (current.is_goal()) {
            break;
        }

        unsigned depth = states[current].m_depth + 1;
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

    std::vector<Board> m_moves(states[queue.top()].m_depth + 1);
    m_moves.back() = queue.top();
    for (std::size_t i = 1; i < m_moves.size(); ++i) {
        m_moves[m_moves.size() - i - 1] = *states[m_moves[m_moves.size() - i]].m_parent;
    }

    return Solution(m_moves);
}
