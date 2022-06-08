#include "board.h"

#include <algorithm>
#include <random>

namespace {

std::pair<unsigned, unsigned> find_empty(const std::vector<std::vector<unsigned>> & data)
{
    for (unsigned col = 0; col < data.size(); ++col) {
        for (unsigned row = 0; row < data.size(); ++row) {
            if (data[col][row] == 0) {
                return {col, row};
            }
        }
    }
    return {0, 0};
}

unsigned calc_hash(const std::vector<std::vector<unsigned>> & data)
{
    unsigned value = 0;
    for (unsigned col = 0; col < data.size(); ++col) {
        for (unsigned row = 0; row < data.size(); ++row) {
            value = value * 31 + data[col][row];
        }
    }
    return value;
}

} // anonymous namespace

Board::Board(const std::vector<std::vector<unsigned>> & data)
    : m_data{data}
    , hash_value{calc_hash(data)}
    , m_empty{find_empty(data)}
{
}

Board::Board(std::vector<std::vector<unsigned>> && data)
    : m_data{std::move(data)}
    , hash_value{calc_hash(data)}
    , m_empty{find_empty(data)}
{
}

Board Board::create_goal(const unsigned size)
{
    std::vector<std::vector<unsigned>> goal(size, std::vector<unsigned>(size));
    unsigned tile = 1;
    for (unsigned col = 0; col < size; ++col) {
        for (unsigned row = 0; row < size; ++row, ++tile) {
            goal[col][row] = tile;
        }
    }
    goal[size - 1][size - 1] = 0;
    return Board(std::move(goal));
}

Board Board::create_random(const unsigned size)
{
    std::vector<unsigned> permutation(size * size, 0);
    std::iota(permutation.begin(), permutation.end(), 0);

    std::shuffle(permutation.begin(), permutation.end(), std::mt19937(std::random_device()()));

    std::vector<std::vector<unsigned>> table(size, std::vector<unsigned>(size));
    for (unsigned col = 0; col < size; ++col) {
        for (unsigned row = 0; row < size; ++row) {
            table[col][row] = permutation[col * size + row];
        }
    }
    return Board(std::move(table));
}

std::size_t Board::size() const
{
    return m_data.size();
}

std::string Board::to_string() const
{
    std::string result = "";
    for (unsigned col = 0; col < size(); ++col) {
        result += std::to_string(m_data[col][0]);
        for (unsigned row = 1; row < size(); ++row) {
            result += "\t" + std::to_string(m_data[col][row]);
        }
        result += "\n";
    }
    return result;
}

bool Board::is_goal() const
{
    return hamming() == 0;
}

bool Board::is_solvable() const
{
    if (is_goal()) {
        return true;
    }
    unsigned cnt = 0;
    for (unsigned col1 = 0; col1 < size(); ++col1) {
        for (unsigned row1 = 0; row1 < size(); ++row1) {
            for (unsigned col2 = 0; col2 <= col1; ++col2) {
                for (unsigned row2 = 0; (col2 < col1 && row2 < size()) || (row2 < row1); ++row2) {
                    if (m_data[col1][row1] < m_data[col2][row2] && m_data[col1][row1] && m_data[col2][row2]) {
                        ++cnt;
                    }
                }
            }
        }
    }
    return (size() % 2 == 1) ? (cnt % 2 == 0) : (cnt % 2 != m_empty.first % 2);
}

unsigned Board::hamming() const
{
    if (size() == 0) {
        return 0;
    }
    unsigned hamm_value = 0;
    for (unsigned col = 0; col < size(); ++col) {
        for (unsigned row = 0; row < size(); ++row) {
            if (m_data[col][row] != (col * size() + row + 1) % (size() * size())) {
                ++hamm_value;
            }
        }
    }
    return hamm_value;
}

unsigned Board::manhattan() const
{
    int manh_value = 0;
    for (unsigned col = 0; col < size(); ++col) {
        for (unsigned row = 0; row < size(); ++row) {
            if (m_data[col][row] != 0) {
                int x = (m_data[col][row] - 1) / size() - col;
                manh_value += abs(x);
                int y = (m_data[col][row] - 1) % size() - row;
                manh_value += abs(y);
            }
        }
    }
    return manh_value;
}

unsigned Board::hash() const
{
    return hash_value;
}

Board Board::make_move(const unsigned col, const unsigned row) const
{
    auto result = m_data;
    std::swap(result[m_empty.first][m_empty.second], result[col][row]);
    return Board(result);
}

std::vector<Board> Board::next_moves() const
{
    std::vector<Board> moves;
    if (m_empty.first > 0) {
        moves.push_back(make_move(m_empty.first - 1, m_empty.second));
    }
    if (m_empty.second > 0) {
        moves.push_back(make_move(m_empty.first, m_empty.second - 1));
    }
    if (m_empty.first + 1 < size()) {
        moves.push_back(make_move(m_empty.first + 1, m_empty.second));
    }
    if (m_empty.second + 1 < size()) {
        moves.push_back(make_move(m_empty.first, m_empty.second + 1));
    }
    return moves;
}

const std::vector<unsigned> & Board::operator[](const unsigned col) const
{
    return m_data[col];
}

bool operator==(const Board & lhs, const Board & rhs)
{
    return lhs.hash() == rhs.hash() || lhs.m_data == rhs.m_data;
}

bool operator!=(const Board & lhs, const Board & rhs)
{
    return lhs.hash() != rhs.hash() || lhs.m_data != rhs.m_data;
}

std::ostream & operator<<(std::ostream & out, const Board & board)
{
    return out << board.to_string();
}
