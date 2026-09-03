package src;
public class Player {
    private final StoneColor color;
    private final String name;
    private int wins = 0;
    private boolean bot = false;

    public Player(StoneColor color, String name) {
        this.color = color;
        this.name = name;
        if (name.equals("Computer") && color == StoneColor.WHITE) bot = true;
    }

    public StoneColor getColor() { return color; }
    public String getName() { return name; }
    public int getWins() { return wins; }
    public void recordWin() { wins++; }
    public String getSymbol() { return color.symbol(); }
    
    public boolean isBot() { return bot; }
    public MoveResult botPlayTurn(Game game) {
        Position suggestedMove = game.suggestMoveForCurrentPlayer();
        if (suggestedMove != null) {
            MoveResult priorityResult = game.tryPlacement(suggestedMove);
            if (priorityResult.isLegal()) return priorityResult;
        }
        return MoveResult.illegal("The computer could not find a legal move. You win this round.");
    }
}
