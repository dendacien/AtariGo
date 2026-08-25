import java.util.Scanner;

public class Main {
    public static void main (String[] args) {
        Scanner scan = new Scanner(System.in);
        System.out.println("Welcome to Atari Go!");
        instructions();
        System.out.print("Press Enter to start");
        scan.nextLine();
        System.out.print("Name for Player 1 (default Slate): ");
        String playerName1 = scan.nextLine();
        if (playerName1.isBlank()) playerName1 = "Slate";
        Player slatePlayer = new Player(StoneColor.BLACK, playerName1);
        System.out.print("Name for Player 2 (default Shell)(enter Computer for a bot): ");
        String playerName2 = scan.nextLine();
        if (playerName2.isBlank()) playerName2 = "Shell";
        Player shellPlayer = new Player(StoneColor.WHITE, playerName2);
        Match match = new Match(scan, slatePlayer, shellPlayer);
        match.startGame();
        scan.close();
        System.out.println("Thank you for playing!");
    }

    public static void instructions () {
        String RED = "\u001B[31m";
        String YELLOW = "\u001B[33m";
        String CYAN = "\u001B[36m";
        String PURPLE = "\u001B[35m";
        String RESET = "\u001B[0m";
        System.out.println("Stones are represented by "+CYAN+"(W)"+RESET+" for "+CYAN+"white"+RESET+" and "+PURPLE+"(B)"+RESET+" for "+PURPLE+"black"+RESET+".");
        System.out.println("This game is played on a 9x9 grid. To place a stone give the column and row you wish to place your stone.");
        System.out.println("You can enter this in any order and any capitalization: 1A, A1, a1, etc");
        System.out.println("Player 1 will go first and will play "+PURPLE+"black (B)"+RESET+".");
        System.out.println("Any stone can be placed in an empty space and if placing that stone will not result in immediate capture.");
        System.out.println("A stone is captured as soon as it has no more \"liberties\" which are empty spaces directly from the");
        System.out.println("stone. No diagonal directions are considered in Go. Stones that are touching vertically or hoizonally");
        System.out.println("share liberties. Any stone or chain of stones that has one liberty is said to be in \"atari\".");
        System.out.println("Stones in atari will be shown in "+YELLOW+"yellow"+RESET+".");
        System.out.println("The player that captures the first stone wins the game. Captured stones will be shown in "+RED+"red"+RESET+".");
        System.out.println("Enter q or Q in any move to quit.");
    }

}