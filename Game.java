import java.util.Set;

public class Game {
    private boolean running = true;
    private StoneColor turn = StoneColor.BLACK;
    private StoneColor winner = StoneColor.EMPTY;
    private Board board;

    public Game () {
        board = new Board(9);
    }

    public StoneColor getCurrentTurn() { return turn; }

    public StoneColor getWinner() { return winner; }

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

    public void endGame() { running = false; }

    public Set<Position> getCaptures() { return board.getCatpured(); }

    public Set<Position> getAtarPositions() { return board.getAtari(); }

    public String getIntersectionSymbol(Position stone) { return board.get(stone).symbol(); }

    public boolean isRunning() { return running; }

    public int getBoardSize() { return board.getSize(); }

    public Position suggestMoveForCurrentPlayer() { return board.suggestMove(turn); }

    public void currentPlayerConcedes() {
        winner = turn.opponent();
        endGame();
    }
}
