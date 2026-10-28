import java.util.Scanner;
/**
 * Classe que implementa uma interface textual do jogo
 * @author Isaura Cid (63761) e Rafael Ribeiro (63733)
 */


public class SumdokuTxt {

    /**
     * Interação com o jogador (no início e fim do jogo), para decidir o tamanho da grelha de Sumdoku a ser jogado (3 ou 5)
     * Verifica se o argumento passado é válido
     * 
     * @param args Argumento passado pelo jogador que, se for válido, vai definir o tamanho da grelha
     */
    public static void main(String[] args) {

        Scanner reader = new Scanner(System.in); //cria um scanner

        
        if (args.length > 0){   //verifica se o tamanho do argumento passado é maior que 0
            int inputValue = Integer.parseInt(args[0]);     //inputValue passa a ser o argumento convertido para inteiro
            RandomSumdokuPuzzle rSP = new RandomSumdokuPuzzle(inputValue);      //cria o objeto random para o tamanho argumentado
            int maxAttempts = inputValue*inputValue;    //número máximo de tentativas
            reader = new Scanner(System.in);
            if (inputValue==3) {    //verifica se o valor argumentado é 3
                SumdokuPuzzle puzzle = rSP.nextPuzzle();    //seleciona o próximo puzzle
                play(puzzle, maxAttempts, reader);  //joga o puzzle selecionado
                if(rSP.hasNextPuzzle()) {   //verifica se há mais puzzles para jogar com o tamanho argumentado
                        System.out.println("Queres tentar resolver um novo puzzle (true/false)?");  //se houver, pergunta ao jogador se quer resolver outro puzzle com o mesmo tamanho
                        boolean userChoice = reader.nextBoolean();      //boolean para verificar a resposta do jogador (true ou false)
                        if(userChoice){     //verifica se a resposta do jogador foi true
                            puzzle = rSP.nextPuzzle();      //se foi true, vai buscar outro puzzle para ser jogado
                            play(puzzle, maxAttempts, reader); 
                        } else if(!userChoice){     //se foi false, vai dar a mensagem a seguir
                            System.out.println("Espero que tenhas gostado. Volta sempre!");
                        } 
                } 
            }
            if (inputValue==5){     //verifica se o valor argumentado é 5
                SumdokuPuzzle puzzle = rSP.nextPuzzle();    //seleciona o próximo puzzle
                play(puzzle, maxAttempts, reader);  //joga o puzzle selecionado
                
            }
            if(inputValue != 3 || inputValue != 5){
            System.out.println("Não existem mais puzzles de tamanho " + inputValue +  " para jogar.");  //se for true, o jogador já jogou todos os puzzles do tamanho argumentado
            }
        } else {
            System.out.println("Insira um argumento (3 ou 5)"); //se o argumento for diferente de 3 ou 5, dá esta mensagem
        }
    }

    /**
     * Interação com o jogador durante o jogo, e implementação do jogo.
     * 
     * @param puzzle Puzzle que vai ser jogado
     * @param maxAttempts Número máximo de tentativas
     * @param reader Scanner criado acima
     */
    static void play(SumdokuPuzzle puzzle, int maxAttempts,Scanner reader){
        SumdokuGrid playedGrid = new SumdokuGrid(puzzle.size());

        System.err.println("\n");   
        System.out.println("Bem vindo ao jogo Sumdoku!");
        System.out.println("As pistas do puzzle:");
        System.out.println(puzzle.cluesToString());
        System.out.println("Tens " + maxAttempts + " tentativas para resolver o puzzle. Boa sorte!");
        
        boolean invalidPlay = true; //boolean para verificar se a jogada é válida
        int square = 0; //a casa
        int value = 0;  //o valor
        int gridSize = puzzle.size();   //o tamanho da grelha
        
        for(int attempt = 1; attempt<=maxAttempts; attempt++) { //até ao número máximo de tentativas
            while (invalidPlay) {   //enquanto a casa a preencher for inválida
                System.out.println("Casa a preencher?");    //imprimir esta pergunta
                square = reader.nextInt();  //ler a casa a preencher
                
                if (square < 1 || square > gridSize * gridSize) {   //verifica se a casa argumentada é menor que 1 ou maior que o número de casas
                    System.out.println("Valor inválido. Tem de estar entre 1 e " + gridSize * gridSize);    //se for, a casa não existe, logo é inválida
                } else if (playedGrid.isFilled(rowOfSquare(square, gridSize), columnOfSquare(square, gridSize))) {      //verifica se a casa selecionada está preenchida
                    System.out.println("Esta casa já está preenchida.");    //se estiver, imprime que a casa já está preenchida
                } else {
                    invalidPlay = false;    //se passar pelas condições acima, a jogada é válida
                }
            }
            invalidPlay = true;
            while(invalidPlay) {    //enquanto o valor para preencher a casa for inválido
                System.out.println("Valor a colocar?"); //imprimir esta pergunta
                value = reader.nextInt();   //ler o valor a preencher
                if(value < 1 || value > gridSize) { //verifica se o valor para preencher é menor que 1 ou maior que o tamanho da grelha
                        System.out.println("Valor inválido. Tem de estar entre 1 e " + gridSize );   //se for, o valor é inválido
                } else {    
                    invalidPlay = false;    //se não for, está entre 1 e gridSize, logo o valor para preencher a casa é válido
                }
            }
            invalidPlay = true;
            playedGrid.fill(rowOfSquare(square, gridSize), columnOfSquare(square, gridSize), value);
            System.out.println(playedGrid.toString());  //se passar pelas condições acima, preenche a casa escolhida com o valor escolhido
            if (puzzle.isSolvedBy(playedGrid)) {    //verifica se o puzzle já foi resolvido na sua totalidade
                System.out.println("Parabéns, resolveste o puzzle!");   //se sim, imprime esta mensagem
                return; //se não, não faz nada
            }
        }
        System.out.println("Opps, tentativas esgotadas!");  //se for até ao número de tentativas, imprime esta mensagem
    }

     /**
     * Uma função que calcula a linha de uma determinada casa da grelha, dado o seu tamanho.
     * 
     * @param square casa cuja linha será calculada
     * @param gridSize tamanho da grelha
     * @return O número da linha em que a casa se encontra
     */
    public static int rowOfSquare (int square, int gridSize) {
        if (square % gridSize == 0) {
            return square / gridSize;
        }
        return square / gridSize + 1;
    }
    /**
     * Uma função que calcula a coluna de uma determinada casa da grelha, dado o seu tamanho.
     * 
     * @param square casa cuja coluna será calculada
     * @param gridSize tamanho da grelha.
     * @return O número da coluna em que a casa se encontra
     */
    public static int columnOfSquare (int square, int gridSize) {
        if (square % gridSize == 0) {
            return gridSize;
        }
        return square % gridSize;
    }










}


