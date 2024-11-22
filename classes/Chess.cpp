#include "Chess.h"

int enPassantSquare = -1;
const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;
const int DirectionOffsets[] =  {8, -8, -1, 1, 7, 9, -9, -7}; // North, South, West, East, and NW, NE, SW, SE
int NumSquaresToEdge[64][8]; // 8 directions per square
bool whiteKingMoved = false;
bool whiteRookKingsideMoved = false;
bool whiteRookQueensideMoved = false;

bool blackKingMoved = false;
bool blackRookKingsideMoved = false;
bool blackRookQueensideMoved = false;

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
    /*
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
    }*/
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

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
            validMoves = generateSlidingMoves(srcIndex, Rook); // N, S, E, W
            break;
        case Bishop:
            validMoves = generateSlidingMoves(srcIndex, Bishop); // Diagonals
            break;
        case Queen:
            validMoves = generateSlidingMoves(srcIndex, Queen); // All directions
            break;
        case King:
            validMoves = generateNonSlidingMoves(srcIndex, {8, -8, 1, -1, 7, -7, 9, -9}); // All directions
            // Check for castling as a special case

            if (!whiteKingMoved && srcIndex == 4) {
                // White kingside castling
                if (!whiteRookKingsideMoved && dstIndex == 6) {
                    if (_grid[0][5].bit() == nullptr && _grid[0][6].bit() == nullptr) {
                        return true; // Kingside castling is valid
                    }
                }
                // White queenside castling
                if (!whiteRookQueensideMoved && dstIndex == 2) {
                    if (_grid[0][1].bit() == nullptr && _grid[0][2].bit() == nullptr && _grid[0][3].bit() == nullptr) {
                        return true; // Queenside castling is valid
                    }
                }
            }

            if (!blackKingMoved && srcIndex == 60) {
                // Black kingside castling
                if (!blackRookKingsideMoved && dstIndex == 62) {
                    if (_grid[7][5].bit() == nullptr && _grid[7][6].bit() == nullptr) {
                        return true; // Kingside castling is valid
                    }
                }
                // Black queenside castling
                if (!blackRookQueensideMoved && dstIndex == 58) {
                    if (_grid[7][1].bit() == nullptr && _grid[7][2].bit() == nullptr && _grid[7][3].bit() == nullptr) {
                        return true; // Queenside castling is valid
                    }
                }
            }

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

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) 
{
    int srcIndex = findIndexInGrid(src);
    int dstIndex = findIndexInGrid(dst);
    ChessPiece pieceType = static_cast<ChessPiece>(bit.gameTag());
    src.setBit(nullptr);
    // If the destination holder has an opponent's piece, capture it
    Bit* dstBit = dst.bit();
    if (dstBit && dstBit->getOwner() != bit.getOwner()) {
        dst.destroyBit(); // Remove the opponent's piece
    }

    if (pieceType == King) {
        handleCastling(srcIndex, dstIndex, bit.getOwner()->playerNumber());
        // Mark the king as moved
        if (bit.getOwner()->playerNumber() == 0) whiteKingMoved = true;
        else blackKingMoved = true;
    }

    if (pieceType == Rook) {
        // Mark the rook as moved
        if (srcIndex == 63) whiteRookKingsideMoved = true;  // White kingside rook
        if (srcIndex == 56) whiteRookQueensideMoved = true; // White queenside rook
        if (srcIndex == 7) blackRookKingsideMoved = true;   // Black kingside rook
        if (srcIndex == 0) blackRookQueensideMoved = true;  // Black queenside rook
    }
    if (pieceType == Pawn && std::abs(srcIndex - dstIndex) == 16) 
    {
        enPassantSquare = (srcIndex + dstIndex) / 2; // Set the square behind the pawn
    } else 
    {
        enPassantSquare = -1; // Reset if no pawn moved two spaces
    }

    if (pieceType == Pawn) {
        handlePawnPromotion(bit, dstIndex);
        if (_grid[srcIndex / 8][srcIndex % 8].bit()) 
        { 
            handleEnPassant(srcIndex, dstIndex, bit.getOwner()->playerNumber());
        }
    }

    // Place the piece in the destination holder
    src.setBit(nullptr); // Clear the source square
    dst.setBit(&bit);
    bit.setParent(&dst);
    bit.moveTo(dst.getPosition());

    std::cout << "FEN string parsed successfully. Current board setup:" << std::endl;
    std::cout << boardToFEN() << std::endl;
    // End the turn after a successful move
    endTurn();
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
        /*
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag() & 127];
    } else {
        notation = '0'; */
        int gameTag = bit->gameTag();

        // Validate gameTag range
        if (gameTag >= 1 && gameTag <= 6) {
            return (bit->getOwner()->playerNumber() == 0) ? wpieces[gameTag] : bpieces[gameTag];
        } else {
            std::cerr << "Invalid gameTag: " << gameTag << " at row: " << row << ", column: " << column << std::endl;
            return '?'; // Unknown piece
        }
    }
    return notation;
}

//Puts the Fen string into the board
void Chess::FENtoBoard(const std::string& fen) {
    int row = 7;
    int column = 0;

    for (char c : fen) {
        if (c == '/') {
            // Move to the next row
            row--;
            column = 0;
        } else if (isdigit(c)) {
            // Skip empty squares
            column += c - '0';
        } else {
            // Place the corresponding piece
            int player = isupper(c) ? 0 : 1; // Uppercase is White, Lowercase is Black
            ChessPiece piece;

            switch (tolower(c)) {
                case 'p': piece = Pawn; break;
                case 'r': piece = Rook; break;
                case 'n': piece = Knight; break;
                case 'b': piece = Bishop; break;
                case 'q': piece = Queen; break;
                case 'k': piece = King; break;
                default: continue;
            }

            if (row >= 0 && column < 8) {
                Bit* bit = PieceForPlayer(player, piece);
                bit->setPosition(_grid[row][column].getPosition());
                bit->setParent(&_grid[row][column]);
                bit->setGameTag(piece);
                _grid[row][column].setBit(bit);
            }

            column++; // Move to next column
        }

        if (row < 0) {
            break; // Stops from going past the board
        }
    }
}

//Turns the board to Fen Notation
std::string Chess::boardToFEN() {
    std::string fen;
    for (int row = 7; row >= 0; row--) {
        int emptyCount = 0;
        for (int col = 0; col < 8; col++) {
            Bit* bit = _grid[row][col].bit();
            if (bit) {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                char pieceChar = bitToPieceNotation(row, col);
                fen += pieceChar;
            } else {
                emptyCount++;
            }
        }
        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
        }
        if (row > 0) {
            fen += '/';
        }
    }
    return fen;
}

//Makes sure that the fen is not malformed or messed up in anyway
bool validateFEN(const std::string& fen) {
    int rowCount = 1, squareCount = 0;

    for (char c : fen) {
        if (c == '/') {
            rowCount++;
            if (squareCount != 8) return false;
            squareCount = 0;
        } else if (isdigit(c)) {
            squareCount += c - '0';
        } else if (strchr("pPnNbBrRqQkK", c)) {
            squareCount++;
        } else {
            return false; // Invalid character
        }
        if (squareCount > 8) return false;
    }
    return rowCount == 8 && squareCount == 8;
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
std::vector<int> Chess::generateSlidingMoves(int srcIndex, ChessPiece pieceType) {
    std::vector<int> moves;

    int srcRow = srcIndex / 8;
    int srcCol = srcIndex % 8;

    int startDirIndex = (pieceType == Bishop) ? 4 : 0;
    int endDirIndex = (pieceType == Rook) ? 4 : 8;

    for (int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++) {
        int maxSquares = NumSquaresToEdge[srcIndex][directionIndex];

        for (int n = 0; n < maxSquares; n++) {
            int targetIndex = srcIndex + DirectionOffsets[directionIndex] * (n+1);

            // Directly use NumSquaresToEdge, no additional bounds check needed
            Bit* targetBit = _grid[targetIndex / 8][targetIndex % 8].bit();

            // If there's a piece on the target square
            if (targetBit) {
                // Check if it's an opponent's piece
                if (targetBit->getOwner() != _grid[srcRow][srcCol].bit()->getOwner()) {
                    moves.push_back(targetIndex); // Capture move
                }
                break; // Stop sliding in this direction after encountering a piece
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
        if (targetBit == nullptr && enPassantSquare == targetIndex) {
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

void Chess::handleCastling(int srcIndex, int dstIndex, int playerNumber) {
    if (playerNumber == 0) { // White
        if (srcIndex == 4 && dstIndex == 6) { // Kingside castling
            Bit* rook = _grid[0][7].bit();
            _grid[0][5].setBit(rook);
            rook->setParent(&_grid[0][5]);
            rook->moveTo(_grid[0][5].getPosition());
            _grid[0][7].setBit(nullptr); // Clear original rook position
        } else if (srcIndex == 4 && dstIndex == 2) { // Queenside castling
            Bit* rook = _grid[0][0].bit();
            _grid[0][3].setBit(rook);
            rook->setParent(&_grid[0][3]);
            rook->moveTo(_grid[0][3].getPosition());
            _grid[0][0].setBit(nullptr); // Clear original rook position
        }
    } else { // Black
        if (srcIndex == 60 && dstIndex == 62) { // Kingside castling
            Bit* rook = _grid[7][7].bit();
            _grid[7][5].setBit(rook);
            rook->setParent(&_grid[7][5]);
            rook->moveTo(_grid[7][5].getPosition());
            _grid[7][7].setBit(nullptr); // Clear original rook position
        } else if (srcIndex == 60 && dstIndex == 58) { // Queenside castling
            Bit* rook = _grid[7][0].bit();
            _grid[7][3].setBit(rook);
            rook->setParent(&_grid[7][3]);
            rook->moveTo(_grid[7][3].getPosition());
            _grid[7][0].setBit(nullptr); // Clear original rook position
        }
    }
}

void Chess::handlePawnPromotion(Bit &bit, int dstIndex) {
    if (bit.gameTag() == Pawn) {
        int row = dstIndex / 8;
        int col = dstIndex % 8;
        int playerNumber = bit.getOwner()->playerNumber();

        if ((playerNumber == 0 && row == 7) || (playerNumber == 1 && row == 0)) 
            {
                bit.setGameTag(Queen); // Update the gameTag to Queen
                bit.LoadTextureFromFile(playerNumber == 0 ? "chess/w_queen.png" : "chess/b_queen.png"); // Update the texture
                bit.setOwner(getPlayerAt(playerNumber));
                bit.setSize(pieceSize, pieceSize);
                std::cout << "Pawn promoted to Queen at row: " << row << ", col: " << col << std::endl;
            }
    }
}

void Chess::handleEnPassant(int srcIndex, int dstIndex, int playerNumber) {
    if (abs(srcIndex - dstIndex) == 9 || abs(srcIndex - dstIndex) == 7) {
        // Check if it was an en passant capture
        int capturedPawnIndex = (playerNumber == 0) ? dstIndex + 8 : dstIndex - 8;
        Bit* capturedPawn = _grid[capturedPawnIndex / 8][capturedPawnIndex % 8].bit();

        if (capturedPawn && capturedPawn->gameTag() == Pawn) {
            // Remove the captured pawn
            _grid[capturedPawnIndex / 8][capturedPawnIndex % 8].setBit(nullptr);
        }
    }
}


void Chess::initializeNumSquaresToEdge() {
    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {

            int numNorth = 7 - rank;
            int numSouth = rank;
            int numWest = file;
            int numEast = 7 - file;

            int squareIndex = rank * 8 + file;

            NumSquaresToEdge[squareIndex][0] = numNorth;                      // North
            NumSquaresToEdge[squareIndex][1] = numSouth;                      // South
            NumSquaresToEdge[squareIndex][2] = numWest;                       // West
            NumSquaresToEdge[squareIndex][3] = numEast;                       // East
            NumSquaresToEdge[squareIndex][4] = std::min(numNorth, numWest);   // Northwest
            NumSquaresToEdge[squareIndex][5] = std::min(numNorth, numEast);   // Northeast
            NumSquaresToEdge[squareIndex][6] = std::min(numSouth, numWest);   // Southwest
            NumSquaresToEdge[squareIndex][7] = std::min(numSouth, numEast);   // Southeast
            };
    }
}