#include <complex>

#include "BoxBlur.h"
#include "BenchmarkHelper.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "USAGE: bb-bench [version]" << std::endl;
    std::cout << "       versions:" << std::endl;
    std::cout << "          -naive" << std::endl;
    std::cout << "          -expert" << std::endl;
    std::cout << "          -porcupine" << std::endl;
    std::exit(1);
  }

  size_t poly_modulus_degree = 2 << 12;
  size_t size = std::sqrt(poly_modulus_degree / 2);
  std::vector<int> img;
  getInputMatrix(size, img);

  BENCH_FUNCTION(BoxBlur, naive, encryptedFastBoxBlur2x2, img);
  BENCH_FUNCTION(BoxBlur, expert, encryptedBatchedBoxBlur, img);
  BENCH_FUNCTION(BoxBlur, porcupine, encryptedBatchedBoxBlur_Porcupine, img);

  return 0;
}
