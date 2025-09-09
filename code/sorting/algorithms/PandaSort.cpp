#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>
#include <climits>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

// === Configura aquí el directorio de entrada y salida ===
const fs::path INPUT_DIR  = "code/sorting/data/array_input";
const fs::path OUTPUT_DIR = "code/sorting/data/array_output";

std::vector<int> sortArray(std::vector<int> arr) {
    int n = arr.size();
    if (n <= 1) return arr;
    
    int block_size = std::sqrt(n);
    int block_count = (n + block_size - 1) / block_size;
    std::vector<std::queue<int>> block_queue(block_count);
    
    // Ordenar cada bloque usando selection sort
    for (int i = 0; i < block_count; i++) {
        int l = i * block_size;
        int r = std::min((i + 1) * block_size - 1, n - 1);
        
        // Selection sort en el bloque [l, r]
        for (int j = l; j <= r; j++) {
            int min_idx = j;
            for (int k = j + 1; k <= r; k++) {
                if (arr[k] < arr[min_idx]) {
                    min_idx = k;
                }
            }
            if (min_idx != j) {
                std::swap(arr[j], arr[min_idx]);
            }
        }
        
        // Llenar la cola con elementos ordenados del bloque
        for (int j = l; j <= r; j++) {
            block_queue[i].push(arr[j]);
        }
    }
    
    // Merge de todos los bloques
    for (int i = 0; i < n; i++) {
        int mn = INT_MAX, mni = -1;
        for (int j = 0; j < block_count; j++) {
            if (block_queue[j].empty()) continue;
            int cur = block_queue[j].front();
            if (cur < mn) {
                mni = j;
                mn = cur;
            }
        }
        arr[i] = mn;
        block_queue[mni].pop();
    }
    
    return arr;
}

// ---- Procesar un archivo con QuickSort y guardar en OUTPUT_DIR ----
void procesarArchivo(const fs::path& inputPath, const fs::path& outDir) {
    ifstream in(inputPath);

    vector<int> arr;
    int x;
    while (in >> x) arr.push_back(x);
    in.close();

    size_t memBytes = arr.size() * sizeof(int);
    cout << "Procesando " << inputPath.filename().string()
         << " | n=" << arr.size()
         << " | ~mem=" << (memBytes / (1024.0*1024.0)) << " MB\n";

    auto start = chrono::high_resolution_clock::now();
    arr = sortArray(arr);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // Construir nombre de salida: <nombre_entrada_sin_ext>_out.txt en otro directorio
    fs::path outName = inputPath.stem().string() + "_out" + inputPath.extension().string();
    fs::path outPath = outDir / outName;

    ofstream out(outPath);
    for (int v : arr) out << v << ' ';
    out << '\n';
    out.close();

    cout << "✅ " << outPath.string() << " | tiempo=" << elapsed.count() << " s\n";
}

int main() {
    // Asegura que el directorio de salida exista
    fs::create_directories(OUTPUT_DIR);

    // Recolecta archivos .txt de INPUT_DIR
    vector<fs::directory_entry> files;
    for (const auto& entry : fs::directory_iterator(INPUT_DIR)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            files.push_back(entry);
        }
    }

    // Ordena: primero por tamaño (menor a mayor), luego por fecha (más viejo a más nuevo)
    sort(files.begin(), files.end(),
        [](const fs::directory_entry& a, const fs::directory_entry& b) {
            auto sa = fs::file_size(a.path());
            auto sb = fs::file_size(b.path());
            if (sa == sb) return fs::last_write_time(a) < fs::last_write_time(b);
            return sa < sb;
        });

    auto globalStart = chrono::high_resolution_clock::now();
    for (const auto& e : files) procesarArchivo(e.path(), OUTPUT_DIR);
    auto globalEnd = chrono::high_resolution_clock::now();
    cout << "\nTiempo total: "
         << chrono::duration<double>(globalEnd - globalStart).count()
         << " s\n";

    return 0;
}