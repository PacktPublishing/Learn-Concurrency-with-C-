#include <algorithm>
#include <execution>
#include <print>
#include <vector>

constexpr double c = 299'792'458.0;  // speed of light, m/s

auto convertToFrequencyTHz(double wavelenNm) -> double {
  throw std::runtime_error("error when converting");
}
using DataVector = std::vector<double>;

int main() {
  const auto wavelengthsNm =
      DataVector{973.0, 705.9, 884.9, 399.5, 0.0,  // zero!
                 413.8, 675.7, 686.5, 550.5, 877.2};
  auto frequenciesThz = DataVector(wavelengthsNm.size());
  try {
    std::transform(cbegin(wavelengthsNm), cend(wavelengthsNm),
                   begin(frequenciesThz), convertToFrequencyTHz);
  } catch (const std::exception& e) {
    std::println("no-policy error caught: {}", e.what());
  }
  frequenciesThz.clear();
  try {
    std::transform(std::execution::seq, cbegin(wavelengthsNm),
                   cend(wavelengthsNm), begin(frequenciesThz),
                   convertToFrequencyTHz);
  } catch (const std::exception& e) {
    std::println("seq-policy error caught: {}", e.what());
  }
  return 0;
}
// Listing 3.3: execution policy and throwing exceptions