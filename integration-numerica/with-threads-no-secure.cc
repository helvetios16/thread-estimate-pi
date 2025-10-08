#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

struct ThreadData {
  long long start_iter;
  long long end_iter;
  double partial_sum;
  volatile bool done;
};

void worker_thread(ThreadData *data) {
  double h = 1.0 / 10000000.0; // Asumimos n total para el cálculo de h
  data->partial_sum = 0.0;
  for (long long i = data->start_iter; i < data->end_iter; ++i) {
    double x = (i + 0.5) * h;
    data->partial_sum += 4.0 / (1.0 + x * x);
  }
  data->done = true;
}

double estimate_pi_busy_waiting(long long n, int num_threads) {
  std::vector<ThreadData> thread_data(num_threads);
  std::vector<std::thread> threads;
  long long iterations_per_thread = n / num_threads;

  // Lanzar hilos
  for (int i = 0; i < num_threads; ++i) {
    thread_data[i].start_iter = i * iterations_per_thread;
    thread_data[i].end_iter =
        (i == num_threads - 1) ? n : (i + 1) * iterations_per_thread;
    thread_data[i].done = false;
    threads.emplace_back(worker_thread, &thread_data[i]);
  }

  // Busy-waiting
  bool all_done = false;
  while (!all_done) {
    all_done = true;
    for (int i = 0; i < num_threads; ++i) {
      if (!thread_data[i].done) {
        all_done = false;
        break;
      }
    }
  }

  // Unir hilos
  for (auto &t : threads) {
    t.join();
  }

  // Combinar resultados
  double total_sum = 0.0;
  for (int i = 0; i < num_threads; ++i) {
    total_sum += thread_data[i].partial_sum;
  }

  double h = 1.0 / static_cast<double>(n);
  return h * total_sum;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Uso: " << argv[0] << " <n_iteraciones> <num_threads>\n";
    return 1;
  }
  long long n = atoll(argv[1]);
  int num_threads = atoi(argv[2]);

  auto start = std::chrono::high_resolution_clock::now();
  double pi = estimate_pi_busy_waiting(n, num_threads);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;

  std::cout << "pi (busy-waiting) = " << pi << std::endl;
  std::cout << "Tiempo: " << elapsed.count() << " segundos" << std::endl;

  return 0;
}
