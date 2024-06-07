#include <random>
#include <iostream>
#include <fstream>
#include <string>

template <typename D, typename G>
void generate(D& d, G& g, size_t max, const std::string& name)
{
  std::ofstream header(name + ".hpp");
  header << "#pragma once\n"
         << "struct object {\n"
         << "  int x;\n"
         << "  int y;\n"
         << "  int z;\n"
         << "  int d;\n"
         << "};\n"
         << "\n"
         << "const object* begin_data();\n"
         << "const object* end_data();\n";
  std::ofstream impl(name + ".cpp");
  impl << "#include <" << name << ".hpp>\n"
       << "\n"
       << "static const object data[] {\n";
  for (size_t i = 0; i < max; ++i) {
    impl << "  {" << d(g) << ", " << d(g) << ", " << d(g) << ", " << d(g) << "},\n";
  }
  impl << "};\n"
       << "const object* begin_data() { return data; }\n"
       << "const object* end_data() { return data + " << max << "; }\n";
}



int main()
{
  std::random_device r;
  std::mt19937 e(r());
  std::uniform_int_distribution n(0,100000);;
  generate(n, e, 1'000'000, "input_data");
}
