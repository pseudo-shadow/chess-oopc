#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cctype>
#include <cmath>
#include <sstream> 
class ChessPiece {
protected:
    char symbol;
    bool isWhite;

public:
    ChessPiece(char sym, bool white) : symbol(sym), isWhite(white) {}
    virtual ~ChessPiece() = default;

    char getSymbol() const { return symbol; }
    bool getIsWhite() const { return isWhite; }

    virtual bool isValidMove(int fromX, int fromY, int toX, int toY, const std::vector<std::vector<ChessPiece*>>& board) const = 0;
    
    bool isPathClear(int fromX, int fromY, int toX, int toY, const std::vector<std::vector<ChessPiece*>>& board) const {
        int dx = (toX > fromX) ? 1 : ((toX < fromX) ? -1 : 0);
        int dy = (toY > fromY) ? 1 : ((toY < fromY) ? -1 : 0);
        
        int x = fromX + dx;
        int y = fromY + dy;
        
        while (x != toX || y != toY) {
            if (board[y][x] != nullptr) {
                return false;
            }
            x += dx;
            y += dy;
        }
        
        return true;
    }
};

class Pawn : public ChessPiece {
public:
    Pawn(bool white) : ChessPiece(white ? 'P' : 'p', white) {}

    bool isValidMove(int fromX, int fromY, int toX, int toY, const std::vector<std::vector<ChessPiece*>>& board) const override {
        int direction = isWhite ? -1 : 1;
        int startRow = isWhite ? 6 : 1;
        
        if (fromX == toX && toY == fromY + direction && board[toY][toX] == nullptr) {
            return true;
        }
        
        if (fromX == toX && fromY == startRow && toY == fromY + 2 * direction && 
            board[toY][toX] == nullptr && board[fromY + direction][fromX] == nullptr) {
            return true;
        }
        
        if ((toX == fromX - 1 || toX == fromX + 1) && toY == fromY + direction && 
            board[toY][toX] != nullptr && board[toY][toX]->getIsWhite() != isWhite) {
            return true;
        }
        
        return false;
    }
};

class Rook : public ChessPiece {
public:
    Rook(bool white) : ChessPiece(white ? 'R' : 'r', white) {}

    bool isValidMove(int fromX, int fromY, int toX, int toY, const std::vector<std::vector<ChessPiece*>>& board) const override {
        
        if (fromX != toX && fromY != toY) {
            return false;
        }
        
        return isPathClear(fromX, fromY, toX, toY, board);
    }
};

class Knight : public ChessPiece {
public:
    Knight(bool white) : ChessPiece(white ? 'N' : 'n', white) {}

    bool isValidMove(int fromX, int fromY, int toX, int toY, const std::vector<std::vector<ChessPiece*>>& board) const override {
        
        int dx = std::abs(toX - fromX);
        int dy = std::abs(toY - fromY);
        
        return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
    }
};

class Bishop : public ChessPiece {
public:
    Bishop(bool white) : ChessPiece(white ? 'B' : 'b', white) {}

    bool isValidMove(int fromX, int fromY, int toX, int toY, const std::vector<std::vector<ChessPiece*>>& board) const override {
        
        if (std::abs(toX - fromX) != std::abs(toY - fromY)) {
            return false;
        }
        
        return isPathClear(fromX, fromY, toX, toY, board);
    }
};

class Queen : public ChessPiece {
public:
    Queen(bool white) : ChessPiece(white ? 'Q' : 'q', white) {}

    bool isValidMove(int fromX, int fromY, int toX, int toY, const std::vector<std::vector<ChessPiece*>>& board) const override {
        
        bool isDiagonal = std::abs(toX - fromX) == std::abs(toY - fromY);
        bool isStraight = fromX == toX || fromY == toY;
        
        if (!isDiagonal && !isStraight) {
            return false;
        }
        
        return isPathClear(fromX, fromY, toX, toY, board);
    }
};

class King : public ChessPiece {
public:
    King(bool white) : ChessPiece(white ? 'K' : 'k', white) {}

    bool isValidMove(int fromX, int fromY, int toX, int toY, const std::vector<std::vector<ChessPiece*>>& board) const override {
        
        int dx = std::abs(toX - fromX);
        int dy = std::abs(toY - fromY);
        
        return dx <= 1 && dy <= 1 && (dx != 0 || dy != 0);
    }
};

class ChessBoard {
private:
    std::vector<std::vector<ChessPiece*>> board;
    bool whiteToMove;
    std::map<std::string, std::pair<int, int>> algebraicToCoords;
    std::map<std::pair<int, int>, std::string> coordsToAlgebraic;

    void setupAlgebraicNotation() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                std::string notation = std::string(1, 'a' + j) + std::string(1, '8' - i);
                algebraicToCoords[notation] = {j, i};
                coordsToAlgebraic[std::make_pair(j, i)] = notation;
            }
        }
    }

    bool isValidCoordinate(int x, int y) const {
        return x >= 0 && x < 8 && y >= 0 && y < 8;
    }

public:
    ChessBoard() : whiteToMove(true) {
        board.resize(8, std::vector<ChessPiece*>(8, nullptr));
        setupAlgebraicNotation();
        
        for (int i = 0; i < 8; ++i) {
            board[1][i] = new Pawn(false); 
            board[6][i] = new Pawn(true);  
        }
        
        board[0][0] = new Rook(false);
        board[0][7] = new Rook(false);
        board[7][0] = new Rook(true);
        board[7][7] = new Rook(true);
        
        board[0][1] = new Knight(false);
        board[0][6] = new Knight(false);
        board[7][1] = new Knight(true);
        board[7][6] = new Knight(true);
        
        board[0][2] = new Bishop(false);
        board[0][5] = new Bishop(false);
        board[7][2] = new Bishop(true);
        board[7][5] = new Bishop(true);
        
        board[0][3] = new Queen(false);
        board[7][3] = new Queen(true);
        
        board[0][4] = new King(false);
        board[7][4] = new King(true);
    }
    
    ~ChessBoard() {
        for (auto& row : board) {
            for (auto& piece : row) {
                delete piece;
                piece = nullptr; 
            }
        }
    }
    
    void display() const {
        std::cout << "\n   a b c d e f g h\n";
        std::cout << "  +-----------------+\n";
        
        for (int i = 0; i < 8; ++i) {
            std::cout << 8 - i << " |";
            
            for (int j = 0; j < 8; ++j) {
                if (board[i][j] == nullptr) {
                    std::cout << ((i + j) % 2 == 0 ? "." : " ");
                } else {
                    std::cout << board[i][j]->getSymbol();
                }
                std::cout << " ";
            }
            
            std::cout << "| " << 8 - i << "\n";
        }
        
        std::cout << "  +-----------------+\n";
        std::cout << "   a b c d e f g h\n\n";
        
        std::cout << (whiteToMove ? "White" : "Black") << " to move\n";
    }
    
    bool makeMove(const std::string& from, const std::string& to) {
        if (algebraicToCoords.find(from) == algebraicToCoords.end() || 
            algebraicToCoords.find(to) == algebraicToCoords.end()) {
            std::cout << "Invalid notation. Please use algebraic notation (e.g., e2 to e4).\n";
            return false;
        }
        
        int fromX = algebraicToCoords[from].first;
        int fromY = algebraicToCoords[from].second;
        int toX = algebraicToCoords[to].first;
        int toY = algebraicToCoords[to].second;
        
        // Validate coordinates
        if (!isValidCoordinate(fromX, fromY) || !isValidCoordinate(toX, toY)) {
            std::cout << "Invalid coordinates.\n";
            return false;
        }
        
        // Check if there is a piece at the starting position
        if (board[fromY][fromX] == nullptr) {
            std::cout << "No piece at position " << from << ".\n";
            return false;
        }
        
        // Check if it's the correct player's turn
        if (board[fromY][fromX]->getIsWhite() != whiteToMove) {
            std::cout << "It's " << (whiteToMove ? "White" : "Black") << "'s turn.\n";
            return false;
        }
        
        // Check if the destination has a piece of the same color
        if (board[toY][toX] != nullptr && board[toY][toX]->getIsWhite() == board[fromY][fromX]->getIsWhite()) {
            std::cout << "Cannot capture your own piece.\n";
            return false;
        }
        
        // Check if the move is valid for the piece
        if (!board[fromY][fromX]->isValidMove(fromX, fromY, toX, toY, board)) {
            std::cout << "Invalid move for " << board[fromY][fromX]->getSymbol() << ".\n";
            return false;
        }
        
        // Perform the move
        delete board[toY][toX]; // Delete the captured piece (if any)
        board[toY][toX] = board[fromY][fromX];
        board[fromY][fromX] = nullptr;
        
        // Switch turns
        whiteToMove = !whiteToMove;
        
        return true;
    }
    
    bool isGameOver() const {
        // Simplified version - just checking if kings are present
        bool whiteKingExists = false;
        bool blackKingExists = false;
        
        for (const auto& row : board) {
            for (const auto& piece : row) {
                if (piece != nullptr) {
                    if (piece->getSymbol() == 'K') whiteKingExists = true;
                    if (piece->getSymbol() == 'k') blackKingExists = true;
                }
            }
        }
        
        return !whiteKingExists || !blackKingExists;
    }
};

int main() {
    std::cout << "========== C++ Chess Game ==========\n";
    std::cout << "Enter moves in algebraic notation (e.g., e2 e4)\n";
    std::cout << "Enter 'quit' to exit\n";
    
    ChessBoard board;
    std::string input, from, to;
    
    while (!board.isGameOver()) {
        board.display();
        
        std::cout << "Enter move: ";
        std::getline(std::cin, input);
        
        if (input == "quit") {
            break;
        }
        
        // Parse input - expecting format like "e2 e4"
        std::istringstream iss(input);
        if (!(iss >> from >> to)) {
            std::cout << "Invalid input format. Use 'from to' (e.g., e2 e4).\n";
            continue;
        }
        
        // Convert input to lowercase for consistency
        for (char& c : from) c = std::tolower(c);
        for (char& c : to) c = std::tolower(c);
        
        if (!board.makeMove(from, to)) {
            std::cout << "Move failed. Try again.\n";
        }
    }
    
    if (board.isGameOver()) {
        board.display();
        std::cout << "Game over!\n";
    }
    
    std::cout << "Thanks for playing!\n";
    
    return 0;
}