/**
 *@author Rafael Ribeiro 63733 e Isaura Cid 63761
 */
import java.util.Scanner;
public class Sumdoku {
    /**
    O método main, através da leitura de um argumento da linha de comandos, permite jogar o
    jogo com um puzzle builtin para o tamanho escolhido, se este existir. Se não for fornecido
    qualquer argumento então o programa começa por ler a grelha e o conjunto de grupos do puzzle
    e depois permite jogar o jogo com esse puzzle. Em ambos os casos é feita a validação de
    que se trata efetivamente de um puzzle e o número máximo de tentativas é o número de casas. 
    @params args  O tamanho da grelha escolhida no terminal.
    @requires args[0] == 3 
     */
    public static void main (String[] args) {
        
        Scanner scanner = new Scanner(System.in);

        if(args.length > 0){
            int inputValue = Integer.parseInt(args[0]);
            scanner = new Scanner(System.in);

            System.out.println("Bem vindo ao jogo Sumdoku!");
            System.out.println("Neste jogo a grelha tem tamanho " + inputValue + " e tens estas pistas:");
            SumdokuGrid grid = getBuiltInGrid(inputValue);
            GridGroups groups =getBuiltInGroups(inputValue);
            System.out.println(groups.toString());
            cluesToString(grid, groups);
            int maxAttempts = grid.size()*grid.size();
            
            play(grid, groups, maxAttempts, scanner);

        } else {
            
            scanner = new Scanner(System.in);
            boolean invalidSize = true;
            int size = 0;
            while(invalidSize) {
                System.out.println("Leitura do puzzle.");
                System.out.println("Tamanho da grelha?");
                size = scanner.nextInt();
                if(size < 3 || size > 9) {
                    System.out.println("Tamanho da grelha invalido.");
                } else {
                    invalidSize = false;
                }    
            }
            SumdokuGrid grid = null;
            GridGroups groups = null;
            boolean validPuzzle = true;
            while(validPuzzle) {
                grid = readGrid(size, scanner);            
                groups = readGroups(grid, scanner);
                if(definesPuzzle(grid, groups)) {
                    validPuzzle = false;
                } else {
                    System.out.println("A grid e os grupos juntos, nao definem um puzzle valido.");
                }
            }
            

            System.out.println("Bem vindo ao jogo Sumdoku!");
            System.out.println("Neste jogo a grelha tem tamanho " + grid.size() + " e tens estas pistas:");
            System.out.println(groups.toString());
            cluesToString(grid, groups);
            int maxAttempts = grid.size()*grid.size();
            play(grid, groups, maxAttempts, scanner);                             

        }
        scanner.close();
    }
    /**
     * Uma função que calcula, a linha de uma casa de uma grelha quadrada, dado o seu tamanho.
     * Como descrito anteriormente, as linhas de 1 a gridSize e as casas são numeradas de 1 a gridSize x gridSize.
     * @param square casa cuja linha será calculada.
     * @param gridSize tamanho da grelha.
     * @return O número da linha em que a casa especificada se encontra.
     */
    public static int rowOfSquare (int square, int gridSize) {
        if (square % gridSize == 0) {
            return square / gridSize;
        }
        return square / gridSize + 1;
    }
    /**
     * Uma função que calcula, a coluna de uma casa de uma grelha quadrada, dado o seu tamanho.
     * Como descrito anteriormente, as colunas de 1 a gridSize e as casas são numeradas de 1 a gridSize x gridSize.
     * @param square casa cuja coluna será calculada.
     * @param gridSize tamanho da grelha.
     * @return O número da coluna em que a casa especificada se encontra.
     */
    public static int columnOfSquare (int square, int gridSize) {
        if (square % gridSize == 0) {
            return gridSize;
        }
        return square % gridSize;
    }
    /**
     * Gera uma linha da grelha dada uma linha específica.
     * 
     * @param row A linha da grelha.
     * @param grid A grelha a ser processada.
     * @return Uma StringBuilder representando a linha da grelha.
     */
    private static StringBuilder gridLine(int row, SumdokuGrid grid) {
        StringBuilder sbLine = new StringBuilder();
        for (int i=1; i <= grid.size(); i++){
            sbLine.append(grid.value(row, i));
        }
        return sbLine;
    }
    /**
     * Gera uma coluna da grelha dada uma coluna específica.
     * 
     * @param col A coluna da grelha.
     * @param grid A grelha a ser processada.
     * @return Uma StringBuilder representando a coluna da grelha.
     */
    private static StringBuilder gridColumn(int col, SumdokuGrid grid) {
        StringBuilder sbCol = new StringBuilder();
        for (int i=1; i <= grid.size(); i++){
            sbCol.append(grid.value(i, col));
        }
        return sbCol;
    }
    /**
     * Verifica se uma grelha é válida para um puzzle Sumdoku.
     * A grelha é considerada válida se todos os números entre 1 e grid.size()
     * ocorrem em cada linha e coluna de grid.
     * 
     * @param grid A grelha a ser verificada.
     * @return true se a grelha for válida, false caso contrário.
     */
    public static boolean isValidForPuzzle(SumdokuGrid grid) {
        if (grid != null){
            for (int l=1; l <= grid.size(); l++){ // go for every line of the grid
                String lin = gridLine(l, grid).toString();
                String col = gridColumn(l, grid).toString();
                for (int j=1; j <= grid.size(); j++){
                    if (!lin.contains((Integer.toString(j))) || !col.contains((Integer.toString(j)))){
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    }
    /**
     * Verifica se os grupos de uma grelha são válidos para um puzzle Sumdoku.
     * A verificação consiste em garantir que não há grupos vazios e que cada casa pertence a um grupo.
     * 
     * @param groups Os grupos da grelha a serem verificados.
     * @return true se os grupos forem válidos, false caso contrário.
     */
    public static boolean isValidForPuzzle(GridGroups groups) {
        
        if(groups == null) {
            return false;
        }
        if(groups.gridSize() < 3 || groups.gridSize() > 9) {
            return false;
        }
        int gridSize = groups.gridSize();
        StringBuilder sb = new StringBuilder();
        //System.out.println(gridSize);
        
        for (int r=1; r<=gridSize; r++) {
            for (int c=1; c<=gridSize; c++) {
                int gnum = groups.groupOfSquare(r,c);
                //System.out.println(String.format("r = %d c = %d", r,c));
                sb.append(gnum);
                //System.out.println(sb.toString());

                if(gnum == 0) {
                        return false;
                }  
            }   
        }

        boolean bool = true ;
        //System.out.println(sb.toString());
        for (int i = 1; i <= groups.numberOfGroups(); i++) {
            boolean groupIsPresent = false;
            for(int j = 0; j < sb.length(); j++) {
                String str;
                if(j > 8) {
                    str = String.valueOf(sb.charAt(j)) + String.valueOf(sb.charAt(j+1));
                    j++;
                } else {
                    str = String.valueOf(sb.charAt(j));
                }
                if(str.equals(String.valueOf(i))) {
                    groupIsPresent = true;
                }
            }
            bool &= groupIsPresent;
        }    
        return bool;
    }
    
/**
     * Verifica se um puzzle é válido combinando as grelhas e os grupos.
     * Um puzzle é considerado válido se a grelha e os grupos têm tamanhos compatíveis.
     * 
     * @param grid A grelha a ser verificada.
     * @param groups Os grupos da grelha a serem verificados.
     * @return true se o puzzle for válido, false caso contrário.
     */
    public static boolean definesPuzzle(SumdokuGrid grid, GridGroups groups) {

        // se houver grid.size - 1 grupos com um so elemento em linhas e colunas diferentes entao so existe uma solucao
        if (grid.size() != groups.gridSize()) {
            return false;
        }
        // numero de grupos nao pode ser inferior ao grid.size, se nao existe multiplas solucoes.
        // Para um grid.size de 3 se existirem 3 grupos cada grupo poderia ser por exemplo cada uma das linhas ou colunas 
        // 1 solucao do Sumdoku poderia-se trocar as posicoes das linhas ou colunas (trocar ordem de linha 1 e 2) isso resultava numa nova solucao
        if (grid.size() > groups.numberOfGroups()) {
            return false;
        }
        return true; 

    }
     /**
     * Gera uma representação textual das pistas de um puzzle Sumdoku.
     * A representação consiste na soma dos valores das casas de cada grupo.
     * 
     * @param grid A grelha com a solução.
     * @param groups Os grupos da grelha que definem o puzzle.
     * @return Uma string com a soma das casas de cada grupo.
     */
    public static String cluesToString(SumdokuGrid grid, GridGroups groups) {
        int numberOfGroups = groups.numberOfGroups();
        int gridSize = groups.gridSize();
        StringBuilder clues = new StringBuilder("Soma das casas:");

        for (int group = 1; group <= numberOfGroups; group++) {
            int sum = 0;
            for (int r = 1;  r <= gridSize; r++) {
                for (int c = 1; c <= gridSize; c++) {
                    if (groups.groupOfSquare(r,c) == group) {
                        sum += grid.value(r,c);
                    }
                }
            }
            clues.append(" G").append(group).append(" = ").append(sum);
        }
        return clues.append(" \n").toString(); 
    }
    /**
     * Lê uma grelha do jogador, validando as entradas para garantir que são válidas.
     * 
     * @param size Tamanho da grelha.
     * @param scanner Scanner para ler os dados do jogador.
     * @return A grelha preenchida com os valores fornecidos.
     * @requires size > 2 e size < 10
     */
    public static SumdokuGrid readGrid(int size, Scanner scanner) {
        SumdokuGrid grid = new SumdokuGrid(size);

        boolean invalidGrid = true;

        while(invalidGrid) { 
            if (scanner != null) {
               if (size>2 && size<10) {
                   for(int square = 1; square<= size*size; square++) {
                           int squareValue;
                           boolean bool;
                           do {
                               bool = false;
                              
                               System.out.println("Insere um valor para a casa "+ square);
                               squareValue = scanner.nextInt();
                              
                               if(squareValue < 1 || squareValue > size) {
                                       System.out.println("Valor invalido. Tem de estar entre 1 e " + size);
                                       bool = true;
                                   }
                          
                           } while (bool);
                      
                           grid.fill(rowOfSquare(square, size), columnOfSquare(square, size), squareValue);
                      
                    }
                }
            } 
            if(isValidForPuzzle(grid)) {
                invalidGrid = false;
            } else {
                System.out.println("A grelha inserida é inválida. Recomece.");
            }
        } 
            
        return grid;
    }
   

     /**
     * Lê os grupos do puzzle definidos pelo jogador.
     * 
     * @param grid A grelha do puzzle.
     * @param scanner Scanner para ler os dados do jogador.
     * @return O conjunto de grupos definidos pelo jogador.
     */
    public static GridGroups readGroups(SumdokuGrid grid, Scanner scanner) {
       
        int  gridsize = grid.size();
        int  numOfSquares = gridsize*gridsize;

        GridGroups groups=null;
        boolean invalidGroups = true;
        while(invalidGroups) { 
            if(scanner != null) {
                
                StringBuilder sb = new StringBuilder();
                
                int numberOfGroups = 0;
                boolean numberIsNotValid = true;
                
                while (numberIsNotValid) { 
                    System.out.println("Insere o numero de grupos.");
                    numberOfGroups = scanner.nextInt();
                    if(numberOfGroups <= 0 || numberOfGroups > numOfSquares) {
                        System.out.println("Numero de grupos invalido.");
                    } else {
                        numberIsNotValid = false;
                    }
                    
                }
                
                groups = new GridGroups(gridsize, numberOfGroups);
    
                int groupSize = 0;
                
                for(int i = 1; i <= numberOfGroups; i++) {
                    boolean invalidSize = true;
                    
                    while (invalidSize) { 
                        System.out.println("Insere o tamanho do grupo " + i);
                        groupSize = scanner.nextInt();
                        if(groupSize <= 0 || groupSize > numOfSquares) {
                            System.out.println("Tamanho do grupo invalido.");
                        } else {
                            invalidSize = false;
                        }
                        
                    }
        
                    for(int j = 1; j <= groupSize; j++) {
                        System.out.println("Insere a casa " + j + " do grupo " + i);
                        int squareInput;
                        boolean bool;
                        do {
                            bool = false; 
                            squareInput = scanner.nextInt();
                           
                            for(int h = 0; h < sb.length(); h++) {
                                String str;
                                if(h > 8) {
                                    str = String.valueOf(sb.charAt(h)) + String.valueOf(sb.charAt(h+1));
                                    h++;
                                } else {
                                    str = String.valueOf(sb.charAt(h));
                                }
                                if(str.equals(String.valueOf(squareInput))) {
                                    System.out.println("Esta casa ja pertence a um grupo, tenta outra vez.");
                                    bool = true;
                                }
                            }
                            
                        } while (bool);
                        sb.append(squareInput);
        
                        groups.addSquareToGroup(rowOfSquare(squareInput, gridsize), columnOfSquare(squareInput, gridsize), i);
                    }
        
                }
            }
            if(isValidForPuzzle(groups)) {
                invalidGroups = false;
            } else {
                System.out.println("Os grupos inseridos sao inválidos. Recomece.");
            }
        }
        return groups;
    } 
   /**
     * Retorna uma grelha pré-definida para puzzles de tamanho 3.
     * 
     * @param size O tamanho da grelha.
     * @return A grelha pré-definida ou null se o tamanho não for 3.
     */
    public static SumdokuGrid getBuiltInGrid(int size) {
        SumdokuGrid grid = new SumdokuGrid(size);
        if(size == 3) {
            String str = "312123231";
            for(int ln = 0; ln<str.length(); ln++) {
                grid.fill(rowOfSquare(ln +1, size), columnOfSquare(ln +1, size), Integer.parseInt(str.substring(ln, ln +1)));
            }

            return grid;
        }

        return null;
    }
    /**
     * Retorna os grupos pré-definidos para puzzles de tamanho 3.
     * 
     * @param size O tamanho da grelha.
     * @return Os grupos pré-definidos ou null se o tamanho não for 3.
     */
    public static GridGroups getBuiltInGroups(int size) {
        
        GridGroups gg = null;
        
        if(size == 3) {

            String str = "113123445";
            gg = new GridGroups(size, 5);
            for(int ln = 0; ln<str.length(); ln++) {
                gg.addSquareToGroup(rowOfSquare(ln +1, size), columnOfSquare(ln +1, size), Integer.parseInt(str.substring(ln, ln +1)));
            }

        }
        return gg;
    }
    /**
    * Verifica se o puzzle foi resolvido corretamente, comparando a grelha preenchida pelo jogador
    * (playedGrid) com a grelha original (grid).
    * 
    * A função percorre todas as células das duas grelhas e, se todas as células de playedGrid
    * corresponderem às de grid, a função retorna true, indicando que o puzzle foi resolvido. Caso
    * contrário, retorna false.
    * 
    * @param playedGrid A grelha preenchida pelo jogador.
    * @param grid A grelha original do puzzle.
    * @return true se a grelha preenchida pelo jogador resolver o puzzle, false caso contrário.
    * @requires playedGrid != null e grid != null e grid e playedGrid terem o mesmo tamanho.
    */
    public static boolean puzzleSolved(SumdokuGrid playedGrid, SumdokuGrid grid) {
    
        int gridSize = grid.size();

    for (int r = 1; r <= gridSize; r++) {
        for (int c = 1; c <= gridSize; c++) {
            if (playedGrid.value(r, c) != grid.value(r, c)) {
                return false;
            }
        }
    }
    return true;
    }
    /**
     * Inicia o jogo e permite que o jogador tente resolver o puzzle Sumdoku.
     * O procedimento gerencia as tentativas do jogador, validando as entradas e verificando se o
     * puzzle foi resolvido a cada tentativa.
     * 
     * O jogo termina quando o jogador resolve o puzzle ou atinge o número máximo de tentativas.
     * 
     * @param grid A grelha do puzzle que precisa ser resolvido.
     * @param groups Os grupos de números do puzzle.
     * @param maxAttempts O número máximo de tentativas permitidas no jogo.
     * @param scanner O objeto Scanner usado para ler as respostas do jogador.
     * @requires grid e groups devem ser objetos válidos. maxAttempts deve ser um número positivo.
     * @ensures O jogo é jogado até o jogador vencer ou alcançar o número máximo de tentativas.
     */
    public static void play(SumdokuGrid grid, GridGroups groups, int maxAttempts, Scanner scanner) {

        SumdokuGrid playedGrid = new SumdokuGrid(grid.size());

        System.out.println(cluesToString(grid, groups));
        System.out.println("Tens " + maxAttempts + " tentativas para resolver o puzzle. Boa sorte!");

        boolean invalidPlay = true;
        int square = 0;
        int value = 0;
        int gridsize = grid.size();
        
        for(int attempt = 1; attempt<=maxAttempts; attempt++) {
            
            while(invalidPlay) {
                System.out.println("Casa a preencher?");
                square = scanner.nextInt();
                if(square < 1 || square > gridsize * gridsize) {
                    System.out.println("Valor invalido. Tem de estar entre 1 e " + gridsize * gridsize);
                } else {
                    invalidPlay = false;
                }
            }
            invalidPlay = true;
            while(invalidPlay) {
                System.out.println("Valor a colocar?");
                value = scanner.nextInt();
                if(value < 1 || value > gridsize) {
                        System.out.println("Valor invalido. Tem de estar entre 1 e " + gridsize * gridsize);   
                } else {
                    invalidPlay = false;
                }
            }
            invalidPlay = true;
            playedGrid.fill(rowOfSquare(square, gridsize), columnOfSquare(square, gridsize), value);
            System.out.println(playedGrid);
            if (puzzleSolved(playedGrid, grid)) {
                System.out.println("Parabens, resolveste o puzzle!");
                return;
            }
        }
        System.out.println("Tentativas esgotadas. Tenta outra vez!");
    }
}





