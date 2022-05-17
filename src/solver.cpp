#include "solver.h"

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

Solver::Solution Solver::solve(const Board & initial)
{
    if (!initial.is_solvable()) {
        return Solution();
    }

    std::unordered_map<Board, Parameters> states;
    auto comparator = [&states](const Board & lhs, const Board & rhs) {
        return states[lhs].depth + states[lhs].heuristic * 1.7 > states[rhs].depth + states[rhs].heuristic * 1.7;
    };
    std::priority_queue<Board, std::vector<Board>, decltype(comparator)> queue(comparator);

    states[initial] = Parameters(0, initial.hamming() + initial.manhattan(), nullptr);
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
            unsigned value = available[i].hamming() + available[i].manhattan();
            states[available[i]] = Parameters(depth, value, cur_ptr);
            queue.push(available[i]);
        }
    }

    std::vector<Board> m_moves(states[queue.top()].depth + 1);
    m_moves.back() = queue.top();
    for (unsigned i = 1; i < m_moves.size(); ++i) {
        m_moves[m_moves.size() - i - 1] = *states[m_moves[m_moves.size() - i]].parent;
    }

    return Solution(m_moves);
}
