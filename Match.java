import java.util.Scanner;
import java.util.Set;

public class Match {
    private final Player slatePlayer, shellPlayer;
    private final Scanner input;
    private Game currentGame;
    private final String RED = "\u001B[31m";
    private final String YELLOW = "\u001B[33m";
    private final String CYAN = "\u001B[36m";
    private final String PURPLE = "\u001B[35m";
    private final String RESET = "\u001B[0m";

    public Match(Scanner input, Player player1, Player player2) {
        this.input = input;
        this.slatePlayer = player1;
        this.shellPlayer = player2;
    }

    public void startGame() {
        currentGame = new Game();
        while(currentGame.isRunning()) {
            Player currentPlayer = getPlayerByColor(currentGame.getCurrentTurn());
            System.out.println("Current player is " + currentPlayer.getName() + " " + currentPlayer.getSymbol());
            gameUI(currentPlayer);
        }
        endScore(currentGame.getWinner());
    }

    private void gameUI(Player currentPlayer) {
        int size = currentGame.getBoardSize();

        if(getPlayerByColor(currentGame.getCurrentTurn()).isBot()) {
            MoveResult result = currentPlayer.botPlayTurn(currentGame);

            if (!result.isLegal()) {
                System.out.println(result.getMessage());
                currentGame.currentPlayerConcedes();
                return;
            }

            if (result.isCapture()) {
                printBoard(size);
                System.out.println(result.getMessage());
                currentGame.endGame();
                return;
            }
            else System.out.println(result.getMessage());
        } else {
            printBoard(size);
            MoveResult result = MoveResult.illegal("");
            while (!result.isLegal()) {
                if (!result.getMessage().isBlank()) System.out.println(result.getMessage());
                Position placement = captureInput(currentPlayer);
                if (placement.row == -1 && placement.col == -1) {
                    currentGame.endGame();
                    return;
                }
                result = currentGame.tryPlacement(placement);
                if (result.isCapture()) {
                    printBoard(size);
                    System.out.println(result.getMessage());
                    currentGame.endGame();
                    return;
                }
            }
            if (!result.getMessage().isBlank()) System.out.println(result.getMessage());
        }
    }

    private Position captureInput(Player currentPlayer) {
        MoveResult result = MoveResult.illegal("");
        int captureRow, captureCol;
        Position returnPosition = null;
        while (returnPosition == null) {
            captureCol = 0;
            captureRow = 0;
            if (!result.getMessage().isBlank()) System.out.println(result.getMessage());
            String playerSymbol = currentPlayer.getSymbol();
            if (playerSymbol.contains("B")) playerSymbol = PURPLE + playerSymbol + RESET;
            else playerSymbol = CYAN + playerSymbol + RESET;
            System.out.print(playerSymbol + " move: ");
            String captureString = input.nextLine();
            if (captureString.contains("q") || captureString.contains("Q")) {
                currentGame.endGame();
                return new Position(-1, -1);
            }
            if (captureString.length() != 2) {
                result = MoveResult.illegal("Invalid input, must be one number and one letter");
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
                    returnPosition = new Position(captureRow - 1, captureCol -1);
                }
            }
        }
        return returnPosition;
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
        Set<Position> captures = currentGame.getCaptures();
        Set<Position> ataris = currentGame.getAtarPositions();
        for (int col = 0; col < size; col++) {
            Position stone = new Position(row, col);
            String intersection = currentGame.getIntersectionSymbol(stone);
            if (captures.contains(stone)) intersection = RED + intersection + RESET;
            else if (ataris.contains(stone)) intersection = YELLOW + intersection + RESET;
            else if (intersection.contains("B")) intersection = PURPLE + intersection + RESET;
            else if (intersection.contains("W")) intersection = CYAN + intersection + RESET;
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
