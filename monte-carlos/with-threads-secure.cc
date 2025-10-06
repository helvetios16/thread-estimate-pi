#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

struct ThreadData {
  int thread_id;
  int total_points;
  std::atomic<int> *shared_counter;
  volatile bool done;
};

void worker_thread_critical(ThreadData *data) {
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

  // Sección crítica protegida con atomic
  (*data->shared_counter) += local_count;

  data->done = true;
}

double estimate_pi_busy_waiting_critical(int total_points, int num_threads) {
  ThreadData *thread_data = new ThreadData[num_threads];
  std::thread *threads = new std::thread[num_threads];
  std::atomic<int> shared_counter(0);

  int points_per_thread = total_points / num_threads;

  // Lanzar threads
  for (int i = 0; i < num_threads; ++i) {
    thread_data[i].thread_id = i;
    thread_data[i].total_points = points_per_thread;
    thread_data[i].shared_counter = &shared_counter;
    thread_data[i].done = false;
    threads[i] = std::thread(worker_thread_critical, &thread_data[i]);
  }

  // Busy-waiting para esperar a que todos los threads terminen
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

  // Unir threads
  for (int i = 0; i < num_threads; ++i) {
    threads[i].join();
  }

  int total_inside = shared_counter.load();

  delete[] thread_data;
  delete[] threads;

  return 4.0 * total_inside / total_points;
}

int main() {
  const int TOTAL_POINTS = 10000000;
  const int NUM_THREADS = 4;

  auto start = std::chrono::high_resolution_clock::now();
  double pi = estimate_pi_busy_waiting_critical(TOTAL_POINTS, NUM_THREADS);
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;

  std::cout << "Estimación de π (busy-waiting con sección crítica): " << pi
            << std::endl;
  std::cout << "Tiempo: " << elapsed.count() << " segundos" << std::endl;

  return 0;
}
