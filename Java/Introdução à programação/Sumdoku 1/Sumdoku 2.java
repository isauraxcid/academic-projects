public class Sumdoku {
    public static void main (String[] args) {
 
        
    }

    public static int rowOfSquare (int square, int gridSize) {
        if (square % gridSize == 0) {
            return square / gridSize;
        }
        return square / gridSize + 1;
    }

    public static int columnOfSquare (int square, int gridSize) {
        return square % gridSize;
    }

    private static StringBuilder gridLine(int row, SumdokuGrid grid) {
        StringBuilder sbLine = new StringBuilder();
        for (int i=1; i <= grid.size(); i++){
            sbLine.append(grid.value(row, i));
        }
        return sbLine;
    }

    private static StringBuilder gridColumn(int col, SumdokuGrid grid) {
        StringBuilder sbCol = new StringBuilder();
        for (int i=1; i <= grid.size(); i++){
            sbCol.append(grid.value(i, col));
        }
        return sbCol;
    }

    public static boolean isValidForPuzzle(SumdokuGrid grid) {
        if (grid != null){
            for (int l=1; l <= grid.size(); l++){ // go for every line of the grid
                String lin = gridLine(l, grid).toString();
                for (int j=1; j <= grid.size(); j++){
                    if (!lin.contains((Integer.toString(j)))){
                        return false;
                    }
                }
            }
            for (int c=1; c <= grid.size(); c++){ // go for every column of the grid
                String col = gridColumn(c, grid).toString();
                for (int j=1; j <= grid.size(); j++){
                    if (!col.contains((Integer.toString(j)))){
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

    public static boolean isValidForPuzzle(GridGroups groups) {
        int gridSize = groups.gridSize();

        if (groups != null) {
            for (int r=1; r<=gridSize; r++) {
                for (int c=1; c<=gridSize; c++) {
                    int groupOfSquare = groups.groupOfSquare(r,c);
                    if (groupOfSquare == 0) {
                        return false;
                    }   
                } 
            return true;
            }
        } 
        return false;
    }

    public static boolean definesPuzzle(SumdokuGrid grid, GridGroups groups) {
        int gridSize = groups.gridSize();
        int sizeOfTheGrid = grid.size();

        // falta verificar se existe apenas uma solução

        if (gridSize == sizeOfTheGrid) {
            return true;
        }
        return false;
    }

    public static String clueToString(SumdokuGrid grid, GridGroups groups) {
        int numberOfGroups = groups.numberOfGroups();
        int sum = 0;

        for (groups = 1; groups <= numberOfGroups; groups++) {

        } 
    }
}
