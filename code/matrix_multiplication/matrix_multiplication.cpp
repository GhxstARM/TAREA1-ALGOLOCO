#include "algorithms/matrix_algorithms.hpp"

#include <windows.h>
#include <psapi.h>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

struct CaseMetadata {
    std::string n = "unknown";
    std::string type = "unknown";
    std::string domain = "unknown";
    std::string sample = "unknown";
};

std::size_t workingSetBytes() {
    PROCESS_MEMORY_COUNTERS_EX counters{};
    counters.cb = sizeof(counters);
    if (!GetProcessMemoryInfo(GetCurrentProcess(),
                              reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters),
                              sizeof(counters))) {
        throw std::runtime_error("No se pudo obtener el uso de memoria del proceso");
    }
    return counters.WorkingSetSize;
}

Matrix readMatrix(const std::filesystem::path& inputPath) {
    std::ifstream input(inputPath);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo de entrada: " + inputPath.string());
    }
    Matrix matrix;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        std::istringstream rowStream(line);
        std::vector<long long> row;
        long long value = 0;
        while (rowStream >> value) row.push_back(value);
        if (!rowStream.eof()) throw std::runtime_error("La matriz contiene un valor invalido");
        if (row.empty()) throw std::runtime_error("La matriz contiene una fila vacia");
        matrix.push_back(std::move(row));
    }
    if (matrix.empty()) throw std::runtime_error("La matriz de entrada no puede estar vacia");
    const std::size_t size = matrix.size();
    for (const auto& row : matrix) {
        if (row.size() != size) throw std::runtime_error("La matriz de entrada debe ser cuadrada");
    }
    return matrix;
}

CaseMetadata parseMetadata(const std::filesystem::path& inputPath) {
    std::string stem = inputPath.stem().string();
    if (stem.size() > 2 && stem.compare(stem.size() - 2, 2, "_1") == 0) {
        stem.resize(stem.size() - 2);
    }
    std::vector<std::string> parts;
    std::size_t begin = 0;
    while (begin <= stem.size()) {
        const std::size_t end = stem.find('_', begin);
        parts.push_back(stem.substr(begin, end - begin));
        if (end == std::string::npos) break;
        begin = end + 1;
    }
    if (parts.size() == 4) return {parts[0], parts[1], parts[2], parts[3]};
    return {};
}

std::string caseName(const std::filesystem::path& inputPath) {
    std::string stem = inputPath.stem().string();
    if (stem.size() > 2 && stem.compare(stem.size() - 2, 2, "_1") == 0) stem.resize(stem.size() - 2);
    return stem;
}

std::string csvField(const std::string& value) {
    std::string escaped = "\"";
    for (const char character : value) {
        if (character == '\"') escaped += "\"\"";
        else escaped += character;
    }
    return escaped + "\"";
}

void writeMatrix(const std::filesystem::path& outputPath, const Matrix& matrix) {
    std::filesystem::create_directories(outputPath.parent_path());
    std::ofstream output(outputPath);
    if (!output) throw std::runtime_error("No se pudo escribir el archivo de salida");
    for (const auto& row : matrix) {
        for (std::size_t j = 0; j < row.size(); ++j) {
            if (j != 0) output << ' ';
            output << row[j];
        }
        output << '\n';
    }
}

void appendMeasurement(const std::filesystem::path& csvPath,
                       const std::string& algorithm,
                       const std::filesystem::path& inputPath,
                       const CaseMetadata& metadata,
                       long long elapsedNanoseconds,
                       long long memoryDeltaBytes,
                       const std::filesystem::path& outputPath) {
    std::filesystem::create_directories(csvPath.parent_path());
    const bool writeHeader = !std::filesystem::exists(csvPath) ||
                             std::filesystem::file_size(csvPath) == 0;
    std::ofstream csv(csvPath, std::ios::app);
    if (!csv) throw std::runtime_error("No se pudo abrir el CSV de mediciones");
    if (writeHeader) {
        csv << "algorithm,input,n,type,domain,sample,time_ns,memory_delta_bytes,status,output_file\n";
    }
    csv << csvField(algorithm) << ',' << csvField(inputPath.string()) << ','
        << csvField(metadata.n) << ',' << csvField(metadata.type) << ','
        << csvField(metadata.domain) << ',' << csvField(metadata.sample) << ','
        << elapsedNanoseconds << ',' << memoryDeltaBytes << ",ok,"
        << csvField(outputPath.string()) << '\n';
}

Matrix execute(const std::string& algorithm, const Matrix& left, const Matrix& right) {
    if (algorithm == "naive") return naiveMultiply(left, right);
    if (algorithm == "strassen") return strassenMultiply(left, right);
    throw std::runtime_error("Algoritmo de multiplicacion no valido: " + algorithm);
}

Matrix runAlgorithm(const std::string& algorithm,
                    const Matrix& left,
                    const Matrix& right,
                    const std::filesystem::path& leftPath,
                    const CaseMetadata& metadata) {
    const std::size_t memoryBefore = workingSetBytes();
    const auto start = std::chrono::steady_clock::now();
    Matrix product = execute(algorithm, left, right);
    const auto finish = std::chrono::steady_clock::now();
    const std::size_t memoryAfter = workingSetBytes();

    if (product.size() != left.size()) {
        throw std::runtime_error("El producto tiene una dimension invalida");
    }
    for (const auto& row : product) {
        if (row.size() != left.size()) throw std::runtime_error("El producto no es cuadrado");
    }

    const std::filesystem::path dataDirectory = leftPath.parent_path().parent_path();
    const std::filesystem::path outputPath = dataDirectory / "matrix_output" /
        (caseName(leftPath) + "_" + algorithm + "_out.txt");
    writeMatrix(outputPath, product);
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
    const long long memoryDelta = static_cast<long long>(memoryAfter) -
                                  static_cast<long long>(memoryBefore);
    appendMeasurement(dataDirectory / "measurements" / "measurements.csv", algorithm, leftPath,
                      metadata, elapsed, memoryDelta, outputPath);
    std::cout << algorithm << ": " << elapsed << " ns, " << memoryDelta << " bytes\n";
    return product;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc < 3 || argc > 4) {
            std::cerr << "Uso: matrix_multiplication <matriz_1> <matriz_2> [naive|strassen|all]\n";
            return 1;
        }
        const std::filesystem::path leftPath = argv[1];
        const std::filesystem::path rightPath = argv[2];
        const std::string selection = argc == 4 ? argv[3] : "all";
        const Matrix left = readMatrix(leftPath);
        const Matrix right = readMatrix(rightPath);
        if (left.size() != right.size()) {
            throw std::runtime_error("Las matrices deben tener la misma dimension");
        }
        const CaseMetadata metadata = parseMetadata(leftPath);

        if (selection == "all") {
            const Matrix naive = runAlgorithm("naive", left, right, leftPath, metadata);
            const Matrix strassen = runAlgorithm("strassen", left, right, leftPath, metadata);
            if (naive != strassen) {
                throw std::runtime_error("Naive y Strassen produjeron resultados distintos");
            }
        } else if (selection == "naive" || selection == "strassen") {
            runAlgorithm(selection, left, right, leftPath, metadata);
        } else {
            throw std::runtime_error("Algoritmo de multiplicacion no valido: " + selection);
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
