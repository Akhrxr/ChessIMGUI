#pragma once
#include "Game.h"
#include "ChessSquare.h"

const int pieceSize = 64;

enum ChessPiece {
    NoPiece = 0,
    Pawn = 1,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

//
// the main game class
//
class Chess : public Game
{
public:
    Chess();
    ~Chess();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder& holder) override;
    bool        canBitMoveFrom(Bit& bit, BitHolder& src) override;
    bool        canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void        bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

    void        stopGame() override;
    BitHolder& getHolderAt(const int x, const int y) override { return _grid[y][x]; }

	void        updateAI() override;
    bool        gameHasAI() override { return true; }

    std::vector<int> generateSlidingMoves(int srcIndex, ChessPiece pieceType);
    std::vector<int> generateNonSlidingMoves(int srcIndex, const std::vector<int>& offsets);
    std::vector<int> generatePawnMoves(int srcIndex, int playerNumber);

    int     findIndexInGrid(const BitHolder& holder);
    void    initializeNumSquaresToEdge();

    void    FENtoBoard(const std::string& fen);
    std::string boardToFEN();

    void    handleCastling(int srcIndex, int dstIndex, int playerNumber);
    void    handlePawnPromotion(Bit &bit, int dstIndex);
    void    handleEnPassant(int srcIndex, int dstIndex, int playerNumber);

private:
    Bit *       PieceForPlayer(const int playerNumber, ChessPiece piece);
    const char  bitToPieceNotation(int row, int column) const;

    ChessSquare      _grid[8][8];
};

