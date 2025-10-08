#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

struct ThreadData {
  long long start_iter;
  long long end_iter;
  std::atomic<double> *shared_sum;
  volatile bool done;
};

void worker_thread_critical(ThreadData *data) {
  double h = 1.0 / 10000000.0; // Asumimos n total para el cálculo de h
  double local_sum = 0.0;
  for (long long i = data->start_iter; i < data->end_iter; ++i) {
    double x = (i + 0.5) * h;
    local_sum += 4.0 / (1.0 + x * x);
  }

  // Sección crítica protegida por atomic
  // Usamos un bucle de compare_exchange para simular fetch_add
  double current = data->shared_sum->load();
  while (
      !data->shared_sum->compare_exchange_weak(current, current + local_sum)) {
    // Si falla, current se actualiza con el valor actual, así que solo
    // reintentamos
  }

  data->done = true;
}

double estimate_pi_busy_waiting_critical(long long n, int num_threads) {
  std::vector<ThreadData> thread_data(num_threads);
  std::vector<std::thread> threads;
  std::atomic<double> shared_sum(0.0);
  long long iterations_per_thread = n / num_threads;

  // Lanzar hilos
  for (int i = 0; i < num_threads; ++i) {
    thread_data[i].start_iter = i * iterations_per_thread;
    thread_data[i].end_iter =
        (i == num_threads - 1) ? n : (i + 1) * iterations_per_thread;
    thread_data[i].shared_sum = &shared_sum;
    thread_data[i].done = false;
    threads.emplace_back(worker_thread_critical, &thread_data[i]);
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

  double h = 1.0 / static_cast<double>(n);
  return h * shared_sum.load();
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Uso: " << argv[0] << " <n_iteraciones> <num_threads>\n";
    return 1;
  }
  long long n = atoll(argv[1]);
  int num_threads = atoi(argv[2]);

  auto start = std::chrono::high_resolution_clock::now();
  double pi = estimate_pi_busy_waiting_critical(n, num_threads);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;

  std::cout << "pi (busy-waiting con sección crítica) = " << pi << std::endl;
  std::cout << "Tiempo: " << elapsed.count() << " segundos" << std::endl;

  return 0;
}
