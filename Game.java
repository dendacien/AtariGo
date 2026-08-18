public class Game {
    private boolean running = true;
    StoneColor turn = StoneColor.BLACK;
    StoneColor winner = StoneColor.EMPTY;
    Board board;

    public Game () {
        board = new Board(9);
    }

    public StoneColor getCurrentTurn() { return turn; }

    public StoneColor getWinner() {
        return winner;
    }

    public MoveResult tryPlacement(Position stone) {
        if (!running) return MoveResult.illegal("Game is already over.");

        MoveResult result = board.placeStone(stone, turn);
        if (!result.isLegal()) return result;

        if (result.isCapture()) {
            winner = result.getWinner();
            return result;
        }

        turn = turn.opponent();
        return result;
    }

    public void endGame() {
        running = false;
    }
    public boolean isRunning() { return running; }

}
