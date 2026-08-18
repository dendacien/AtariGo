public enum StoneColor {
    EMPTY("   "),
    BLACK("(B)"),
    WHITE("(W)");

    public final String symbol;

    StoneColor(String symbol) {
        this.symbol = symbol;
    }

    public StoneColor opponent() {
        if(this == BLACK) return WHITE;
        if(this == WHITE) return BLACK;
        return EMPTY;
    }

    public boolean isPlayerColor() {
        return this == BLACK || this == WHITE;
    }

    public String symbol() {
        return symbol;
    }
}
