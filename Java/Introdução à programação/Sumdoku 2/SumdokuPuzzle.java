/**
 * Classe cujos objetos representam puzzles de Sumdoku construídos à custa de uma matriz definindo a 
 * que grupo pertence cada casa da quadrícula e um vetor que define quanto somam as casas de cada grupo
 * @author Isaura Cid (63761) e Rafael Ribeiro (63733)
 */

public class SumdokuPuzzle{

    private int[][] groupMembership;
    private int[] groupsValues;

    /** 
     * Método que verifica se os parâmetros fornecidos definem um puzzle válido.
     * 
     * @param groupMembership matriz quadrada (N*N) que define os grupos de cada casa (valores de 0 a numberOfGroups()-1).
     * @param groupsValues vetor que define quanto somam as casas de cada grupo (valores entre 1 e (N^2 + N)/2).
     * @return {@code true} se os parâmetros definem um puzzle válido, {@code false} caso contrário.
     * @requires {@code 0 <= g < gropsValues.length}.
     */
    public static boolean definesPuzzle(int[][] groupMembership, int[] groupsValues){
        int N = groupMembership.length; //cria um int N correspondente ao tamanho da grid (groupMembership.length representa cada linha da matriz, logo, cada linha da grid)

        // Ponto 1
        if (N < 3 || N > 9) { //verifica se N é menor que 3 ou maior que 9
            return false; //se for, retorna false
        }
        for (int[] gM : groupMembership) { //percorre a matriz groupMembership
                if(gM.length != N) { //verifica se cada elemento é diferente de N
                    return false; //se for diferente de N, retorna false
                }
        }
        // Ponto 2
        if (groupsValues.length < 1 || groupsValues.length > N*N) { //verifica se o tamanho de groupsValues é menor que 1 e maior que N*N
            return false; //se for, retorna false
        }
        for(int g = 0; g < groupsValues.length; g++){ //percorre cada elemento do groupsValues
            if (groupsValues[g] < 1 || groupsValues[g] > (N*N*N + N*N)/2){ //verifica se o valor de g é menor que 1 ou maior que (N^3 + N^2)/2
                return false; //se for, retorna false
            }
        }
        // Ponto 3
        for (int r = 0; r < groupMembership.length; r++) { //percorre cada linha da matriz groupMembership 
            for(int c = 0; c < groupMembership[r].length; c++){ //percorre cada coluna de cada linha da matriz
                if (groupMembership[r][c] < 0 || groupMembership[r][c] > groupsValues.length-1) { //verifica se o grupo respetivo a cada casa não está no intervalo
                    return false; //se não estiver no intervalo, portanto, se o if for true, retorna false
                }
            }
        }
        // Ponto 4
        for (int g = 0; g < groupsValues.length; g++) { //percorre o groupsValues (g de group - cada posição do vetor é um grupo)
            boolean found = false; //inicializa um boolean found a false
            for (int r = 0; r < groupMembership.length; r++) { //percorre cada linha da matriz groupMembership
                for (int c = 0; c < groupMembership[r].length; c++) { //percorre cada coluna (cada casa contém o nº do grupo a que pertence) de cada linha da matriz groupMembership 
                    found |= groupMembership[r][c] == g;
                }
            }
            if(!found){
                return false;
            }
        }
        // Ponto 5
        //se todos os g forem encontrados pelo menos 1 vez, retorna true
        SumdokuSolver solver = new SumdokuSolver(groupMembership, groupsValues);
        return  solver.howManySolutions(N) == 1;
    }
    
    /**
     * Construtor que dá um puzzzle através dos argumentos dados.
     * 
     * @param groupMembership matriz quadrada (N*N) que define os grupos de cada casa (valores de 0 a numberOfGroups()-1).
     * @param groupsValues groupsValues vetor que define quanto somam as casas de cada grupo (valores entre 1 e (N^2 + N)/2).
     * @requires {@code definesPuzzle(groupMembership, groupsValues) == true}.
     * @return Uma instância de SumdokuPuzzle que representa o puzzle definido por groupMembership e groupsValues.
     */
    public SumdokuPuzzle(int[][] groupMembership, int[] groupsValues){
        if (definesPuzzle(groupMembership, groupsValues)){ //verifica se os argumentos dados definem um puzzle Sumdoku
            this.groupMembership = groupMembership;  //se definirem, então a matriz passa a ser igual à argumentada
            this.groupsValues = groupsValues; //e o vetor passa a ser igual ao argumentado
        } else {
            this.groupMembership = null; //se não definirem, então a matriz passa a null
            this.groupsValues = null; //se não definirem, então o vetor passa a null
        }
    }

    /**
     * Método que dá o tamanho da grelha do puzzle
     * 
     * @return {@code groupMembership.length} se {@code groupMembership != null} ou 0, caso contrário.
     */
    public int size() {
        if (groupMembership != null) { //verifica se groupMembership não é null
            return groupMembership.length; //se não for null, devolve o tamanho da matriz, logo, o tamanho da grid
        }
        return 0; //se for null, devolve o valor 0
    }

    /**
     * Método que dá o número de grupos do puzzle.
     * 
     * @return {@code groupsValues.length} se {@code groupsValues != null} ou 0, caso contrário.
     */
    public int numberOfGroups(){
        if (groupsValues != null) { //verifica se groupsValues não é null
            return groupsValues.length; //se não for null, devolve o tamanho do vetor, logo, o número de grupos existentes
        } else {
            return 0; //se for null, devolve o valor 0
        }
    }

    /**
     * Método que dá o número do grupo da casa na linha e coluna dada.
     * 
     * @param col que representa a coluna da grelha.
     * @param row que representa a linha da grelha.
     * @requires 1 <= col <= size() && 1 <= row <= size().
     * @return {@code groupMembership[row - 1][col - 1]}.
     */
    public int groupNumber(int row, int col) {
        return groupMembership[row - 1][col - 1]+1; //devolve o número do grupo da casa argumentada
    }

    /**
     * Método que dá quanto somam as casas do grupo dado
     * 
     * @param group
     * @requires 1 <= group <= numberOfGroups().
     * @return {@code groupsValues[group - 1]}
     */
    public int valueGroup(int group) {
        return groupsValues[group - 1]; //devolve o valor da soma do grupo argumentado
    }

    /**
     * Método que verifica se playedGrid é a solução do puzzle
     * 
     * @param playedGrid
     * @requires {@code playedGrid != null && playedGrid == size()}.
     * @return true se playedGrid for a solução do puzzle, ou false se contrário
     */
    public boolean isSolvedBy(SumdokuGrid playedGrid) {
        int size = playedGrid.size();
            
        //verifica se a solução está preenchida com valores válidos
        for (int r = 1; r <= size; r++) {
            for (int c = 1; c <= size; c++) {
                if (!playedGrid.isFilled(r, c) || playedGrid.value(r, c) < 1 || playedGrid.value(r, c) > size) {
                    return false; 
                }
            }
        }
        //percorre cada grupo e faz a soma do respetivo grupo
        for (int g = 1; g <= numberOfGroups(); g++) {
            int soma = 0;
            for (int r = 1; r <= size; r++) {
                for (int c = 1; c <= size; c++) {
                    if (groupNumber(r, c) == g) {
                        soma += playedGrid.value(r, c);
                    }
                }
            }
            //verifica se a soma dos grupos está certa
            if (soma != valueGroup(g)) {
                return false;
            }
        }
    return true;
    }
    
    /**
     * Método que verifica se as casas já preenchidas de playedGrid têm os valores da solução do puzzle
     * 
     * @param playedGrid
     * @requires {@code playedGrid != null && playedGrid == size()}.
     * @return true se os valores jogados forem os mesmos que os valores da solução, false se contrário
     */
    public boolean isPartiallySolvedBy(SumdokuGrid playedGrid) {
        int size = playedGrid.size();
        SumdokuSolver solver = new SumdokuSolver(this.groupMembership, this.groupsValues);
        int [][][] solution = solver.findSolutions(1);
        
        for (int r = 1; r <= size; r++) {
            for (int c = 1; c <= size; c++) {
                if (playedGrid.isFilled(r,c)) {
                    if (playedGrid.value(r,c) != solution[0][r - 1][c - 1]) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * Método que dá uma representação textual das pistas do puzzle.
     * 
     * @return String com as pistas do puzzle.
     */
    public String cluesToString() {
        StringBuilder gridClues = new StringBuilder();
        StringBuilder clues = new StringBuilder(); //cria uma StringBuilder para apresentar as listas numa linha

        for (int r = 0; r < groupMembership.length; r++) {
            for (int c = 0; c < groupMembership[r].length; c++) {
                int g = groupMembership[r][c];
                gridClues.append(" ");
                gridClues.append(g+1);
            }
            gridClues.append("\n");
        }
        for (int g = 0; g < groupsValues.length; g++) { //percorre o vetor groupsValues
            clues.append("G").append(g+1).append(" = ").append(groupsValues[g]); //adiciona à StringBuilder o número do grupo e a soma respetiva
            if (g < groupsValues.length - 1) {
                clues.append(" ");
            }
        }
        return gridClues.append(clues).append(" \n").toString();
    }

    /**
     * Método que dá uma representação textual do puzzle.
     * 
     * @return String com o puzzle.
     */
    public String toString() {
        StringBuilder sB = new StringBuilder();

        for (int r = 0; r < groupMembership.length; r++) {
            for (int c = 0; c < groupMembership[r].length; c++) {
                int g = groupMembership[r][c];
                sB.append(g).append(" ");
            }
            sB.append("\n");
        }
        return sB.toString();
    }
}