package test;

import src.Board;
import src.Game;
import src.MoveResult;
import src.Position;
import src.StoneColor;

public final class GameTest {
    private int assertions;

    public static void main(String[] args) {
        GameTest test = new GameTest();
        test.rejectsInvalidAndOccupiedMoves();
        test.switchesTurnsAfterLegalMove();
        test.rejectsMovesAfterGameEnds();
        test.recordsCaptures();
        System.out.println("Passed " + test.assertions + " assertions.");
    }

    private void rejectsInvalidAndOccupiedMoves() {
        Board board = new Board(9);

        MoveResult outOfBounds = board.placeStone(new Position(-1, 0), StoneColor.BLACK);
        assertFalse(outOfBounds.isLegal(), "out-of-bounds moves must be illegal");

        MoveResult occupied = board.placeStone(new Position(3, 4), StoneColor.BLACK);
        assertFalse(occupied.isLegal(), "occupied positions must be illegal");
    }

    private void switchesTurnsAfterLegalMove() {
        Game game = new Game();

        MoveResult result = game.tryPlacement(new Position(0, 0));

        assertTrue(result.isLegal(), "an empty corner should accept a move");
        assertFalse(result.isCapture(), "the opening move should not capture");
        assertEquals(StoneColor.WHITE, game.getCurrentTurn(), "a legal move should switch turns");
    }

    private void rejectsMovesAfterGameEnds() {
        Game game = new Game();
        game.endGame();

        MoveResult result = game.tryPlacement(new Position(0, 0));

        assertFalse(result.isLegal(), "ended games must reject moves");
        assertEquals("Game is already over.", result.getMessage(), "ended-game message should explain the rejection");
    }

    private void recordsCaptures() {
        Board board = new Board(9);

        assertTrue(board.placeStone(new Position(2, 4), StoneColor.BLACK).isLegal(), "setup move should be legal");
        MoveResult capture = board.placeStone(new Position(3, 3), StoneColor.BLACK);

        assertTrue(capture.isLegal(), "capture move should be legal");
        assertTrue(capture.isCapture(), "surrounding a chain's last liberty should capture it");
        assertEquals(StoneColor.BLACK, capture.getWinner(), "capturing player should win Atari Go");
        assertTrue(board.getCatpured().contains(new Position(3, 4)), "captured chain should include the surrounded stone");
    }

    private void assertTrue(boolean condition, String message) {
        assertions++;
        if (!condition) {
            throw new AssertionError(message);
        }
    }

    private void assertFalse(boolean condition, String message) {
        assertTrue(!condition, message);
    }

    private void assertEquals(Object expected, Object actual, String message) {
        assertions++;
        if (!expected.equals(actual)) {
            throw new AssertionError(message + ": expected " + expected + " but got " + actual);
        }
    }
}
