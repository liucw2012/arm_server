import static java.lang.Math.*;

public class MonteCarlo {

	public static void main(String[] args) {
		final int N = 50001;
		int sum = 0; 
		int j = 0;
		while (j < 1000) {
			for(int i = 1; i < N; i++) if(pow(random(), 2) + pow(random(), 2) < 1)
				sum++;
			j++;
		}
		System.out.printf("PI = %f%n", 4.0 * sum / (N - 1));
	}

}
