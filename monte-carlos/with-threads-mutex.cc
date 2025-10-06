#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

struct ThreadData {
  int thread_id;
  int total_points;
  int *shared_counter;
  std::mutex *mtx;
};

void worker_thread_mutex(ThreadData *data) {
  std::random_device rd;
  std::mt19937 gen(rd() + data->thread_id);
  std::uniform_real_distribution<> dis(0.0, 1.0);

  int local_count = 0;

  for (int i = 0; i < data->total_points; ++i) {
    double x = dis(gen);
    double y = dis(gen);
    if (x * x + y * y <= 1.0) {
      local_count++;
    }
  }

  // Sección crítica protegida con mutex
  data->mtx->lock();
  (*data->shared_counter) += local_count;
  data->mtx->unlock();
}

double estimate_pi_mutex(int total_points, int num_threads) {
  ThreadData *thread_data = new ThreadData[num_threads];
  std::thread *threads = new std::thread[num_threads];
  std::mutex mtx;
  int shared_counter = 0;

  int points_per_thread = total_points / num_threads;

  // Lanzar threads
  for (int i = 0; i < num_threads; ++i) {
    thread_data[i].thread_id = i;
    thread_data[i].total_points = points_per_thread;
    thread_data[i].shared_counter = &shared_counter;
    thread_data[i].mtx = &mtx;
    threads[i] = std::thread(worker_thread_mutex, &thread_data[i]);
  }

  // Unir threads
  for (int i = 0; i < num_threads; ++i) {
    threads[i].join();
  }

  delete[] thread_data;
  delete[] threads;

  return 4.0 * shared_counter / total_points;
}

int main() {
  const int TOTAL_POINTS = 10000000;
  const int NUM_THREADS = 4;

  auto start = std::chrono::high_resolution_clock::now();
  double pi = estimate_pi_mutex(TOTAL_POINTS, NUM_THREADS);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;

  std::cout << "Estimación de π (con mutex): " << pi << std::endl;
  std::cout << "Tiempo: " << elapsed.count() << " segundos" << std::endl;

  return 0;
}
