#pragma once

#include <algorithm>
#include <random>
#include <string>
#include <vector>

class Board
{
public:
    Board() = default;
    explicit Board(const std::vector<std::vector<unsigned>> &);

    static Board create_goal(unsigned);
    static Board create_random(unsigned);

    std::size_t size() const;
    std::string to_string() const;

    bool is_goal() const;
    bool is_solvable() const;

    unsigned hamming() const;
    unsigned manhattan() const;
    unsigned hash() const;

    std::vector<Board> next_moves() const;

    Board & operator=(const Board &) = default;
    const std::vector<unsigned> & operator[](unsigned) const;

    friend bool operator==(const Board &, const Board &);
    friend bool operator!=(const Board &, const Board &);
    friend std::ostream & operator<<(std::ostream &, const Board &);

private:
    void calc_hash();
    Board make_move(unsigned, unsigned) const;

    std::vector<std::vector<unsigned>> m_data;
    std::pair<unsigned, unsigned> m_empty;
    unsigned hash_value = 0;
};
