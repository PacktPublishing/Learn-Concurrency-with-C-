#include <algorithm>
#include <execution>
#include <print>
#include <random>
#include <ranges>
#include <vector>

constexpr double c = 299'792'458.0;  // speed of light, m/s

auto convertToFrequencyTHz(double wavelenNm) {
  return c * 1e-3 / wavelenNm;
}

using DataVector = std::vector<double>;

auto generateData(unsigned size, double minWavelengthNm,
                  double maxWavelengthNm) {
  auto gen = std::mt19937(std::random_device{}());
  auto dist = std::uniform_real_distribution<double>(
      minWavelengthNm, maxWavelengthNm);
  auto wavelengthsNm = DataVector(size);
  std::ranges::generate(wavelengthsNm,
                        [&] { return dist(gen); });
  return wavelengthsNm;
}

auto processData(const DataVector& wavelengthsNm,
                 double minFreqTHz, double maxFreqTHz) {
  return wavelengthsNm |
         std::views::transform(convertToFrequencyTHz) |
         std::views::filter([=](double f) {
           return f >= minFreqTHz and f <= maxFreqTHz;
         }) |
         std::ranges::to<std::vector>();
}

auto getMinMaxValue(const DataVector& filteredFrequencies) {
  return std::ranges::minmax_element(filteredFrequencies);
}

int main() {
  constexpr auto N = 10'000;
  constexpr double minWavelengthNm = 3.0;
  constexpr double maxWavelengthNm = 750.0;
  constexpr double minFreqTHzExtremeUV = 2500.0;
  constexpr double maxFreqTHzExtremeUV = 30000.0;

  auto wavelengthsNm =
      generateData(N, minWavelengthNm, maxWavelengthNm);
  auto processedData = processData(
      wavelengthsNm, minFreqTHzExtremeUV, maxFreqTHzExtremeUV);
  auto [min, max] = getMinMaxValue(processedData);
  std::println(
      "Extreme UV frequencies detected in range: | {:.2f} THz, "
      "{:.2f} THz | ",
      *min, *max);
  return 0;
}
// Listing 3.6: Performing operations on data with ranges