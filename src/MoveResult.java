package src;
public class MoveResult {
    private final boolean legal;
    private final boolean capture;
    private final StoneColor winner;
    private final String message;

    private MoveResult(boolean legal, boolean capture, StoneColor winner, String message){
        this.legal = legal;
        this.capture = capture;
        this.winner = winner;
        this.message = message;
    }

    public static MoveResult illegal(String message) {
        return new MoveResult(false, false, StoneColor.EMPTY, message);
    }

    public static MoveResult legalNoCapture(String message) {
        return new MoveResult(true, false, StoneColor.EMPTY, message);
    }

    public static MoveResult legalCapture(StoneColor winner) {
        return new MoveResult(true, true, winner, "Capture made. Game over.");
    }

    public boolean isLegal() { return legal; }

    public boolean isCapture() { return capture; }

    public StoneColor getWinner() { return winner; }

    public String getMessage() { return message; }
}
