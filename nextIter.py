import sys

def get_neighbors(board, x, y):
    """Returns the number of live neighbors for cell (x, y)."""
    neighbors = [
        (-1, -1), (-1, 0), (-1, 1),
        (0, -1),          (0, 1),
        (1, -1), (1, 0), (1, 1)
    ]
    live_neighbors = 0
    for dx, dy in neighbors:
        nx, ny = x + dx, y + dy
        if 0 <= nx < len(board) and 0 <= ny < len(board[0]):
            live_neighbors += board[nx][ny]
    return live_neighbors

def next_iteration(board):
    """Generates the next iteration of Conway's Game of Life."""
    new_board = [[0 for _ in row] for row in board]  # Create an empty board for the new state
    for i in range(len(board)):
        for j in range(len(board[0])):
            live_neighbors = get_neighbors(board, i, j)
            if board[i][j] == 1:
                # Cell is alive: stays alive if it has 2 or 3 neighbors
                if live_neighbors == 2 or live_neighbors == 3:
                    new_board[i][j] = 1
            else:
                # Cell is dead: comes to life if it has exactly 3 neighbors
                if live_neighbors == 3:
                    new_board[i][j] = 1
    return new_board

def print_board(board):
    """Prints the board in the specified format."""
    for row in board:
        print(" ".join(map(str, row)))

def parse_input(input_str):
    """Converts the input string into a 2D list (board)."""
    return [list(map(int, line.split())) for line in input_str.strip().splitlines()]

def main():
    input_str = sys.stdin.read()  # Read the entire input from stdin
    board = parse_input(input_str)
    new_board = next_iteration(board)
    print_board(new_board)

if __name__ == "__main__":
    main()

