public class Helpers {

  private int size;
  private int total;
  private int index;
  private int samples[];

  public Helpers() {
    this.total = 0;
    this.index = 0;
    println("Help activated");
  }

  //Moving average stuff
  public void movingAverage(int size){
    this.size = size;
    samples = new int[size];
    for (int i = 0; i < size; i++) samples[i] = 0;
  }

  public void add(int x) {
      total -= samples[index];
      samples[index] = x;
      total += x;
      if (++index == size) index = 0; // cheaper than modulus
  }

  public int getAverage() {
      // println("Total: " + total + "size: " + size);
      return total / size;
  }

  public void shiftArray(int array[]) {
    for (int i = (array.length - 2); i >= 0; i--) {
      array[i+1] = array[i];
    }
  }
}
