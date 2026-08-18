import java.util.Objects;

// Struct-like class
public final class Position {
    final int row;
    final int col;

    public Position(int row, int col) {
        this.row = row;
        this.col = col;
    }

    @Override
    public boolean equals(Object o) {
        if(!(o instanceof Position p)) return false;
        return row == p.row && col == p.col;
    }

    @Override
    public int hashCode() {
        return Objects.hash(row, col);
    }
}
