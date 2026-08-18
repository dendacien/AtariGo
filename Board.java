import java.util.*;

public class Board {
    private final StoneColor[][] grid;
    private final int size;
    private final  Set<Position> capturedChain;
    private final Set<Position> atariChains;

    public Board(int size) {
        this.grid = new StoneColor[size][size];
        this.size = size;
        this.capturedChain = new HashSet<>();
        this.atariChains = new HashSet<>();

        for (int row = 0; row < size; row++) {
            for(int col = 0; col <size; col++) {
                grid[row][col] = StoneColor.EMPTY;
            }
        }

        // Place first four stones for Atari Go
        grid[3][4] = StoneColor.WHITE;
        grid[3][5] = StoneColor.BLACK;
        grid[4][4] = StoneColor.BLACK;
        grid[4][5] = StoneColor.WHITE;
    }

    public MoveResult placeStone(Position stone, StoneColor player) {
        if(!isOnBoard(stone)) return MoveResult.illegal("Out of bounds");
        if(!isEmpty(stone)) return MoveResult.illegal("That space is occupied");

        recordStoneOnBoard(stone, player);
        int playerLiberties = findChain(stone).libertyCount();
        for (Position enemyStart : cardinalEnemyNeighbors(stone, player)) {
            ChainInfo enemyChain = findChain(enemyStart);
            if (enemyChain.libertyCount() == 0) {
                setCapturedChain(enemyChain);
                return MoveResult.legalCapture(player);
            } else if (enemyChain.libertyCount() == 1 && playerLiberties > 1) {
                setAtariChains(enemyChain);
            }
        }

        ChainInfo libertyResult = findChain(stone);
        if (libertyResult.libertyCount() < 2) {
            removeStone(stone);
            return MoveResult.illegal("Can not place into atari, or self capture");
        } else {
            return MoveResult.legalNoCapture();
        }

    }

    private void recordStoneOnBoard(Position stone, StoneColor player) {
        grid[stone.row][stone.col] = player;
    }

    private void removeStone(Position stone) {
        grid[stone.row][stone.col] = StoneColor.EMPTY;
    }

    public int getSize() {
        return size;
    }

    public StoneColor getStoneAtPos(Position stone) {
        return grid[stone.row][stone.col];
    }

    public StoneColor get(Position stone) {
        if (!isOnBoard(stone)) throw new IndexOutOfBoundsException();
        return getStoneAtPos(stone);
    }

    public boolean isOnBoard(Position stone) {
        return stone.row >= 0 && stone.row < size && stone.col >= 0 && stone.col < size;
    }

    public boolean isEmpty(Position stone) {
        return isOnBoard(stone) && getStoneAtPos(stone) == StoneColor.EMPTY;
    }

    private ChainInfo findChain(Position start) {
        StoneColor startColor = getStoneAtPos(start);
        ChainInfo info = new ChainInfo();

        if (!startColor.isPlayerColor()) return info;

        Set<Position> visited = new HashSet<>();
        Deque<Position> frontier = new ArrayDeque<>();
        frontier.add(start);
        visited.add(start);

        while(!frontier.isEmpty()) {
            Position current = frontier.removeFirst();
            info.addStone(current);

            List<Position> neighbors = cardinalNeighbors(current).stream()
                    .filter(n -> !visited.contains(n))
                    .toList();

            for (Position neighbor : neighbors) {
                StoneColor neighborColor = getStoneAtPos(neighbor);
                if (neighborColor== StoneColor.EMPTY) {
                    info.addLiberty(neighbor);
                }
                else if (neighborColor == startColor) {
                    atariChains.remove(neighbor);
                    visited.add(neighbor);
                    frontier.addLast(neighbor);
                }
            }
        }
        return info;
    }

    private List<Position> cardinalNeighbors(Position start) {
        List<Position> out = new ArrayList<>(4);
        Position north = new Position(start.row -1, start.col);
        Position east = new Position(start.row, start.col +1);
        Position south = new Position(start.row + 1, start.col);
        Position west = new Position(start.row, start.col - 1);
        if (isOnBoard(north)) out.add(north);
        if (isOnBoard(east)) out.add(east);
        if (isOnBoard(south)) out.add(south);
        if (isOnBoard(west)) out.add(west);
        return out;
    }

    private Set<Position> cardinalEnemyNeighbors(Position origin, StoneColor player) {
        Set<Position> starts = new HashSet<>();
        StoneColor enemy = player.opponent();
        for (Position neighbor : cardinalNeighbors(origin)) {
            if (getStoneAtPos(neighbor) == enemy) starts.add(neighbor);
        }
        return starts;
    }

    private void setCapturedChain(ChainInfo captured) {
        for (Position stone : captured.listStones()) {
            capturedChain.add(stone);
        }
    }

    private void setAtariChains(ChainInfo atari) {
        for (Position stone : atari.listStones()) {
            atariChains.add(stone);
        }
    }

    public Set<Position> getCatpured() {
        return capturedChain;
    }

    public Set<Position> getAtari() {
        return atariChains;
    }
}
