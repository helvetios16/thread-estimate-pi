#include <chrono>
#include <iostream>
#include <random>

double estimate_pi_secuential(int total_points) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  int points_inside_circle = 0;

  for (int i = 0; i < total_points; ++i) {
    double x = dis(gen);
    double y = dis(gen);
    if (x * x + y * y <= 1.0)
      points_inside_circle++;
  }

  return 4.0 * points_inside_circle / total_points;
}

int main(int argc, char *argv[]) {
  const int TOTAL_POINTS = 10000000;

  auto start = std::chrono::high_resolution_clock::now();
  double pi = estimate_pi_secuential(TOTAL_POINTS);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;

  std::cout << "Estimación de π (secuencial): " << pi << std::endl;
  std::cout << "Tiempo: " << elapsed.count() << " segundos" << std::endl;

  return 0;
}
