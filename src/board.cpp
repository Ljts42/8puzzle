#include "board.h"

#define distance(a, b) (a > b) ? a - b : b - a

Board::Board(const std::vector<std::vector<unsigned>> & data)
    : m_data(data)
{
    calc_hash();
    for (unsigned col = 0; col < data.size(); ++col) {
        for (unsigned row = 0; row < data.size(); ++row) {
            if (data[col][row] == 0) {
                m_empty = std::make_pair(col, row);
                return;
            }
        }
    }
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
    return Board(goal);
}

Board Board::create_random(const unsigned size)
{
    if (size == 0) {
        return Board();
    }

    std::vector<unsigned> permutation(size * size, 0);
    std::iota(permutation.begin(), permutation.end() - 1, 1);

    std::shuffle(permutation.begin(), permutation.end(), random_struct().get_rand());

    std::vector<std::vector<unsigned>> table(size, std::vector<unsigned>(size));
    for (unsigned col = 0; col < size; ++col) {
        for (unsigned row = 0; row < size; ++row) {
            table[col][row] = permutation[col * size + row];
        }
    }
    return Board(table);
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
    return (1 & (cnt ^ (m_empty.first | size()))) != 0u;
}

unsigned Board::hamming() const
{
    if (size() == 0) {
        return 0;
    }
    unsigned hamm_value = 0;
    for (unsigned col = 0; col < size(); ++col) {
        for (unsigned row = 0; row < size(); ++row) {
            if (m_data[col][row] != col * size() + row + 1) {
                ++hamm_value;
            }
        }
    }
    return hamm_value - static_cast<unsigned>(m_data[size() - 1][size() - 1] == 0);
}

unsigned Board::manhattan() const
{
    int manh_value = 0;
    for (unsigned col = 0; col < size(); ++col) {
        for (unsigned row = 0; row < size(); ++row) {
            if (m_data[col][row] != 0) {
                manh_value += distance((m_data[col][row] - 1) / size(), col);
                manh_value += distance((m_data[col][row] - 1) % size(), row);
            }
        }
    }
    return manh_value;
}

void Board::calc_hash()
{
    for (unsigned col = 0; col < size(); ++col) {
        for (unsigned row = 0; row < size(); ++row) {
            hash_value = hash_value * 31 + m_data[col][row];
        }
    }
}

unsigned Board::hash() const
{
    return hash_value;
}

Board Board::make_move(const unsigned col, const unsigned row) const
{
    auto result(m_data);
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
