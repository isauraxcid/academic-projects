package types;

public class Tower {
    
    public static final String EOL = System.lineSeparator(); 
    public static final int DEFAULT_HEIGHT = 8; 

    /**
     * 
     */
    public Tower() {
        
    }

    /**
     * 
     * @param height
     */
    public Tower(int height) {
        
    }

    /**
     * @param d
     */
    public void addToTower(Disk d) {

    }

    /**
     * 
     * @return
     */
    public boolean isFull() {
        return false;
    }

    /**
     * 
     */
    public void removeFromTower() {

    }

    /**
     * 
     * @param d
     * @return
     */
    public boolean isValidMove(Disk d) {
        return false;
    }

    /**
     * 
     * @return
     */
    public Disk viewTopDisk() {
        return null;
    }

    /**
     * 
     * @return
     */
    public boolean isEmpty() {
        return false;
    }

    /**
     * 
     * @return
     */
    public int height() {
        return 0;
    }

    /**
     * 
     * @return
     */
    public int numberOfDisks() {
        return 0;
    }

    @Override
    public String toString() {
        return "";
    }
}