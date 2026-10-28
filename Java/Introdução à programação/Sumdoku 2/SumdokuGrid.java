/**
 * Classe cujos objetos representam grelhas de Sumdoku, cujas casas podem estar ou não preenchidas
 * @author Isaura Cid (63761) e Rafael Ribeiro (63733)
 */

public class SumdokuGrid {
    public int size;
    private Integer[][] grid;
    
    /**
     * Constrói uma grelha sumdoku
     * 
     * @param size Tamanho  da grelha
     * @ensures {@code \result >= 3 && \result <= 9 }
     */
    public SumdokuGrid(int size) {
        this.size = size; //size passa a ser o argumentado
        this.grid = new Integer[size][size]; //a grelha passa a ter o tamanho size*size
    }

    /**
     * Dá o tamanho da grelha
     * 
     * @return o tamanho da grelha
     */
    public int size() {
        return this.size;
    }

    /**
     * Dá o valor de uma casa específica da grelha
     * 
     * @param r Linha da casa 
     * @param c Coluna da casa 
     * @return o valor da casa na linha r e coluna c 
     * @requires {@code 1 <= r && r <= size() && 1 <= c && c <= size() && isFilled(r,c)}
     * @ensures {@code 1 <= \result && \result <= size()}
     */
    public int value(int r, int c) {
        return grid[r - 1][c - 1]; 
    }


    /**
     * Verifica se a casa está preenchida
     * 
     * @param r Linha onde se encontra a casa
     * @param c Coluna onde se encontra a casa
     * @return true se a casa estiver preenchida, false se contrário
     * @requires {@code 1 <= r && r <= size() && 1 <= c && c <= size() }
     */
    public boolean isFilled(int r, int c) {
        return grid[r - 1][c - 1] != null;
    }
    
    /**
     * Preenche a casa da grelha com um valor específico
     * 
     * @param r Linha onde se encontra a casa para preencher
     * @param c Coluna onde se encontra a casa para preencher
     * @param value Valor para preencher a casa da grelha
     * @requires {@code 1 <= r && r <= size() && 1 <= c && c <= size(), 1 <= value && value <= size() }
     * @ensures {@code isFilled(r,c) && value(r, c) == value}
     */
    public void fill(int r, int c, int value) {
        grid[r - 1][c - 1] = value; 
    }

    /**
     * Dá uma representação textual da grelha
     * 
     * @returns uma String para a representação textual da grelha
     */
    public String toString() {
        StringBuilder sb = new StringBuilder();

        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                if (grid[r][c] == null) { //verifica se a grelha está preenchida
                    sb.append(". "); //se não estiver, adiciona um ponto que significa "por preencher"
                } else {
                    sb.append(grid[r][c]).append(" "); //se estiver preenchida, adicionar o valor da casa
                }
            }
            sb.append("\n"); //adiciona uma nova linha após atingir o size da grelha
        }
        return sb.toString(); //converte numa String e retorna-a
    }
}