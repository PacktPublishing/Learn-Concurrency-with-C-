#include <algorithm>
#include <print>
#include <vector>

constexpr double c = 299'792'458.0;  // speed of light, m/s

auto convertToFrequencyTHz(double wavelenNm) {
  return c * 1e-3 / wavelenNm;
}
using DataVector = std::vector<double>;

void printMeasurementData(const DataVector& wavelengthsNm,
                          const DataVector& frequenciesThz) {
  std::println("{:^15} | {:^15}", "Wavelength (nm)",
               "Frequency (THz)");
  std::println("{:-^32}", "");

  for (auto i = 0; i < wavelengthsNm.size(); ++i) {
    std::println("{:<15.2f} | {:>15.2f}", wavelengthsNm[i],
                 frequenciesThz[i]);
  }
}

int main() {
  const auto wavelengthsNm =
      DataVector{973.0, 705.9, 884.9, 399.5, 484.6,
                 413.8, 675.7, 686.5, 550.5, 877.2};
  auto frequenciesThz = DataVector{};
  frequenciesThz.reserve(wavelengthsNm.size());
  std::transform(cbegin(wavelengthsNm), cend(wavelengthsNm),
                 std::back_inserter(frequenciesThz),
                 convertToFrequencyTHz);
  printMeasurementData(wavelengthsNm, frequenciesThz);
  return 0;
}
// Listing 3.1: use std::transform without execution policy