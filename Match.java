import java.util.Scanner;
import java.util.Set;

public class Match {
    private final Player slatePlayer, shellPlayer;
    private final Scanner input;
    private Game currentGame;
    private final String RED = "\u001B[31m";
    private final String YELLOW = "\u001B[33m";
    private final String RESET = "\u001B[0m";

    public Match(Scanner input, Player player1, Player player2) {
        this.input = input;
        this.slatePlayer = player1;
        this.shellPlayer = player2;
        startGame();
    }

    private void startGame() {
        currentGame = new Game();
        while(currentGame.isRunning()) {
            System.out.println("Current player is " + getPlayerByColor(currentGame.getCurrentTurn()).getName());
            gameUI();
        }
        endScore(currentGame.getWinner());
    }

    private void gameUI() {
        int size = currentGame.board.getSize();
        printBoard(size);
        MoveResult result = MoveResult.illegal("");
        int captureRow;
        int captureCol;
        while (!result.getMessage().equals("OK")) {
            captureCol = 0;
            captureRow = 0;
            if (!result.getMessage().isBlank()) System.out.println(result.getMessage());
            System.out.print("Move: ");
            String captureString = input.nextLine();
            if (captureString.contains("q") || captureString.contains("Q")) {
                currentGame.endGame();
                return;
            }
            if (captureString.length() != 2) {
                result = MoveResult.illegal("Input too long, must only be column number and row letter");
            } else {
                for(char c : captureString.toCharArray()) {
                    if (Character.isDigit(c)) {
                        captureCol = c - '0';
                    } else if (Character.isAlphabetic(c)){
                        captureRow = Character.toLowerCase(c) - 'a' + 1;
                    }
                }
                if (captureCol == 0 || captureRow == 0) {
                    result = MoveResult.illegal("Invalid input, must be one number and one letter");
                } else {
                    Position placement = new Position(captureRow - 1, captureCol -1);
                    result = currentGame.tryPlacement(placement);
                    if (result.isCapture()) {
                        printBoard(size);
                        System.out.println(result.getMessage());
                        currentGame.endGame();
                        return;
                    }
                }
            }
        }
    }

    private void printBoard(int size) {
        printHeader(size);
        for (int row = 0; row < size; row++) {
            printRow(row, size);
            if (row < size - 1) printSeparator(size);
        }
    }

    private void printHeader(int size) {
        for (int i = 1; i <= size; i++) {
            System.out.printf("   %d", i);
        }
        System.out.println();
    }

    private void printRow(int row, int size) {
        System.out.printf("%c ", 'A' + row);
        Set<Position> captures = currentGame.board.getCatpured();
        Set<Position> ataris = currentGame.board.getAtari();
        for (int col = 0; col < size; col++) {
            Position stone = new Position(row, col);
            String intersection = currentGame.board.get(stone).symbol;
            if (captures.contains(stone)) intersection = RED + intersection + RESET;
            if (ataris.contains(stone)) intersection = YELLOW + intersection + RESET;
            if (col + 1 != size) intersection = intersection + "-";
            System.out.printf("%s", intersection);
        }
        System.out.println();
    }

    private void printSeparator(int size) {
        for (int col = 0; col < size; col++) {
            System.out.print("   |");
        }
        System.out.println();
    }

    public Player getPlayerByColor(StoneColor inColor) {
        return switch(inColor) {
            case BLACK -> slatePlayer;
            case WHITE -> shellPlayer;
            default -> throw new IllegalArgumentException("EMPTY has no player");
        };
    }

    private void endScore(StoneColor winner) {
        if (slatePlayer.getColor() == winner) {
            slatePlayer.recordWin();
            System.out.println(slatePlayer.getName() + " wins!");
        }  else if (shellPlayer.getColor() == winner) {
            shellPlayer.recordWin();
            System.out.println(shellPlayer.getName() + " wins!");
        }
        else System.out.println("No winner, no points awarded.");

        System.out.println(slatePlayer.getName() + ": " + slatePlayer.getWins());
        System.out.println(shellPlayer.getName() + ": " + shellPlayer.getWins());
        if (winner != StoneColor.EMPTY) {
            System.out.print("Play another round? ");
            String answer = input.nextLine();
            if (!answer.isBlank() && (answer.charAt(0) == 'y' || answer.charAt(0) == 'Y')) startGame();
        }
    }


}
