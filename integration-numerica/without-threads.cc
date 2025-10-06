#include <chrono>
#include <iostream>

double estimate_pi_sequential(long long n) {
  double h = 1.0 / static_cast<double>(n);
  double sum = 0.0;

  for (long long i = 0; i < n; i++) {
    double x = (i + 0.5) * h;
    sum += 4.0 / (1.0 + x * x);
  }

  return h * sum;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <n_iteraciones>\n";
    return 1;
  }
  long long n = atoll(argv[1]);

  auto start = std::chrono::high_resolution_clock::now();
  double pi = estimate_pi_sequential(n);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;

  std::cout << "pi (secuencial) = " << pi << std::endl;
  std::cout << "Tiempo: " << elapsed.count() << " segundos" << std::endl;

  return 0;
}
