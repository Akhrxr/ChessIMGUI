# Chess AI Implementation Project


In this version of the Chess project. I added the ability to castle, en passant, and promoting the pawn to a queen. 
There is also the capability of starting a game with a fen string, as well as the begginings to saving the game
    into a fen string.
I did this by first including checks to each of the new abilities added. For castling, it must
    check if either the king or rook (either side) has moved yet, and if there is empty space between the two pieces.
In order to promote to queen, once the pawn reaches the other side of the board, the piece is deleted and replaced by a queen piece.
For en passant, I added a check to see where the square that the pawn would have been if they have done a double move on their first move and nothing since.
    With this check the other player can step into that square (index) and take that pawn.



































![Chess Board](https://raw.githubusercontent.com/zaphodgjd/class-chess-123/main/chess/w_king.png)

## 🎯 Project Overview
This repository contains a skeleton implementation of a Chess AI engine written in C++. The project is designed to teach fundamental concepts of game AI, including board representation, move generation, and basic game tree search algorithms.

### 🎓 Educational Purpose
This project serves as a teaching tool for computer science students to understand:
- Game state representation
- Object-oriented design in C++
- Basic AI concepts in game playing
- Bitboard operations and chess piece movement
- FEN (Forsyth–Edwards Notation) for chess position representation

## 🔧 Technical Architecture

### Key Components
1. **Chess Class**: Core game logic implementation
   - Board state management
   - Move validation
   - Game state evaluation
   - AI player implementation

2. **Piece Representation**
   - Unique identifiers for each piece type
   - Sprite loading and rendering
   - Movement pattern definitions

3. **Board Management**
   - 8x8 grid representation
   - Piece positioning
   - Move history tracking
   - FEN notation support

## 🚀 Getting Started

### Prerequisites
- C++ compiler with C++11 support or higher
- Image loading library for piece sprites
- CMake 3.10 or higher

### Building the Project
```bash
mkdir build
cd build
cmake ..
make
```

### Running Tests
```bash
./chess_tests
```

## 📝 Implementation Details

### Current Features
- Basic board setup and initialization
- Piece movement validation framework
- FEN notation parsing and generation
- Sprite loading for chess pieces
- Player turn management

### Planned Features
- [ ] AI move generation
- [ ] Position evaluation
- [ ] Opening book integration
- [ ] Advanced search algorithms
- [ ] Game state persistence

## 🔍 Code Examples

### Piece Movement Validation
```cpp
bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
    // TODO: Implement piece-specific movement rules
    return false;
}
```

### FEN Notation Generation
```cpp
const char Chess::bitToPieceNotation(int row, int column) const {
    if (row < 0 || row >= 8 || column < 0 || column >= 8) {
        return '0';
    }
    // Implementation details for FEN notation
}
```

## 📚 Class Assignment Structure

### Phase 1: Board Setup
- Implement piece placement
- Setup initial board state
- Validate board representation

### Phase 2: Move Generation
- Implement basic piece movements
- Add move validation
- Implement special moves (castling, en passant)

### Phase 3: AI Implementation
- Develop position evaluation
- Implement minimax algorithm
- Add alpha-beta pruning
- Basic opening book

## 🤝 Contributing
Students are encouraged to:
1. Fork the repository
2. Create a feature branch
3. Implement assigned components
4. Submit their fork for review

## 🔒 Code Style and Standards
- Use consistent indentation (4 spaces)
- Follow C++ naming conventions
- Document all public methods
- Include unit tests for new features

## 📄 License
This project is licensed under the MIT License.

## 👥 Contributors
- [Akhror Rakhmatov] - Initial work

## 🙏 Acknowledgments
- Chess piece sprites from [Wikipedia](https://en.wikipedia.org/wiki/Chess_piece)
- Original game engine framework by [ocornut](https://github.com/ocornut/imgui)

---
*This README is part of an educational project and is intended to serve as an example of good documentation practices.*![alt text]

![Bishop](image.png)
![GameBoard](ScreenShots/Gameboard.png)
![KingMoves](ScreenShots/KingMoves.png)
![KnightMoves](ScreenShots/KnightMoves.png)
![PawnMoves](ScreenShots/PawnMoves.png)
![QueenMoves](ScreenShots/QueenMoves.png)
![RookMoves](ScreenShots/RookMoves.png)
