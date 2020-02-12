#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

//Example code taken from https://en.cppreference.com/w/cpp/algorithm/partition
template <class ForwardIt>
void quicksort(ForwardIt first, ForwardIt last)
{
	if(first == last) return;
	auto pivot = *std::next(first, std::distance(first,last)/2);
	ForwardIt middle1 = std::partition(first, last, 
			 [pivot](const auto& em){ return em < pivot; });
	ForwardIt middle2 = std::partition(middle1, last, 
			 [pivot](const auto& em){ return !(pivot < em); });
	quicksort(first, middle1);
	quicksort(middle2, last);
}

using namespace std::chrono;
int main(){
	std::vector<double> times;
	std::vector<long> nums(100'000, 0);
	for(int i = 0; i < 1'000; ++i) {
			std::generate(nums.begin(), nums.end(), &rand);
			milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			quicksort(nums.begin(), nums.end());
			milliseconds later = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			times.push_back(duration_cast<milliseconds>(later - now).count());
	}
	std::cout << std::accumulate(times.begin(), times.end(), 0.0) / times.size() << "\n";
	return 0;
}


