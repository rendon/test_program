import java.io.InputStreamReader;
import java.io.IOException;
import java.util.*;
import java.math.BigInteger;
import java.io.BufferedReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.InputStream;

/** Built using CHelper plug-in, Actual solution is at the top. */
public class Main {
  public static void main(String[] args) {
    InputStream inputStream = System.in;
    OutputStream outputStream = System.out;
    InputReader in = new InputReader(inputStream);
    PrintWriter out = new PrintWriter(outputStream);

    Solver solver = new Solver();

    out.close();
  }
}

class Solver {
  public void solve(int testNumber, InputReader in, PrintWriter out)
  {
  }
}

class InputReader {
  private BufferedReader reader;
  private StringTokenizer tokenizer;

  public InputReader(InputStream stream)
  {
    reader = new BufferedReader(new InputStreamReader(stream));
    tokenizer = null;
  }

  public String next()
  {
    while (tokenizer == null || !tokenizer.hasMoreTokens()) {
      try {
        tokenizer = new StringTokenizer(reader.readLine());
      } catch (IOException e) {
        throw new RuntimeException(e);
      }
    }

    return tokenizer.nextToken();
  }

  public int nextInt()    { return Integer.parseInt(next());  }
  public long nextLong()  { return Long.parseLong(next());    }
}

