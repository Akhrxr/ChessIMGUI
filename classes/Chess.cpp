#include "Chess.h"

const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;
const int DirectionOffsets[] = {8, -8, 1, -1, 9, -7, -9, 7}; // North, South, East, West, and diagonals
int NumSquaresToEdge[64][8]; // 8 directions per square

Chess::Chess()
{
}

Chess::~Chess()
{
}

//
// make a chess piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    return bit;
}

void Chess::setUpBoard()
{
    initializeNumSquaresToEdge();
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    //
    // we want white to be at the bottom of the screen so we need to reverse the board
    //
    char piece[2];
    piece[1] = 0;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            ImVec2 position((float)(pieceSize * x + pieceSize), (float)(pieceSize * (_gameOptions.rowY - y) + pieceSize));
            _grid[y][x].initHolder(position, "boardsquare.png", x, y);
            _grid[y][x].setGameTag(0);
            piece[0] = bitToPieceNotation(y,x);
            _grid[y][x].setNotation(piece);
        }
    }

    //lambda function so I don't have to keep writing setPosition, setParent, and setGameTag for everything.
    auto placePiece = [&](int y, int x, int player, ChessPiece type) {
        Bit* bit = PieceForPlayer(player, type);
        bit->setPosition(_grid[y][x].getPosition());
        bit->setParent(&_grid[y][x]);
        bit->setGameTag(type);
        _grid[y][x].setBit(bit);
    };

    //Black
    placePiece(7, 0, 1, Rook);   
    placePiece(7, 1, 1, Knight); 
    placePiece(7, 2, 1, Bishop);  
    placePiece(7, 3, 1, Queen);   
    placePiece(7, 4, 1, King);   
    placePiece(7, 5, 1, Bishop); 
    placePiece(7, 6, 1, Knight); 
    placePiece(7, 7, 1, Rook);    
    for (int i = 0; i < 8; i++) {
        placePiece(6, i, 1, Pawn);  // Row 7 A8 to H8
    }

    //White
    placePiece(0, 0, 0, Rook);  
    placePiece(0, 1, 0, Knight);  
    placePiece(0, 2, 0, Bishop);  
    placePiece(0, 3, 0, Queen);
    placePiece(0, 4, 0, King);   
    placePiece(0, 5, 0, Bishop);  
    placePiece(0, 6, 0, Knight); 
    placePiece(0, 7, 0, Rook);  
    for (int i = 0; i < 8; i++) {
        placePiece(1, i, 0, Pawn);  // Row 2 A2 to H2
    }
}

//
bool Chess::actionForEmptyHolder(BitHolder &holder)
{
 return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // Check if the square has a piece
    if (!src.bit()) {
        return false; // No piece to move
    }

    // Check if the piece belongs to the current player
    if (!src.canDragBit(&bit)) {
        return false; // Not the current player's piece
    }

    Player* pieceOwner = bit.getOwner();
    Player* currentPlayer = getCurrentPlayer();
    if (pieceOwner != currentPlayer) {
        return false; // Not the current player's turn
    }


    return true; // Piece can initiate a move
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    int srcIndex = findIndexInGrid(src);
    int dstIndex = findIndexInGrid(dst);

    ChessPiece pieceType = static_cast<ChessPiece>(bit.gameTag());
    std::vector<int> validMoves;

    switch (pieceType) {
        case Rook:
            validMoves = generateSlidingMoves(srcIndex, {0, 1, 2, 3}); // N, S, E, W
            break;
        case Bishop:
            validMoves = generateSlidingMoves(srcIndex, {4, 5, 6, 7}); // Diagonals
            break;
        case Queen:
            validMoves = generateSlidingMoves(srcIndex, {0, 1, 2, 3, 4, 5, 6, 7}); // All directions
            break;
        case King:
            validMoves = generateNonSlidingMoves(srcIndex, {8, -8, 1, -1, 7, -7, 9, -9}); // All directions
            break;
        case Knight:
            validMoves = generateNonSlidingMoves(srcIndex, {15, 17, -15, -17, 10, -10, 6, -6}); // Knight moves
            break;
        case Pawn:
            validMoves = generatePawnMoves(srcIndex, bit.getOwner()->playerNumber());
            break;
        default:
            return false;
    }

    return std::find(validMoves.begin(), validMoves.end(), dstIndex) != validMoves.end();
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    src.setBit(nullptr);

    // If the destination holder has an opponent's piece, capture it
    Bit* dstBit = dst.bit();
    if (dstBit && dstBit->getOwner() != bit.getOwner()) {
        std::cout << "Captured piece with gameTag: " << dstBit->gameTag() << std::endl;
        dst.destroyBit(); // Remove the opponent's piece
    }

    // Place the piece in the destination holder
    src.setBit(nullptr); // Clear the source square
    dst.setBit(&bit);
    bit.setParent(&dst);
    bit.moveTo(dst.getPosition());

    // End the turn after a successful move
    endTurn();
    std::cout << "Turn ended. Next player is: " << getCurrentPlayer()->playerNumber() << std::endl;
}

//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
}

Player* Chess::checkForWinner()
{
    // check to see if either player has won
    return nullptr;
}

bool Chess::checkForDraw()
{
    // check to see if the board is full
    return false;
}

//
// add a helper to Square so it returns out FEN chess notation in the form p for white pawn, K for black king, etc.
// this version is used from the top level board to record moves
//
const char Chess::bitToPieceNotation(int row, int column) const {
    if (row < 0 || row >= 8 || column < 0 || column >= 8) {
        return '0';
    }

    const char* wpieces = { "?PNBRQK" };
    const char* bpieces = { "?pnbrqk" };
    unsigned char notation = '0';
    Bit* bit = _grid[row][column].bit();
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag() & 127];
    } else {
        notation = '0';
    }
    return notation;
}

//
// state strings
//
std::string Chess::initialStateString()
{
    return stateString();
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString()
{
    std::string s;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            s += bitToPieceNotation(y, x);
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string &s)
{
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            int index = y * _gameOptions.rowX + x;
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                _grid[y][x].setBit(PieceForPlayer(playerNumber - 1, Pawn));
            } else {
                _grid[y][x].setBit(nullptr);
            }
        }
    }
}

//Since I couldn't figure out how to get the x and y values from _grid, but I also don't fully understand the bitboard strategy

int Chess::findIndexInGrid(const BitHolder& holder) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (&_grid[row][col] == &holder) {
                return row * 8 + col; // Return the 1D index
            }
        }
    }
    return -1; // Return -1 if not found (should not happen)
}

////////////MoveSets/////////////////
//Rook,Queen,Bishops use these
std::vector<int> Chess::generateSlidingMoves(int srcIndex, const std::vector<int>& directions) {
    std::vector<int> moves;

    for (int dir : directions) {
        int offset = DirectionOffsets[dir];
        int maxSquares = NumSquaresToEdge[srcIndex][dir];

        // Loop through each square in the specified direction
        for (int i = 1; i <= maxSquares; ++i) {
            int targetIndex = srcIndex + i * offset;

            // Ensure the target index is within bounds (0 to 63)
            if (targetIndex < 0 || targetIndex >= 64) {
                break; // Stop if out of bounds
            }

            // Check if the move wraps around the board horizontally
            int srcRow = srcIndex / 8;
            int targetRow = targetIndex / 8;
            if (std::abs((targetIndex % 8) - (srcIndex % 8)) > i) {
                break; // Stop if the move wraps around horizontally
            }

            // Access the target square from _grid
            int targetCol = targetIndex % 8;
            Bit* targetBit = _grid[targetRow][targetCol].bit();

            // If there's a piece on the target square
            if (targetBit) {
                // Check if it's an opponent's piece
                if (targetBit->getOwner() != _grid[srcRow][srcIndex % 8].bit()->getOwner()) {
                    moves.push_back(targetIndex); // Capture move
                }
                break; // Stop sliding in this direction
            }

            // Add the valid move
            moves.push_back(targetIndex);
        }
    }
    return moves;
}


//Knights and Kings
std::vector<int> Chess::generateNonSlidingMoves(int srcIndex, const std::vector<int>& offsets) {
    std::vector<int> moves;

    for (int offset : offsets) {
        int targetIndex = srcIndex + offset;

        // Ensure the target square is within bounds
        if (targetIndex < 0 || targetIndex >= 64) {
            continue;
        }

        int srcRow = srcIndex / 8;
        int targetRow = targetIndex / 8;

        // Check if the move wraps around the board horizontally
        if (std::abs((targetIndex % 8) - (srcIndex % 8)) > 2) {
            continue;
        }

        Bit* targetBit = _grid[targetIndex / 8][targetIndex % 8].bit();
        if (!targetBit || targetBit->getOwner() != _grid[srcRow][srcIndex % 8].bit()->getOwner()) {
            moves.push_back(targetIndex);
        }
    }

    return moves;
}
//Pawns
std::vector<int> Chess::generatePawnMoves(int srcIndex, int playerNumber) {
    std::vector<int> moves;
    int direction = (playerNumber == 0) ? 8 : -8; // White moves up, black moves down

    // Single square move forward
    int targetIndex = srcIndex + direction;
    if (targetIndex >= 0 && targetIndex < 64 && !_grid[targetIndex / 8][targetIndex % 8].bit()) {
        moves.push_back(targetIndex);

        // Double move on first turn
        if ((playerNumber == 0 && srcIndex / 8 == 1) || (playerNumber == 1 && srcIndex / 8 == 6)) {
            int doubleMoveIndex = srcIndex + 2 * direction;
            if (!_grid[doubleMoveIndex / 8][doubleMoveIndex % 8].bit()) {
                moves.push_back(doubleMoveIndex);
            }
        }
    }

    // Capture moves
    int captureOffsets[] = {direction + 1, direction - 1};
    for (int offset : captureOffsets) {
        targetIndex = srcIndex + offset;

        if (targetIndex < 0 || targetIndex >= 64) {
            continue;
        }

        Bit* targetBit = _grid[targetIndex / 8][targetIndex % 8].bit();
        if (targetBit && targetBit->getOwner() != _grid[srcIndex / 8][srcIndex % 8].bit()->getOwner()) {
            moves.push_back(targetIndex);
        }
    }

    return moves;
}

//
// this is the function that will be called by the AI
//
void Chess::updateAI() 
{
}










void Chess::initializeNumSquaresToEdge() {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int squareIndex = rank * 8 + file;

            // North (up the board)
            NumSquaresToEdge[squareIndex][0] = 7 - rank;
            // South (down the board)
            NumSquaresToEdge[squareIndex][1] = rank;
            // East (right)
            NumSquaresToEdge[squareIndex][2] = 7 - file;
            // West (left)
            NumSquaresToEdge[squareIndex][3] = file;
            // Northeast (up and to the right)
            NumSquaresToEdge[squareIndex][4] = std::min(7 - rank, 7 - file);
            // Northwest (up and to the left)
            NumSquaresToEdge[squareIndex][5] = std::min(7 - rank, file);
            // Southeast (down and to the right)
            NumSquaresToEdge[squareIndex][6] = std::min(rank, 7 - file);
            // Southwest (down and to the left)
            NumSquaresToEdge[squareIndex][7] = std::min(rank, file);
        }
    }
}