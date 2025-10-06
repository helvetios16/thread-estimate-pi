#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

struct ThreadData {
  long long start_iter;
  long long end_iter;
  double *shared_sum;
  std::mutex *mtx;
};

void worker_thread_mutex(ThreadData *data) {
  double h = 1.0 / 1000000000.0; // Asumimos n total para el cálculo de h
  double local_sum = 0.0;
  for (long long i = data->start_iter; i < data->end_iter; ++i) {
    double x = (i + 0.5) * h;
    local_sum += 4.0 / (1.0 + x * x);
  }

  // Sección crítica protegida por mutex
  data->mtx->lock();
  *(data->shared_sum) += local_sum;
  data->mtx->unlock();
}

double estimate_pi_mutex(long long n, int num_threads) {
  std::vector<ThreadData> thread_data(num_threads);
  std::vector<std::thread> threads;
  std::mutex mtx;
  double shared_sum = 0.0;
  long long iterations_per_thread = n / num_threads;

  // Lanzar hilos
  for (int i = 0; i < num_threads; ++i) {
    thread_data[i].start_iter = i * iterations_per_thread;
    thread_data[i].end_iter =
        (i == num_threads - 1) ? n : (i + 1) * iterations_per_thread;
    thread_data[i].shared_sum = &shared_sum;
    thread_data[i].mtx = &mtx;
    threads.emplace_back(worker_thread_mutex, &thread_data[i]);
  }

  // Unir hilos (espera a que todos terminen)
  for (auto &t : threads) {
    t.join();
  }

  double h = 1.0 / static_cast<double>(n);
  return h * shared_sum;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Uso: " << argv[0] << " <n_iteraciones> <num_threads>\n";
    return 1;
  }
  long long n = atoll(argv[1]);
  int num_threads = atoi(argv[2]);

  auto start = std::chrono::high_resolution_clock::now();
  double pi = estimate_pi_mutex(n, num_threads);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;

  std::cout << "pi (con mutex) = " << pi << std::endl;
  std::cout << "Tiempo: " << elapsed.count() << " segundos" << std::endl;

  return 0;
}
