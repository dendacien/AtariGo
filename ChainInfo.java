import java.util.HashSet;
import java.util.Set;

public final class ChainInfo {
    private final Set<Position> stones = new HashSet<>();
    private final Set<Position> liberties = new HashSet<>();

    public int libertyCount() {
        return liberties.size();
    }

    public void addStone(Position stone) {
        stones.add(stone);
    }

    public void addLiberty(Position space) {
        liberties.add(space);
    }

    public Set<Position> listStones() {
        return stones;
    }
}
