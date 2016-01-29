public class Helpers {

  private int size;
  private int total;
  private int index;
  private int samples[];

  public Helpers() {
    this.total = 0;
    this.index = 0;
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
      return total / size;
  }

}
