public class Player {
    private final StoneColor color;
    private final String name;
    private int wins = 0;

    public Player(StoneColor setColor, String setName) {
        color = setColor;
        name = setName;
    }

    public StoneColor getColor() { return color; }
    public String getName() { return name; }
    public int getWins() { return wins; }
    public void recordWin() { wins++; }

}
