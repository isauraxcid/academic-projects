/**
 * Classe cujos objetos representam geradores de puzzles Sumdoku
 * @author Isaura Cid (63761) e Rafael Ribeiro (63733)
 */
public class RandomSumdokuPuzzle {
    private int size;
	private SumdokuPuzzle[] gridsCriadas;
	private int currentIndex;

	private SumdokuPuzzle[] gridsCriadas(int size) {
        if (size == 3) {
            return new SumdokuPuzzle[] {
                new SumdokuPuzzle(
                    new int[][] {
                        {0, 0, 2},
                        {0, 1, 2},
                        {3, 3, 4}
                    },
                    new int[] {5, 2, 5, 5, 1}
                ),
                new SumdokuPuzzle(
                    new int[][] {
                        {0, 0, 0},
                        {0, 0, 1},
                        {0, 1, 1}
                    },
                    new int[] {14, 4}
                )
            };
        } else if (size == 5) {
            return new SumdokuPuzzle[] {
                new SumdokuPuzzle(
                    new int[][] {
                        {0, 0, 0, 1, 2},
                        {3, 3, 0, 1, 2},
                        {4, 5, 6, 6, 7},
                        {4, 5, 8, 8, 7},
                        {9, 9, 9, 10, 10}
                    },
                    new int[] {14, 3, 5, 8, 5, 3, 9, 8, 5, 8, 7}
                )
            };
        }
    
        return new SumdokuPuzzle[0]; // Retorna vazio para tamanhos não suportados
    }

    public RandomSumdokuPuzzle(int size){
		this.gridsCriadas = gridsCriadas(size);
		this.size = size;
		this.currentIndex = 0;
    }

    public int size() {
        return size;
    }

    public boolean hasNextPuzzle(){
		return currentIndex < gridsCriadas.length;
	}

    public SumdokuPuzzle nextPuzzle(){
		return gridsCriadas[currentIndex++];
    }
}
