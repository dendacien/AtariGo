package src;
import java.util.Objects;

public final class Position {
    final int row;
    final int col;

    public Position(int row, int col) {
        this.row = row;
        this.col = col;
    }

    @Override
    public String toString() {
        return "" + (char)('A' + row) + (col + 1) ;
    }

    @Override
    public boolean equals(Object o) {
        if(!(o instanceof Position)) return false;
        Position p = (Position) o;
        return row == p.row && col == p.col;
    }

    @Override
    public int hashCode() {
        return Objects.hash(row, col);
    }
}
