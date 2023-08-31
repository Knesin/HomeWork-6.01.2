#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <mutex>
#include <iterator>
#include <algorithm>

std::once_flag one;

void print_cpu() {
    std::cout << "Количество аппаратных ядер - " << std::thread::hardware_concurrency() << std::endl;
}

void sum_vector(std::vector<int>::iterator v1, std::vector<int>::iterator v2, const int size, std::vector<int>::iterator v3) {
    std::call_once(one, print_cpu);
    for (int i = 0; i < size; i++) {
        *v3 = *v1 + *v2;
        v1++;
        v2++;
        v3++;
    }

}

int main()
{
    setlocale(LC_ALL, "Russian");

    std::vector<int> v1(1'000'000);
    std::vector<int> v2(1'000'000);
    std::vector<int> v3(1'000'000);
    std::mt19937 gen;
    std::uniform_int_distribution<int> dis(0, 1'000'000);
    auto rand_num([=]() mutable {return dis(gen); });
    std::generate(v1.begin(), v1.end(), rand_num);
    std::generate(v2.begin(), v2.end(), rand_num);

    std::vector<std::thread> trv(16);
    std::vector<std::chrono::duration<double>> result_time;
    int size_for_thred = 0;
    for (int i = 1; i <= 16; i *= 2)
        for (int j = 1000; j <= 1000000; j *= 10)
        {
            auto start = std::chrono::high_resolution_clock::now();
            size_for_thred = j / i;
            for (int k = 0; k < i; k++) {
                trv[k] = std::move(std::thread(sum_vector, v1.begin() + size_for_thred * k, v2.begin() + size_for_thred * k, size_for_thred, v3.begin() + size_for_thred * k));
            }
            for (int k = 0; k < i; k++) {
                    trv[k].join();
            }
            auto end = std::chrono::high_resolution_clock::now();
            result_time.push_back(end - start);
        }
    std::cout << "\t\t1`000\t\t10`000\t\t100`000\t\t1`000`000" << std::endl;
    int thread_count = 1;
    for (int i = 0; i < 5; i++) {
        std::cout << thread_count << " потоков\t";
        thread_count *= 2;
        for (int j = 0; j < 4; j++)
        {
            std::cout << result_time[i * 4 + j].count() << "\t";
        }
        std::cout << std::endl;
     }
}